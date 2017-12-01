#include "uploadmanager.h"

#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QNetworkRequest>
#include <QMimeDatabase>
#include <QMimeType>
#include <QUrl>

UploadManager::UploadManager(QObject *parent) : QObject(parent)
  ,m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
        emit uploadFinished(reply);
    });
}

void UploadManager::uploadFile(const QByteArray &url, const QStringList &filePathsList, const QVariantMap &headers, bool usesPutMethod)
{
    // to upload file (.jpg, .pdf, .doc, .zip) in Qt, the QHttpMultiPart is a object
    // that encapsulates the files as binary data where be sent in put or post request method
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // for each filepath, create a bodyDevice part to append in multiPart
    foreach (const QString &item, filePathsList) {
        if (!setMultiPartRequest(multiPart, item)) {
            // if the file cannot be read or access denied by OS, the request finish now!
            // and multiPart pointer needs to be deleted!
            delete multiPart;
            return;
        }
    }

    // request handle the url, args and headers
    QNetworkRequest request;
    QNetworkReply *reply;
    request.setUrl(QUrl(url));

    QMapIterator<QString, QVariant> i(headers);
    while (i.hasNext()) {
        i.next();
        request.setRawHeader(i.key().toUtf8(), i.value().toByteArray());
    }

    // start the request
    if (usesPutMethod)
        reply = m_manager->put(request, multiPart);
    else
        reply = m_manager->post(request, multiPart);

    if (reply) {
        QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [this](QNetworkReply::NetworkError e) {
            emit error(e);
        });
        connect(reply, &QNetworkReply::uploadProgress, [this](qint64 bytesSent, qint64 bytesTotal) {
            emit uploadProgressChanged(bytesSent, bytesTotal);
        });
        // multiPart will be deleted with the reply
        multiPart->setParent(reply);
        reply->setParent(this);
    }
}

bool UploadManager::setMultiPartRequest(QHttpMultiPart *httpMultiPart, const QString &filePath)
{
    QString fpath(filePath);

    if (fpath.contains("file://"))
        fpath.replace("file://", "");

#ifdef Q_OS_IOS
    QUrl url(fpath);
    fpath = url.toLocalFile();
#endif

    QFile *file = new QFile(fpath);
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning() << "File cannot be opened!";
        return false;
    }

    // get the file name:
    // if the file is> /home/user/Downloads/linux_files/archive.tar.gz
    // the fpath is: archive.tar.gz
    fpath = QFileInfo(*(file)).fileName().trimmed();

    QHttpPart filePart;
    // content disposition is required by server side upload file manager
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("multipart/form-data; filename=%1; file=%2; name=file").arg(fpath, fpath));
    // content disposition is the file mime-type
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QMimeType(QMimeDatabase().mimeTypeForFile(fpath)).name());
    // set the parent to file not be deleted
    file->setParent(httpMultiPart);
    // the file will be loaded as binary data
    filePart.setBodyDevice(file);
    // append the filePart to request multi part data
    httpMultiPart->append(filePart);

    return true;
}
