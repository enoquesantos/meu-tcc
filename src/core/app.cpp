#include "app.h"
#include "pluginmanager.h"
#include "utils.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QJSValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QQuickStyle>
#include <QSettings>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras>
#elif defined Q_OS_IOS
// pass
#else
// used to set the application window icon in desktop mode
#include <QIcon>
#endif

App* App::m_instance = nullptr;

App::App(QObject *parent) : QObject(parent)
{
    App::m_instance = this;
    init();
}

App::~App()
{
    if (m_config.size())
        m_config.clear();
}

void App::init()
{
    // read the config.json file
    m_config = Utils::instance()->readFile(QStringLiteral(":/config.json")).toMap();

    // the default events names used by application qml components
    QStringList defaultEvents({
        QStringLiteral("appendOptionPage"),         // evt 1
        QStringLiteral("cameraImageSaved"),         // evt 2
        QStringLiteral("cancelSearch"),             // evt 3
        QStringLiteral("imageFileSelected"),        // evt 4
        QStringLiteral("initUserProfile"),          // evt 5
        QStringLiteral("logoutUser"),               // evt 6
        QStringLiteral("newActionNotification"),    // evt 7
        QStringLiteral("newPushNotification"),      // evt 8
        QStringLiteral("newPushNotificationToken"), // evt 9
        QStringLiteral("openDrawer"),               // evt 10
        QStringLiteral("popCurrentPage"),           // evt 11
        QStringLiteral("setUserProperty"),          // evt 12
        QStringLiteral("updateUserProfile"),        // evt 13
        QStringLiteral("userProfileChanged")        // evt 14
    });

    // initialize a temporary list with the events names defined by user
    // append or override default events names if already defined by user
    defaultEvents << m_config.value(QStringLiteral("events")).toStringList();
    QVariantMap map;
    foreach(const QString &eventName, defaultEvents)
        map.insert(eventName, eventName);
    m_config.insert(QStringLiteral("events"), map);

    QApplication::setApplicationName(m_config.value(QStringLiteral("applicationName")).toString());
    QApplication::setOrganizationName(m_config.value(QStringLiteral("organizationName")).toString());
    QApplication::setOrganizationDomain(m_config.value(QStringLiteral("organizationDomain")).toString());

    m_qsettings = new QSettings(this);

    /**
     * @brief QQuickStyle::setStyle
     * set the quick controls style. The 'applicationStyle' key needs to be a string with the style Name:
     * Material, Universal or Default.
     * @link qthelp://org.qt-project.qtquickcontrols2.591/qtquickcontrols2/qquickstyle.html
     */
    QQuickStyle::setStyle(m_config.value(QStringLiteral("applicationStyle")).toString());

    /**
     * @brief m_pluginManager
     * This object manage the application plugins and require a reference to
     * qsettings object to save the plugins pages, application version and others data.
     * PluginManager compare the app version value from previous saved value using QApplication::ApplicationVersion()
     * to decide if remove all qml cached files (*.qmlc and *.jsc) to refresh new changes
     */
    PluginManager pluginManager(this);
    pluginManager.setApp(this);
    pluginManager.loadPlugins();

    setPluginsPaths();

#ifdef Q_OS_ANDROID
    // pass
#elif defined Q_OS_IOS
    // pass
#else
    // set application icon if running in desktop linux or osx
    qApp->setWindowIcon(QIcon::fromTheme(":/icon.png"));
    QApplication::addLibraryPath(qApp->applicationDirPath() + "/plugins");
#endif
}

QVariantMap App::config() const
{
    return m_config;
}

void App::setPluginsPaths()
{
    m_config.insert(QStringLiteral("plugins"), readSetting(QStringLiteral("pluginsPaths"), settingTypeJsonObject));
}

QVariant App::readSetting(const QString &key, quint8 returnType)
{
    if (key.isEmpty())
        return 0;
    QVariant value(m_qsettings->value(key, QLatin1String("")));
    if (returnType == settingTypeInt)
        return value.toInt();
    else if (returnType == settingTypeBool)
        return value.toBool();
    else if (returnType == settingTypeString)
        return value.toString();
    else if (returnType == settingTypeStringList)
        return value.toStringList();
    else if (returnType == settingTypeJsonArray)
        return QJsonDocument(QJsonDocument::fromJson(value.toByteArray())).array().toVariantList();
    else if (returnType == settingTypeJsonObject)
        return QJsonDocument(QJsonDocument::fromJson(value.toByteArray())).object().toVariantMap();
    else
        return value;
}

void App::saveSetting(const QString &key, const QVariant &value)
{
    QJsonDocument jsonDocument(QJsonDocument::fromVariant(value));
    if (value.typeName() == QStringLiteral("QJSValue"))
        jsonDocument = QJsonDocument::fromVariant(value.value<QJSValue>().toVariant());
    // if the value send is a valid json array or object, save as json serialized
    if (jsonDocument.isObject() || jsonDocument.isArray())
        m_qsettings->setValue(key, jsonDocument.toJson(QJsonDocument::Compact));
    else // if is not a valid json, save as variant
        m_qsettings->setValue(key, value);
}

void App::removeSetting(const QString &key)
{
    if (m_qsettings->contains(key))
        m_qsettings->remove(key);
}

void App::fireEventNotify(const QString &eventName, const QString &eventData)
{
    if (App::m_instance == nullptr)
        return;
    emit App::m_instance->eventNotify(eventName, eventData);
}

App *App::instance()
{
    return m_instance;
}
