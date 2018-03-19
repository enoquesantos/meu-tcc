#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>

class QSettings;

/**
 * @brief The Settings class
 * @extends QObject
 * This class has some Settingslication responsibilities and contains a useful utilities:
 * 1: Handle the local settings from a QSettings instance.
 *    QML objects can save/read/remove local settings using the methods save(...), read(...) and remove(...)
 * 2: Handle the Settingslication plugins from PluginManager instance.
 *    The pluginManager is a object that load all plugins, create a database table for each plugin and
 *    remove all qml cached files when Settings are updated.
 * 3: Load the config.json file that contains the Settingslication property settings.
 * 4: Set the QuickControls style (Material, Universal) defined from config.json, passed to QQuickStyle.
 * 5: Handle the push notification messages from a connection with android activity and ios QtSettingsDelegate, sending the events to QML main window.
 */
class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 SettingTypeBool MEMBER settingTypeBool CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeInt MEMBER settingTypeInt CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeString MEMBER settingTypeString CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeStringList MEMBER settingTypeStringList CONSTANT FINAL)
    Q_PROPERTY(quint8 SettingTypeJsonObject MEMBER settingTypeJsonObject CONSTANT FINAL)
    Q_PROPERTY(bool IS_ANDROID MEMBER _IS_ANDROID CONSTANT FINAL)
    Q_PROPERTY(bool IS_IOS MEMBER _IS_IOS CONSTANT FINAL)
    Q_PROPERTY(bool IS_OSX MEMBER _IS_OSX CONSTANT FINAL)
    Q_PROPERTY(bool IS_LINUX MEMBER _IS_LINUX CONSTANT FINAL)
    Q_PROPERTY(bool IS_WINDOWS MEMBER _IS_WINDOWS CONSTANT FINAL)
    Q_PROPERTY(bool IS_MOBILE MEMBER _IS_MOBILE CONSTANT FINAL)
public:
    /**
     * @brief Settings
     * The object constructor
     * @param parent *QObject a pointer to the object parent
     */
    explicit Settings(QObject *parent = nullptr);

    /**
     * @brief ~Settings
     * The object destructor
     */
    ~Settings();

    /**
     * @brief init
     * This method is called by constructor and load the config.json,
     * register the Settingslication settings names to QSettingslication,
     * set the Settingslication style to QQuickStyle::setStyle and
     * load all plugins using PluginManager object.
     */
    void init();

    /**
     * @brief config
     * Get the Settingslication config.json as QMap<QString, QVariant> object and
     * is used by QML objects to get window properties, the colors used in widgets and
     * others properties like webservice informations (base url, images url and authentication parameters)
     * @return QVariantMap
     */
    QVariantMap config() const;

    /**
     * @brief setPluginsPaths
     * Settingsend into Config.json a list with all plugins paths by plugin name.
     * Can be used to qml plugins load a file or page using a syntax like this: Config.plugins.about + "About.qml"
     */
    void setPluginsPaths();

    /**
     * @brief read
     * Read a generic value (string, integer, float or a json) from Settingslication local settings.
     * The returned type will be defined by 'returnType', based on the object attributes 'settingType'.
     * The 'returnType' prevent QML objects to make parse to specific values like integer, float or JSON.
     * @param key QString the key to retrieve the value
     * @param returnType quint8 the type value to be returned. Default is QString
     * @return QVariant
     */
    Q_INVOKABLE QVariant read(const QString &key, quint8 returnType = 4);

    /**
     * @brief save
     * Save a generic data in Settingslication settings.
     * This method is useful to save strings, booleans, integers or json, using a keyword and value.
     * @param key QString the key name to be used when retrieve the value
     * @param value QVariant the value to save. If some value already exists to the key, the data will be updated
     */
    Q_INVOKABLE void save(const QString &key, const QVariant &value);

    /**
     * @brief remove
     * Remove a value from local settings if exists.
     * @param key QString the key to remove from qsettings
     */
    Q_INVOKABLE void remove(const QString &key);

private:
    /**
     * @brief m_qsettings
     * This object handle the Settingslication settings using key->value to save/read data.
     */
    QSettings *m_qsettings;

    /**
     * @brief m_config
     * This object keep the Settingslication configuration (from config.json) file while Settingslication running.
     * The content data of this object is not saved and can be used to:
     * 1: define Settingslication colors, font size and others properties
     * 2: keeps the Settingslication name, description and other flags
     * 3: set the webservice url, username and userpassword for Basic Authentication (send in http requests)
     */
    QVariantMap m_config;

    /**
     * The properties bellow will be used to define the return type
     * in 'read' method called by QML objects to get apropriated setting value types.
     */
    const quint8 settingTypeBool = 1;
    const quint8 settingTypeInt = 2;
    const quint8 settingTypeString = 4;
    const quint8 settingTypeStringList = 5;
    const quint8 settingTypeJsonObject = 6;
    const quint8 settingTypeJsonArray = 7;

    bool _IS_ANDROID = false;
    bool _IS_IOS = false;
    bool _IS_OSX = false;
    bool _IS_LINUX = false;
    bool _IS_WINDOWS = false;
    bool _IS_MOBILE = false;
};

#endif // SETTINGS_H
