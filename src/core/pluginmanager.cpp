#include "pluginmanager.h"
#include "plugindatabasetablecreator.h"
#include "settings.h"
#include "utils.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtAlgorithms>
#include <QThread>

#ifdef QT_DEBUG
#include <QDebug>
#endif

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{
}

QDir PluginManager::pluginsDir()
{
    static QDir pluginsDir;

#ifdef Q_OS_ANDROID
    pluginsDir.setPath(QStringLiteral("assets:/plugins"));
#else
    pluginsDir.setPath(qApp->applicationDirPath());
#endif

#ifdef Q_OS_DARWIN
    // for osx desktop
    pluginsDir.cd(QStringLiteral("../../../"));
#endif
    pluginsDir.cd(QStringLiteral("plugins"));
    return pluginsDir;
}

void PluginManager::setSettings(Settings *settings)
{
    m_settings = settings;
}

void PluginManager::loadPlugins()
{
    QString savedVersion(m_settings->readSetting(QStringLiteral("version")).toString());

    // read the plugins directory path
    QDir pluginsDir(PluginManager::pluginsDir());
    pluginsDir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);

    // keeps the plugin root absolute path
    QString pluginsDirPath(pluginsDir.absolutePath() + QStringLiteral("/"));

    // the list of plugins folders in plugins root directory
    QStringList pluginsDirs(pluginsDir.entryList());

    // QApplication::applicationVersion() return the current application version:
    // 1: In android, return the value for android:versionName property of the AndroidManifest.xml manifest element
    // 2: In iOS, return the value for CFBundleVersion property of the information property list (from in ios Info.plist)
    // 3: On other platforms, the default is a empty string (linux desktop).
    QString applicationVersion(QApplication::applicationVersion());

    // if the application is not running in DEBUG mode
    // the plugins will be reloaded only if the application are updated
#ifndef QT_DEBUG
    // check the saved version with the current running version.
    // if the current running version is the same value of saved version
    // nothing to be done. Otherwise, all cached data needs to be removed to be recreated and plugins and pages will be re-reload.
    if (!savedVersion.isEmpty() && !applicationVersion.isEmpty() && savedVersion.compare(applicationVersion) == 0)
        return;
#endif
    clearCache();

    QString pluginAbsPath;
    QVariantMap pluginJson;
    Utils *utils = Utils::instance();
    foreach (const QString &pluginDirName, pluginsDirs) {
        QFile file(pluginsDirPath + pluginDirName + QStringLiteral("/config.json"));
        // set the config.json absolute path for current plugin
        pluginAbsPath = QFileInfo(file.fileName()).absolutePath();
        if (file.exists()) {
            // load the config.json
            pluginJson = utils->readFile(file.fileName()).toMap();

            if (pluginJson.isEmpty())
                continue;

            // append the plugin path with plugin name to be added in Config.json to be read by qml plugins
            #if defined Q_OS_ANDROID || defined Q_OS_IOS
                m_pluginsPaths.insert(pluginDirName.toLower(), pluginAbsPath + QStringLiteral("/"));
            #else
                m_pluginsPaths.insert(pluginDirName.toLower(), QStringLiteral("file://") + pluginAbsPath + QStringLiteral("/"));
            #endif

            // append all listeners from current plugin (if available)
            if (pluginJson.contains(QStringLiteral("listeners"))) {
                QStringList listeners(pluginJson.value(QStringLiteral("listeners")).toStringList());
                foreach (const QString &item, listeners) {
                    // when application running in desktop mode we need to prepend
                    // the "file:/" in absolute listener path
                    #if defined Q_OS_ANDROID || defined Q_OS_IOS
                        m_listeners << pluginAbsPath + QStringLiteral("/") + item;
                    #else
                        m_listeners << QStringLiteral("file://") + pluginAbsPath + QStringLiteral("/") + item;
                    #endif
                }
            }
            createDatabaseTables(pluginAbsPath);
            parsePages(pluginAbsPath, pluginJson);
        }
    }

    sortPages();
    save();
}

bool PluginManager::sortByKey(const QVariant &a, const QVariant &b)
{
    static QString order(QStringLiteral("order"));
    return a.toMap().value(order).toInt() > b.toMap().value(order).toInt();
}

