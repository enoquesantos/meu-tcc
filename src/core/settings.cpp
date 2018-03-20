#include "settings.h"
#include "utils.h"

#include <QApplication>
#include <QIcon>
#include <QJSValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QQuickStyle>
#include <QSettings>

Settings::Settings(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_ANDROID
_IS_ANDROID = true;
_IS_MOBILE  = true;
#elif defined Q_OS_IOS
_IS_IOS     = true;
_IS_MOBILE  = true;
#elif defined Q_OS_DARWIN
_IS_OSX     = true;
#elif defined Q_OS_LINUX
_IS_LINUX   = true;
#elif defined Q_OS_WIN
_IS_WINDOWS = true;
#endif
    init();
}

Settings::~Settings()
{
    if (m_config.size())
        m_config.clear();
}

void Settings::init()
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

    qApp->setWindowIcon(QIcon::fromTheme(QStringLiteral(":/icon.png")));
    QApplication::addLibraryPath(qApp->applicationDirPath() + QStringLiteral("/plugins"));
}

QVariantMap Settings::config() const
{
    return m_config;
}

void Settings::setPluginsPaths()
{
    m_config.insert(QStringLiteral("plugins"), read(QStringLiteral("pluginsPaths"), _TypeJsonObject));
}

QVariant Settings::read(const QString &key, quint8 returnType)
{
    if (key.isEmpty())
        return 0;
    QVariant value(m_qsettings->value(key, QLatin1String("")));
    if (returnType == _TypeInt)
        return value.toInt();
    else if (returnType == _TypeBool)
        return value.toBool();
    else if (returnType == _TypeString)
        return value.toString();
    else if (returnType == _TypeStringList)
        return value.toStringList();
    else if (returnType == _TypeJsonArray)
        return QJsonDocument(QJsonDocument::fromJson(value.toByteArray())).array().toVariantList();
    else if (returnType == _TypeJsonObject)
        return QJsonDocument(QJsonDocument::fromJson(value.toByteArray())).object().toVariantMap();
    else
        return value;
}

void Settings::save(const QString &key, const QVariant &value)
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

void Settings::remove(const QString &key)
{
    if (m_qsettings->contains(key))
        m_qsettings->remove(key);
}
