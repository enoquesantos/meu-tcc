#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QObject>
#include<QVector>

class QDateTime;
class QFileInfo;
class QIODevice;
class QStandardPaths;
class QUrl;

/**
 * @brief The DownloadManager class
 * This class handle http requests to download files and uses QNetworkAccessManager as network API.
 * The files can be request using doDownload method and files downloaded will be saved in path defined
 * by QStandardPaths::writableLocation(QStandardPaths::DownloadLocation()).
 * You can set a custom directory and multiple files can be downloaded in a single doDownload() call.
 */
class DownloadManager: public QObject
{
    Q_OBJECT
public:
    /**
     * @brief DownloadManager
     * Default class constructor
     * @param parent QObject *
     */
    explicit DownloadManager(QObject *parent);

    /**
     * The object destructor.
     * Delete m_manager and all pointers in m_currentDownloads vector.
     */
    ~DownloadManager();

    /**
     * @brief doDownload
     * @overload
     * @param urls QStringList
     * @param directoryToSave QByteArray
     */
    void doDownload(const QStringList &urls, const QByteArray &directoryToSave = "");

    /**
     * @brief doDownload
     * Starts a http request to each url in 'urls' using 'm_manager' pointer as HTTP GET request.
     * The files can be any type like png, jpg, pdf, odt, xlsx or docx.
     * The file type will be defined by 'mime-type' of downloaded file.
     * For each downloaded file, the signal 'fileSaved' with the absolute path will be emitted.
     * After all requested files are saved in device, the 'finished' signal will be sent with the object pointer.
     * The object creator can delete this instance if needed, creating a connection with finished signal.
     * @param urls QStringList the list of urls to download
     * @param headers QVariantMap A custom HTTP headers as key->value
     * @param directoryToSave QByteArray (optional) A custom directory path to save the files
     */
    void doDownload(const QStringList &urls, const QVariantMap &headers = QVariantMap(), const QByteArray &directoryToSave = "");

private:
    /**
     * @brief saveFileName
     * @param url QUrl the remote the the file url to get the file name, will be checked if
     * already exists in device (generating a new name if true) and sets the new downloaded file path.
     * @return QString return the downloaded file absolute path
     */
    QString saveFileName(const QUrl &url);

    /**
     * @brief saveToDisk
     * Write a file to disk using 'fileName' (with absolute path) and
     * 'data' with the file content to be saved on the device.
     * @param filename QString the absolute file path and name
     * @param data QIODevice the file binary data as byte array
     * @return bool
     */
    bool saveToDisk(const QString &filename, QIODevice *data);

private slots:
    /**
     * @brief onDownloadFinished
     * This slot are connected with m_manager 'finished' signal and receive the request responses.
     * @param reply QNetworkReply* the pointer for request reply object
     */
    void onDownloadFinished(QNetworkReply *reply);

signals:
    /**
     * @brief downloadProgressChanged
     * Emits the amount of bytes received on each data downloaded
     * @param bytesReceived qint64
     * @param bytesTotal qint64
     */
    void downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);

    /**
     * @brief error
     * Emits a request error event with network status code
     * @param statusCode QNetworkReply::NetworkError
     */
    void error(QNetworkReply::NetworkError statusCode);

    /**
     * @brief finished
     * Emits a finished status to the object creator to decide when
     * delete the object pointer. This signal will be emitted after all urls
     * passed in 'doDownload' method are success saved in device.
     * @param thiz DownloadManager *
     */
    void finished(DownloadManager *thiz);

    /**
     * @brief fileSaved
     * Emits the notification signal when a new file downloaded are saved in device.
     * @param filePath QByteArray the absolute file path
     */
    void fileSaved(const QByteArray &filePath);

private:
    /**
     * @brief m_directoryToSave
     * a instance of QByteArray, used to keeps the directory where downloaded files will be saved
     */
    QByteArray m_directoryToSave;

    /**
     * @brief m_manager
     * The QNetworkAccessManager class allows the application to send network requests and receive replies
     */
    QNetworkAccessManager *m_manager;

    /**
     * @brief m_currentDownloads
     * Keeps a list of requested files reply.
     * The pointer only can be deleted after each request file are saved in device.
     */
    QVector<QNetworkReply *> m_currentDownloads;
};

#endif // DOWNLOADMANAGER_H
