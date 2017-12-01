#ifndef REQUESTHTTP_H
#define REQUESTHTTP_H

#include <QJsonParseError>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>

class QNetworkRequest;
class QUrl;
class QUrlQuery;

class DownloadManager;
class UploadManager;

/**
 * @brief The RequestHttp class
 * @extends QObject
 * This class manage http requests using get and post methods and can be used by QML components to handle webservice requests.
 * RequestHttp supports upload and download files as high level methods.
 * This class it also supports basic authentication and can be set using the registered properties:
 * basicAuthorizationUser and basicAuthorizationPassword.
 *
 * The properties below is to turn the object features accessible through the QML components
 * To QML components start the requests, can uses the get, post, uploadFile and downloadFile methods.
 */
class RequestHttp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(int Error READ statusError)
    Q_PROPERTY(int Finished READ statusFinished)
    Q_PROPERTY(int Loading READ statusLoading)
    Q_PROPERTY(int Ready READ statusReady)
    Q_PROPERTY(QByteArray baseUrl WRITE setUrl NOTIFY baseUrlChanged MEMBER m_baseUrl)
    Q_PROPERTY(QByteArray basicAuthorizationUser WRITE setBasicAuthorizationUser MEMBER m_basicAuthorizationUser)
    Q_PROPERTY(QByteArray basicAuthorizationPassword WRITE setBasicAuthorizationPassword MEMBER m_basicAuthorizationPassword)
public:
    /**
     * @brief RequestHttp
     * @param parent QObject the object parent
     */
    explicit RequestHttp(QObject *parent = nullptr);

    /**
     * @brief The Status enum
     * Keeps the current request status and can be used by QML components to binding with current status.
     */
    enum Status {
        Error,
        Finished,
        Loading,
        Ready
    };
    Q_ENUM(Status)

    /**
     * @brief setUrl
     * Set the base url for current object
     * @param url QByteArray
     */
    void setUrl(const QByteArray &url);

    /**
     * @brief setBasicAuthorizationUser
     * Set the basic authorization user
     * @param user QByteArray
     */
    void setBasicAuthorizationUser(const QByteArray &user);

    /**
     * @brief setBasicAuthorizationPassword
     * Set the basic authorization password
     * @param password QByteArray
     */
    void setBasicAuthorizationPassword(const QByteArray &password);

    /**
     * @brief status
     * Get the current request status as integer
     * @return int
     */
    int status();

    /**
     * @brief statusError
     * Get the value for Status::Error
     * @return int
     */
    int statusError();

    /**
     * @brief statusFinished
     * Get the value for Status::Finished
     * @return int
     */
    int statusFinished();

    /**
     * @brief statusLoading
     * Get the value for Status::Loading
     * @return int
     */
    int statusLoading();

    /**
     * @brief statusReady
     * Get the value for Status::Ready
     * @return int
     */
    int statusReady();

    /**
     * @brief setBasicAuthorization
     * Set a Basic Authentication to create a request header as base64 hash
     * @link https://en.wikipedia.org/wiki/Basic_access_authentication
     * @param user QByteArray the username to use in the authentication hash
     * @param password QByteArray the user password to use in the authentication hash
     */
    Q_INVOKABLE void setBasicAuthorization(const QByteArray &user, const QByteArray &password);

    /**
     * @brief downloadFile
     * Download files list passed in urls and saved in default device download directory.
     * If "saveInAppDirectory" is set to true, the files will be saved in application location.
     *
     * @WARNING!
     * This method is asynchronous and uses the DownloadManager object to execute the request and emit the signal "downloadFinished" at the end.
     * Your can get the download status creating a connection with downloadProgressChanged signal.
     * To access the downloaded files, create a connection with "downloadedFileSaved" signal, emitted for each downloaded file.
     *
     * @param urls QStringList the list of files to download
     * @param saveInAppDirectory a flag to decide if downloaded file will be saved in application directory or in the device download path.
     * The default is false (uses QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)). If saveInAppDirectory is set to true, uses the
     * QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) to write the file.
     * @param headers QVariantMap a map with request headers
     */
    Q_INVOKABLE void downloadFile(const QStringList &urls, bool saveInAppDirectory = false, const QVariantMap &headers = QVariantMap());

    /**
     * @brief uploadFile
     * Upload files to webservice as binary data using http post or put method.
     * You can get the status of upload creating a connection with "uploadProgressChanged" signal, that emit the number of bytes sent.
     * After upload finished, the signal "uploadFinished" will be emitted.
     *
     * @WARNING!
     * This method is asynchronous and uses the UploadManager object to execute the request.
     *
     * @param url QByteArray the url where files are sent
     * @param filePathsList QStringList the files list to be sent
     * @param headers QVariantMap a map with request headers
     * @param usesPutMethod bool a flag to decide if uses the HTTP PUT method. Default is false and the POST method will be used.
     */
    Q_INVOKABLE void uploadFile(const QByteArray &url, const QStringList &filePathsList, const QVariantMap &headers = QVariantMap(), bool usesPutMethod = false);

    /**
     * @brief get
     * Request a HTTP GET method to webservice, using the url, url query string (build from a map with key=value pairs) and http headers.
     * If the Basic Authentication is set, will be placed in the http header.
     *
     * @WARNING!
     * This method is asynchronous, to get the response from request, create a connection with "finished" signal.
     *
     * @param url QByteArray the url to make the get request.
     * @param urlArgs QVariantMap a map with url arguments to buil as string and concatenate in the url. This argument uses QUrlQuery to construct the string.
     * @link http://doc.qt.io/qt-5/qurlquery.html
     * @link https://en.wikipedia.org/wiki/Query_string
     * @param headers QVariantMap a map with http headers
     */
    Q_INVOKABLE void get(const QByteArray &url, const QVariantMap &urlArgs = QVariantMap(), const QVariantMap &headers = QVariantMap());

    /**
     * @brief post
     * Request a HTTP POST method to webservice, using the url and postData
     * @param url QByteArray the url to make the post request.
     * @param postData QVariant the content data to sent in request (string, integer, byte array)
     * @param headers QVariantMap a map with http headers
     */
    Q_INVOKABLE void post(const QByteArray &url, const QVariant &postData, const QVariantMap &headers = QVariantMap());

