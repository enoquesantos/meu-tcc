#ifndef UPLOADMANAGER_H
#define UPLOADMANAGER_H

#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QObject>

class QFile;
class QFileInfo;
class QHttpMultiPart;
class QHttpPart;
class QNetworkRequest;
class QMimeType;
class QMimeDatabase;
class QUrl;

/**
 * @brief The UploadManager class
 * This class handle http requests to upload files for some webservice and uses
 * QNetworkAccessManager as network API. The files can be uploaded using 'uploadFile(...)' method.
 *
 * The 'finished' signal will be sent when upload request finishes.
 * You can send multiple files to upload in a single 'uploadFile(...)' call, and custom HTTP headers can be pass.
 */
class UploadManager: public QObject
{
    Q_OBJECT
public:
    /**
     * @brief UploadManager
     * Default class constructor
     * @param parent QObject *
     */
    explicit UploadManager(QObject *parent);

    /**
     * @brief uploadFile
     * Starts a upload request to 'url' and the files needs to be set in 'filePathsList' as absolute path in device.
     * This method is asynchronous, to get the request response, create a connection with uploadFinished(QNetworkReply*) for each file
     * sent to server. When all upload files finishes, the 'finished'
     * @param url QByteArray the url to upload the files
     * @param filePathsList QStringList a files list with absolute paths to load and submit to server
     * @param headers QVariantMap a map with custom HTTP headers to send in request
     * @param usesPutMethod bool a flag to decide where the HTTP method will be used. The default is POST, set true to uses PUT method
     */
    void uploadFile(const QByteArray &url, const QStringList &filePathsList, const QVariantMap &headers = QVariantMap(), bool usesPutMethod = false);

private:
    /**
     * @brief setMultiPartRequest
     * @param httpMultiPart QHttpMultiPart* a pointer to QHttpMultiPart.
     * The QHttpMultiPart class resembles a MIME multipart message to be sent over HTTP.
     * @param filePath QString the file path to create the QHttpPart and append in httpMultiPart pointer.
     * @return bool true if file in 'filePath' can be read and added to httpMultiPart, otherwise return false.
     */
    bool setMultiPartRequest(QHttpMultiPart *httpMultiPart, const QString &filePath);

signals:
    /**
     * @brief error
     * Emits a request error event with network status code
     * @param statusCode QNetworkReply::NetworkError
     */
    void error(QNetworkReply::NetworkError statusCode);

    /**
     * @brief finished
     * Emits a finished status to the object creator to decide when
     * delete the object pointer. This signal will be emitted after all files
     * passed in 'uploadFile' method are success sent to server.
     * @param thiz DownloadManager *
     */
    void finished(UploadManager *thiz);

    /**
     * @brief uploadFinished
     * Emits a finished status for each file sent to server
     * @param reply QNetworkReply *
     */
    void uploadFinished(QNetworkReply *reply);

    /**
     * @brief uploadProgressChanged
     * Emits the upload progress from each file sent to server
     * @param bytesSent qint64
     * @param bytesTotal qint64
     */
    void uploadProgressChanged(qint64 bytesSent, qint64 bytesTotal);

private:
    /**
     * @brief m_manager
     * The QNetworkAccessManager class allows the application to send network requests and receive replies.
     */
    QNetworkAccessManager *m_manager;
};

#endif // UPLOADMANAGER_H
