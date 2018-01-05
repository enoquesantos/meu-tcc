#include "app.h"
#include "pluginmanager.h"
#include "notification.h"
#include "utils.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QJSValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkConfigurationManager>
#include <QQuickStyle>
#include <QSettings>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras>
#elif defined(Q_OS_LINUX)
// used to set the application window icon in desktop mode
#include <QIcon>
#endif

App* App::m_instance = nullptr;

App::App(QObject *parent) : QObject(parent)
  ,m_pluginManager(new PluginManager(this))
{
    App::m_instance = this;

    // a connection to delete the plugin manager when it is no longer needed
    connect(m_pluginManager, &PluginManager::finished, [this](PluginManager *pm) {
        pm->deleteLater();
    });

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
    QStringList defaultEventsNames({
        QStringLiteral("cameraImageSaved"),
        QStringLiteral("cancel"),
        QStringLiteral("eulaAccepted"),
        QStringLiteral("goBack"),
        QStringLiteral("newPushMessage"),
        QStringLiteral("newIntentPushMessage"),
        QStringLiteral("openDrawer"),
        QStringLiteral("drawerClosed"),
        QStringLiteral("pushNotificationToken"),
        QStringLiteral("refreshDrawerPages"),
        QStringLiteral("requestUpdateUserProfile"),
        QStringLiteral("saveUserProfile"),
        QStringLiteral("submitForm"),
        QStringLiteral("userProfileChanged")
    });

    // initialize a temporary map with the events names defined by user 
    QVariantMap map(m_config.value(QStringLiteral("events")).toMap());
    // append or override default events names if already defined by user
    foreach(const QString &eventName, defaultEventsNames)
        map.insert(eventName, eventName);
    // replace the config events by default events names + user events
    m_config.insert(QStringLiteral("events"), map);

    QApplication::setApplicationName(m_config.value(QStringLiteral("appName")).toString());
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

    m_pluginManager->setApp(this);
    m_pluginManager->loadPlugins();
    setPluginsPaths();

#ifdef Q_OS_LINUX
    #ifndef Q_OS_ANDROID
        // set application icon if running at desktop linux or osx
        qApp->setWindowIcon(QIcon::fromTheme(":/app_icon.png"));
        QApplication::addLibraryPath(qApp->applicationDirPath() + "/plugins");
    #endif
#endif
}

bool App::isDeviceOnline()
{
    QNetworkConfigurationManager qcm(this);
    return qcm.isOnline();
}

void App::sendNotification(const QString &title, const QString &message, const QString &actionName, const QVariant &actionValue)
{
    Notification notification(this);
    notification.sendNotification(title, message, actionName, actionValue);
}

QVariantMap App::config()
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

void App::minimize()
{
#ifdef Q_OS_ANDROID
    QtAndroid::androidActivity().callMethod<jboolean>("moveTaskToBack", "(Z)Z", true);
#endif
}

void App::fireEventNotify(const QString &eventName, const QString &eventData)
{
    if (App::m_instance == nullptr)
        return;
    App::m_instance->eventNotify(eventName, eventData);
}