private:
    /**
     * @brief initRequest
     * @param request QNetworkRequest *
     * @param url QByteArray the build a QUrl set to QNetworkRequest.
     * @link http://doc.qt.io/qt-5/qnetworkrequest.html
     * @param headers QVariantMap a map to append in QNetworkRequest as rawHeader
     * @param urlArgs QVariantMap
     */
    void initRequest(QNetworkRequest *request, const QByteArray &url, const QVariantMap &headers, const QVariantMap &urlArgs = QVariantMap());

    /**
     * @brief urlQueryFromMap
     * @param map QVariantMap the url arguments to build a QUrlQuery
     * @return QUrlQuery
     */
    QUrlQuery urlQueryFromMap(const QVariantMap &map);

    /**
     * @brief setStatus
     * Set the current request status
     * @param status Status
     */
    void setStatus(Status status);

    /**
     * @brief setHeaders
     * Set the HTTP headers for some request
     * @param requestHeaders QVariantMap a map with HTTP header as name->value pairs
     * @param request QNetworkRequest *
     */
    void setHeaders(const QVariantMap &requestHeaders, QNetworkRequest *request);

signals:
    /**
     * @brief baseUrlChanged
     * Emitted when the object url (m_baseUrl) change
     * @param url QByteArray
     */
    void baseUrlChanged(const QByteArray &url);

    /**
     * @brief downloadFinished
     * Emitted after downloaded files are finished
     */
    void downloadFinished();

    /**
     * @brief downloadProgressChanged
     * Emitted for each byte received from each file during download files request
     * @param bytesReceived qint64 the total bytes received
     * @param bytesTotal qint64 the size of bytes from origin file
     */
    void downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);

    /**
     * @brief downloadedFileSaved
     * Emitted after each downloaded file are saved in device, emitted from downloadFile method.
     * @param filePath QByteArray the saved file absolute path in the device
     */
    void downloadedFileSaved(const QByteArray &filePath);

    /**
     * @brief error
     * Emitted for all request errors, like invalid https certificates, network errors or server errors.
     * The statusCode can be HTTP response codes (like 500 or 400).
     * The message is "Network reply with error code " + QNetworkReply::NetworkError
     * @param statusCode int
     * @param message QVariant
     */
    void error(int statusCode, const QVariant &message);

    /**
     * @brief finished
     * Emitted for each request success response with the HTTP status code and the server response sended.
     * For each response will be checked if the result has a valid json and will be converted to array or
     * object when is any of types. For object, the response will be a QVariantMap, for array, QVariantList.
     * Otherwise (is not a json), sended as QByteArray.
     * @param statusCode int
     * @param response QVariant
     */
    void finished(int statusCode, const QVariant &response);

    /**
     * @brief statusChanged
     * Emitted when the response status are updated. The possible values is the Class Enum:
     * Status {
     *   Error,
     *   Finished,
     *   Loading,
     *   Ready
     * }
     * @param status int
     */
    void statusChanged(int status);

    /**
     * @brief uploadFinished
     * Emitted after uploaded files are finished
     */
    void uploadFinished();

    /**
     * @brief uploadProgressChanged
     * Emitted for each byte sended from each file during upload files request
     * @param bytesSent qint64 the size of bytes sended to server
     * @param bytesTotal qint64 the size of bytes of the file
     */
    void uploadProgressChanged(qint64 bytesSent, qint64 bytesTotal);

