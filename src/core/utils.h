#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QUrl>
#include <QVariant>

class QFile;
class QJsonArray;
class QJsonDocument;
class QJsonObject;
class QJsonParseError;

/**
 * @brief The Utils class
 * This class contains a useful methods to Qt application, to QML objects make some operations
 * that is unavailable by QML components. The class utilities are:
 *  1: copy a file from local device folder to another device folder with copyFile(...);
 *  2: read a file content, receiving as plain text or json object or array with readFile(...);
 *  3: serializing a object (javascript object) or array (javascript array) to string with stringfyJson(...);
 *  4: parse a json string to object or array with jsonParse(...);
 *  5: create a HTML <a> link from some string if exists with linkifyText(...);
 *  6: parse generic file url to local url or html valid url from string with parseUrl(...).
 */
class Utils : public QObject
{
    Q_OBJECT
private:
    /**
     * @brief Utils
     * The object construct
     * @param parent Utils*
     */
    explicit Utils(QObject *parent = nullptr);

    /**
     * @brief Utils
     * In singleton object, the copy constructor needs to be private
     * @param other Utils
     */
    Utils(const Utils &other);

    /**
     * @brief ~Utils
     * The object destructor
     */
    virtual ~Utils();

    /**
     * @brief operator =
     * In singleton object, the operator '=' needs to be private
     */
    void operator=(Utils const &);

public:
    /**
     * @brief instance
     * Return a pointer to Utils class instance
     * @return Utils*
     */
    static Utils *instance();

    /**
     * @brief stringfyJson
     * Create and return a json as QString from a QVariantMap
     * @param map QVariantMap a map to parse to string
     * @param compact bool a flag to return the string minified (default), otherwise return indented
     * @return QString
     */
    Q_INVOKABLE QString stringfyJson(const QVariant &data, bool compact = true);

    /**
     * @brief jsonParse
     * Create and return a json as QVariantMap from a string json. Useful to deep json document, because the JSON.parse()
     * cannot parse from deep json objects with other objects and arrays.
     * @param jsonStr QString the json as string to parse
     * @return QVariantMap
     */
    Q_INVOKABLE QVariant jsonParse(const QByteArray &jsonStr);

    /**
     * @brief parseUrl
     * Return a string with valid QUrl from url parameter, using QUrl::fromUserInput(...).
     * You can use parseUrl(...) to following purposes:
     *   1: parseUrl("qt-project.org") -> return "http://qt-project.org";
     *   2: parseUrl("ftp.qt-project.org") -> return "ftp://ftp.qt-project.org"
     *   3: parseUrl("hostname") -> return "http://hostname"
     *   4: parseUrl("/home/user/test.html") -> return "file:///home/user/test.html"
     * @param url QString
     * @return QString
     */
    Q_INVOKABLE QString parseUrl(const QString &url);

    /**
     * @brief linkifyText
     * Search and replace http links for html <a> entity to QML Text objects set as clickable
     * @param text QByteArray the text to search and replace the link
     * @return QString
     */
    Q_INVOKABLE QString linkifyText(const QByteArray &text);

    /**
     * @brief readFile
     * Read a file content with some rules:
     *  1: if the file is a valid json object, return a QVariantMap
     *  2: if the file is a valid json array, return a QVariantList
     *  3: if the file is not a valid json, return a QByteArray with file content as QVariant
     * @param path QString the absolute file path
     * @return QVariant
     */
    Q_INVOKABLE QVariant readFile(const QString &filePath);

    /**
     * @brief copyFile
     * Create a copy of 'fileName' if exists and overwrite the destination file if
     * 'overwriteIfExists' parameter is true (default is true).
     * Return true if 'fileName' was success copied to 'newName'.
     * @param fileName QString the absolute file path to be copied
     * @param newName QString a string with new absolute path and destination file name
     * @param overwriteIfExists bool a flag to decide if remove or not the destination file if already exists
     * @return bool
     */
    Q_INVOKABLE bool copyFile(const QString &fileName, const QString &newName, bool overwriteIfExists = true);

    /**
     * @brief fileBaseName
     * Extract the base name from any local or remote file:
     * Ex: fileBaseName("/tmp/archive.tar.gz"); // return "archive.tar.gz"
     * @param filePath QString from a local or remote file (http://.../image.png)
     * @return QString
     */
    Q_INVOKABLE QString fileBaseName(const QString &filePath);

    /**
     * @brief isDeviceOnline
     * Returns true if the system is considered to be connected to another device via
     * an active network interface (Uses QNetworkConfigurationManager class). Otherwise returns false.
     * @return bool
     */
    Q_INVOKABLE bool isDeviceOnline();

    /**
     * @brief readFirebaseToken()
     * This method is useful and needed only for android API level >= 7 and is used to read
     * the firebase token, that can be registered before user enable write permission.
     * If the token was passed to application before user enable write permission to the app,
     * the Qt cannot writer in writteable directory to save any data with qsettings instance.
     * The request permission is make after app startup on the onResume activity method
     * using a native system dialog, and user need to confirm for enable write in local storage.
     * @return QString a string with application token
     */
    Q_INVOKABLE QString readFirebaseToken();

    /**
     * @brief minimizeWindow
     * This method is used only in android to minimize the app, placing above others apps or
     * focusing in the home screen. Is used when back button is pressed by user.
     * This method is called by main window from event handle 'Keys.onBackPressed' and 'onClosing'.
     * This method call the android native method 'moveTaskToBack'.
     */
    Q_INVOKABLE void minimizeWindow();

private:
    /**
     * @brief m_instance
     * A pointer to class instance
     */
    static Utils *m_instance;
};

#endif // UTILS_H
