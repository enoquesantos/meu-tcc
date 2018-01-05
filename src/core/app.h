#ifndef APP_H
#define APP_H

#include <QObject>
#include <QVariant>

class PluginManager;
class QSettings;
class Notification;

/**
 * @brief The App class
 * @extends QObject
 * This class has some application responsibilities and contains a useful utilities:
 * 1: Handle the local settings from a QSettings instance.
 *    QML objects can save/read/remove local settings using the methods saveSetting(...), readSetting(...) and removeSetting(...)
 * 2: Handle the application plugins from PluginManager instance.
 *    The plugin manager is a object that load all plugins, create a database table for each plugin and
 *    remove all qml cached files when app are updated.
 * 3: Load the config.json file that contains the application property settings.
 * 4: Set the QuickControls style (Material, Universal) defined from config.json, passed to QQuickStyle.
 * 5: Handle the push notification messages from a connection with android activity and ios QtAppDelegate, sending the events to QML main window.
 */
class App : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 SettingTypeBool MEMBER settingTypeBool CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeInt MEMBER settingTypeInt CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeString MEMBER settingTypeString CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeStringList MEMBER settingTypeStringList CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeJsonObject MEMBER settingTypeJsonObject CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeJsonArray MEMBER settingTypeJsonArray CONSTANT FINAL)
public:
    /**
     * @brief App
     * The object constructor
     * @param parent *QObject a pointer to the object parent
     */
    explicit App(QObject *parent = nullptr);

    /**
     * @brief ~App
     * The object destructor
     */
    ~App();

    /**
     * @brief init
     * This method is called by constructor and load the config.json,
     * register the application settings names to QApplication,
     * set the application style to QQuickStyle::setStyle and
     * load all plugins using PluginManager object.
     */
    void init();

    /**
     * @brief config
     * Get the application config.json as QMap<QString, QVariant> object and
     * is used by QML objects to get colors and others properties like
     * webservice informations: base url, images url and authentication parameters
     * @return QVariantMap with config.json content
     */
    QVariantMap config();

    /**
     * @brief setPluginsPaths
     * Append into Config.json a list with all plugins paths by plugin name.
     * Can be used to qml plugins load a file or page using a syntax like this: Config.plugins.about + "About.qml"
     */
    void setPluginsPaths();

    /**
     * @brief isDeviceOnline
     * Returns true if the system is considered to be connected to another device via
     * an active network interface (Uses QNetworkConfigurationManager class). Otherwise returns false.
     * @return bool
     */
    Q_INVOKABLE bool isDeviceOnline();

    /**
     * @brief sendNotification
     * @param title
     * @param message
     * @param actionName
     * @param actionValue
     */
    Q_INVOKABLE void sendNotification(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));

    /**
     * @brief readSetting
     * Read a generic value (string, integer, float or a json) from application local settings.
     * The returned type will be defined by 'returnType', based on the object attributes 'settingType'.
     * The 'returnType' prevent QML objects to make parse to specific values like integer, float or JSON.
     * @param key QString the key to retrieve the value
     * @param returnType quint8 the type value to be returned. Default is QString
     * @return QVariant
     */
    Q_INVOKABLE QVariant readSetting(const QString &key, quint8 returnType = 4);

    /**
     * @brief saveSetting
     * Save a generic data in application settings.
     * This method is useful to save strings, booleans, integers or json, using a keyword and value.
     * @param key QString the key name to be used when retrieve the value
     * @param value QVariant the value to save. If some value already exists to the key, the data will be updated
     */
    Q_INVOKABLE void saveSetting(const QString &key, const QVariant &value);

    /**
     * @brief removeSetting
     * Remove a value from local settings if exists.
     * @param key QString the key to remove from qsettings
     */
    Q_INVOKABLE void removeSetting(const QString &key);

    /**
     * @brief minimize
     * This method is used only in android to minimize the app, placing above others apps or
     * focusing in the home screen. Is used when back button is pressed by user. This method is called by main window
     * from event handle 'Keys.onBackPressed' and 'onClosing'.
     * This method call the android native method 'moveTaskToBack'.
     */
    Q_INVOKABLE void minimize();

    /**
     * @brief fireEventNotify
     * This method make a connection with android Activity to receive push events (messages and firebase token).
     * In android, the connection is created in JavaCppConnect.h before application running (via JNI_OnLoad).
     * In ios, this method is called directly from QtAppDelegate.mm.
     * The events will be fired to application using the signal 'notifyEvent'.
     * @param eventName QString the event name
     * @param eventData QString the event data, like a string as json data or string with push notification token.
     */
    static void fireEventNotify(const QString &eventName, const QString &eventData);

signals:
    /**
     * @brief notifyEvent
     * Fire internal android and iOS events that become in eventNotify slot.
     * Is used by push notification token and push notification messages when
     * the app is on foreground or background (but running), sending the event data to qml main window.
     * @param eventName QVariant the name (or indentity) of the event
     * @param eventData QVariant the event argument data
     */
    void eventNotify(const QVariant &eventName, const QVariant &eventData);

private:
    /**
     * @brief m_instance
     * This pointer is used to call object signal from static method fireEventNotify(...),
     * from application binds with android activity and ios QtAppDelegate
     */
    static App *m_instance;

    /**
     * @brief m_qsettings
     * This object handle the application settings using key->value to save/read data.
     */
    QSettings *m_qsettings;

    /**
     * @brief m_pluginManager
     * This object manage the application plugins and require a reference to
     * qsettings object to save the plugins pages, application version and others data.
     * PluginManager compare the app version value from previous saved value using QApplication::ApplicationVersion()
     * to decide if remove all qml cached files (*.qmlc and *.jsc) to refresh new changes
     */
    PluginManager *m_pluginManager;

    /**
     * @brief m_config
     * This object keep the application configuration (from config.json) file while application running.
     * The content data of this object is not saved and can be used to:
     * 1: define application colors, font size and others properties
     * 2: keeps the application name, description and other flags
     * 3: set the webservice url, username and userpassword for Basic Authentication (send in http requests)
     */
    QVariantMap m_config;

    /**
     * The properties bellow will be used to define the return type
     * in 'readSetting' method called by QML objects to get apropriated setting value types.
     */
    const quint8 settingTypeBool = 1;
    const quint8 settingTypeInt = 2;
    const quint8 settingTypeString = 4;
    const quint8 settingTypeStringList = 5;
    const quint8 settingTypeJsonObject = 6;
    const quint8 settingTypeJsonArray = 7;
};

#endif // APP_H
