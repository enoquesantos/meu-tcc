#include "pluginmanager.h"
#include "app.h"
#include "utils.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtAlgorithms>
#include <QTextStream>

#ifdef QT_DEBUG
#include <QDebug>
#endif

PluginManager::PluginManager(QObject *parent) : QObject(parent)
  ,m_forceClearCache(false)
{
}

QDir PluginManager::pluginsDir()
{
#ifdef Q_OS_ANDROID
    QDir dir(QStringLiteral("assets:/plugins"));
    dir.cd(QStringLiteral("plugins"));
    return dir;
#elif defined(Q_OS_LINUX)
    QDir dir(qApp->applicationDirPath());
    dir.cd("plugins");
    return dir;
#else
    QDir dir(qApp->applicationDirPath());
    // for osx desktop
#ifndef Q_OS_IOS
    dir.cd(QStringLiteral("../../../"));
#endif
    dir.cd(QStringLiteral("plugins"));
    return dir;
#endif
}

void PluginManager::setApp(App *app)
{
    m_app = app;
}

void PluginManager::setForceClearCache(bool forceClearCache)
{
    m_forceClearCache = forceClearCache;
}

void PluginManager::loadPlugins()
{
    QString savedVersion(m_app->readSetting(QStringLiteral("version")).toString());

    // QApplication::applicationVersion() return the current application version:
    // 1: In android, return the value for android:versionName property of the AndroidManifest.xml manifest element
    // 2: In iOS, return the value for CFBundleVersion property of the information property list
    // 3: On other platforms, the default is the empty string (linux desktop).
    QString applicationVersion(QApplication::applicationVersion());

    // we need to check the saved version with the current running version.
    // if the execution version is the same value of saved version and 'm_forceClearCache'
    // is not set to true, nothing to be done. Otherwise, all cached data needs to be removed to be recreated
    if (!savedVersion.isEmpty() && !applicationVersion.isEmpty() && savedVersion.compare(applicationVersion) == 0 && !m_forceClearCache) {
        #ifdef QT_DEBUG
            qDebug() << QStringLiteral("Plugins ready from cache!");
        #endif
        emit finished(this);
    } else {
        #ifdef QT_DEBUG
            qDebug() << QStringLiteral("Application was updated or force to load new plugins.");
        #endif

        // if the app was updated, all qml cached files will be deleted!
        // and all plugins will be reloaded and saved.
        clearCache();

        QDir dir(PluginManager::pluginsDir());

        QString pluginAbsPath;
        QVariantMap pluginJson;
        foreach (const QString &pluginDir, dir.entryList()) {
            QFile file(dir.absoluteFilePath(pluginDir + QStringLiteral("/config.json")));
            if (file.exists()) {
                // set the plugin absolute path
                pluginAbsPath = QFileInfo(file.fileName()).absolutePath();

                // load the plugin config.json file
                pluginJson = Utils::instance()->readFile(file.fileName()).toMap();

                // add the plugin absolute path to plugin json
                pluginJson.insert(QStringLiteral("absPath"), pluginAbsPath);

                // append all listeners from current plugin if available
                if (pluginJson.contains(QStringLiteral("listeners"))) {
                    foreach (const QString &item, pluginJson.value(QStringLiteral("listeners")).toStringList()) {
                        // when application running in desktop mode
                        // prepend "file:/" to absolute listener path to be loaded dynamically without in qrc
                        #ifdef Q_OS_ANDROID
                            m_pluginsListeners << pluginAbsPath + "/" + item;
                        #else
                            m_pluginsListeners << QStringLiteral("file://") + pluginAbsPath + QStringLiteral("/") + item;
                        #endif
                    }
                }

                createDatabaseTables(pluginAbsPath);
                parsePages(pluginJson);
            }
        }

        sortPages();
        save();
        emit finished(this);
    }
}