private slots:
    /**
     * @brief onError
     * This slot will be connected on each request, for handle request errors,
     * like invalid HTTPS certificates or some network error.
     * @param code QNetworkReply::NetworkError
     */
    void onError(QNetworkReply::NetworkError code);

    /**
     * @brief onFinished
     * This slot is connected with QNetWorkManager object to receive the requests responses.
     * The response is send as byte array, and needs to be converted to json (if is a json), otherwise, uses a plain text.
     * The response data (plain text or json array or json object) and the response status is sent on the "finished" signal
     * @param reply QNetworkReply *
     */
    void onFinished(QNetworkReply *reply);

private:
    /**
     * @brief m_status
     * Keeps current status for request. The status is sent in 'statusChanged(m_status)' signal.
     */
    Status m_status;

    /**
     * @brief m_baseUrl
     * Keeps the host domain or webservice address as base url for all requests, to simplify the use of component.
     * The components can open requests only with the path, if the property baseUrl is set.
     * If the property baseUrl is set, can call get or post like this:
     *  1: requestHttp.get("/search/", {"product":textInput.text})
     *  2: requestHttp.post("/login", jsonLoginValues)
     *  3: requestHttp.uploadFile("/save_user_image/user_id", ["/home/user/Downloads/my-image.png"])
     *  4: requestHttp.downloadFile("/statics/images/user.png")
     */
    QByteArray m_baseUrl;

    /**
     * @brief m_basicAuthorization
     * Keeps the basic authorization hash to sent in requests as header parameter.
     * The basic authorization is a simple way to authenticate the requests in webservice.
     * The value will be defined in setBasicAuthorization(...) method and the value will be defined by
     * user name and user password as QByteArray from a base64, like this:
     *  1: m_basicAuthorization = "Basic " + QByteArray(usernameAndPassword.toLocal8Bit().toBase64());
     */
    QByteArray m_basicAuthorization;

    /**
     * @brief m_basicAuthorizationUser
     * The user name used to create the basic authorization header, sent in all requests.
     */
    QByteArray m_basicAuthorizationUser;

    /**
     * @brief m_basicAuthorizationPassword
     * The user password used to create the basic authorization header, sent in all requests.
     */
    QByteArray m_basicAuthorizationPassword;

    /**
     * @brief m_networkAccessManage
     * The QNetworkAccessManager class allows the application to send network requests and receive replies.
     * The m_networkAccessManager manage get and post requests.
     */
    QNetworkAccessManager m_networkAccessManager;
};

#endif // REQUESTHTTP_H