void PluginManager::save()
{
    m_settings->saveSetting(QStringLiteral("listeners"), m_listeners);
    m_settings->saveSetting(QStringLiteral("pages"), m_pages);
    m_settings->saveSetting(QStringLiteral("pluginsPaths"), m_pluginsPaths);
    m_settings->saveSetting(QStringLiteral("version"), QApplication::applicationVersion());

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("Application plugins, listeners and version was saved success!");
    #endif
}

void PluginManager::clearCache()
{
    // get the application directory cache
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/qmlcache/"));
    QString	absoluteDirPath(dir.absolutePath());

    // iterate in cache directory content to delete all files
    QStringList cacheFiles(dir.entryList({QStringLiteral("*.qmlc"), QStringLiteral("*.jsc")}, QDir::Files));

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("Application writable location path is %1").arg(absoluteDirPath);
        qDebug() << QStringLiteral("Application cache files contains %1 files").arg(cacheFiles.size());
    #endif

    foreach (const QString &filePath, cacheFiles) {
        QFile file(absoluteDirPath + QStringLiteral("/") + filePath);
        file.setPermissions(QFileDevice::WriteUser);
        file.remove();
    }

    cacheFiles.clear();

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("All application cached files was success deleted!");
    #endif

    m_settings->removeSetting(QStringLiteral("listeners"));
    m_settings->removeSetting(QStringLiteral("pages"));
    m_settings->removeSetting(QStringLiteral("pluginsPaths"));
    m_settings->removeSetting(QStringLiteral("version"));
}

void PluginManager::createDatabaseTables(const QString &pluginDirPath)
{
    if (!QFile::exists(pluginDirPath + QStringLiteral("/plugin_table.sql")))
        return;

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("Found a a sql file for %1 plugin").arg(pluginDirPath);
    #endif

    auto *worker = new PluginDatabaseTableCreator(pluginDirPath + QStringLiteral("/plugin_table.sql"), this);
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void PluginManager::sortPages()
{
    // the qSort function is deprecated after Qt 5.8 and can be removed in some future update.
    QVariantList plugins(m_pages);
    qSort(plugins.begin(), plugins.end(), this->sortByKey);
    m_pages = plugins;
    plugins.clear();
}

void PluginManager::parsePages(const QString &pluginPath, const QVariantMap &pluginConfig)
{
    QString pagePath;
    QVariantMap page;
    QVariantList pages(pluginConfig.value(QStringLiteral("pages")).toList());
    foreach (const QVariant &item, pages) {
        page = item.toMap();

        // the absPath (absolute path) is useful to menu make binds with current page
        // turning the item selected (change the background color) when is the active item.
        pagePath = pluginPath + QStringLiteral("/") + page.value(QStringLiteral("qml")).toString();

        // when application is running on ios, osx or linux desktop,
        // we need to prepend "file://" to absolute path
        #ifndef Q_OS_ANDROID
            pagePath = pagePath.prepend(QStringLiteral("file://"));
        #endif
        page.insert(QStringLiteral("absPath"), pagePath);

        // set defaults properties if not set by page.
        if (!page.contains(QStringLiteral("order")))
            page.insert(QStringLiteral("order"), 0);
        if (!page.contains(QStringLiteral("showInDrawer")))
            page.insert(QStringLiteral("showInDrawer"), false);
        if (!page.contains(QStringLiteral("showInTabBar")))
            page.insert(QStringLiteral("showInTabBar"), false);
        if (!page.contains(QStringLiteral("roles")))
            page.insert(QStringLiteral("roles"), QVariantList());

        // if the page is a login page (set the property "isLogin" to true)
        // save the absolute path to this page to qml application load the login page.
        // This feature is to reduce the hard coded set what's the login page and the home page (initial page after login)
        if (page.contains(QStringLiteral("isLoginPage")))
            m_settings->saveSetting(QStringLiteral("loginPageUrl"), pagePath);
        else if (page.contains(QStringLiteral("isHomePage")))
            m_settings->saveSetting(QStringLiteral("homePageUrl"), pagePath);

        m_pages << page;
    }
}