bool PluginManager::sortByKey(const QVariant &a, const QVariant &b)
{
    static QString order(QStringLiteral("order"));
    return a.toMap().value(order).toInt() > b.toMap().value(order).toInt();
}

void PluginManager::save()
{
    m_app->saveSetting(QStringLiteral("pages"), m_pages);
    m_app->saveSetting(QStringLiteral("listeners"), m_pluginsListeners);
    m_app->saveSetting(QStringLiteral("version"), QApplication::applicationVersion());

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("Application plugins was saved success!");
    #endif
}

void PluginManager::clearCache()
{
    // get the application directory cache
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/qmlcache/"));

    // iterate in cache directory content to delete all files
    QStringList cacheFiles(dir.entryList({QStringLiteral("*.qmlc"), QStringLiteral("*.jsc")}, QDir::Files));

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("Application writableLocation path  is: ") + dir.absolutePath();
        qDebug() << QStringLiteral("Application cache files size: ") + cacheFiles.size();
    #endif

    foreach (const QString &filePath, cacheFiles) {
        QFile file(dir.absolutePath() + "/" + filePath);
        file.setPermissions(QFileDevice::WriteUser);
        file.remove();
    }

    cacheFiles.clear();

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("All application cached files was success deleted!");
    #endif

    m_app->removeSetting(QStringLiteral("pages"));
    m_app->removeSetting(QStringLiteral("listeners"));
    m_app->removeSetting(QStringLiteral("version"));
}

void PluginManager::createDatabaseTables(const QString &pluginDirPath)
{
    if (!QFile::exists(pluginDirPath + QStringLiteral("/plugin_table.sql")))
        return;

    #ifdef QT_DEBUG
        qDebug() << QStringLiteral("Application plugin has a sql file at: ") + pluginDirPath;
    #endif

    auto *worker = new Private::PluginDatabaseTableCreator(pluginDirPath + QStringLiteral("/plugin_table.sql"), this);
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

void PluginManager::parsePages(const QVariantMap &pluginJson)
{
    QVariantMap page;
    QString pluginAbsPath(pluginJson.value(QStringLiteral("absPath")).toString());

    foreach (auto item, pluginJson.value(QStringLiteral("pages")).toList()) {
        page = item.toMap();

        // the 'absPath' (absolute path) is useful to menu make binds with current page
        // turning the item selected (background color) when is the active item.
        // when application is running on ios, osx or linux desktop,
        // we need to prepend "file:/" to absolute path
        #ifndef Q_OS_ANDROID
            page.insert(QStringLiteral("absPath"), QStringLiteral("file://") + pluginAbsPath + QStringLiteral("/") + page.value(QStringLiteral("qml")).toString());
        #else
            page.insert(QStringLiteral("absPath"), pluginAbsPath + "/" + page.value(QStringLiteral("qml")).toString());
        #endif

        // set defaults properties if not set by page.
        if (!page.contains(QStringLiteral("order")))
            page.insert(QStringLiteral("order"), 0);
        if (!page.contains(QStringLiteral("showInDrawer")))
            page.insert(QStringLiteral("showInDrawer"), false);
        if (!page.contains(QStringLiteral("showInTabBar")))
            page.insert(QStringLiteral("showInTabBar"), false);
        if (!page.contains(QStringLiteral("roles")))
            page.insert(QStringLiteral("roles"), QJsonArray());

        // if the page is a login page (set the property "isLogin" to true)
        // save the absolute path to this page to qml application load the login page.
        // This feature is to reduce the hard coded set what's the login page and the home page (initial page after login)
        if (page.contains(QStringLiteral("isLoginPage")))
            m_app->saveSetting(QStringLiteral("loginPageUrl"), page.value(QStringLiteral("absPath")).toString());
        else if (page.contains(QStringLiteral("isHomePage")))
            m_app->saveSetting(QStringLiteral("homePageUrl"), page.value(QStringLiteral("absPath")).toString());

        m_pages << page;
    }
}
