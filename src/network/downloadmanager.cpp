#include "downloadmanager.h"

#include <QDateTime>
#include <QFileInfo>
#include <QIODevice>
#include <QStandardPaths>
#include <QUrl>

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
  ,m_directoryToSave(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toUtf8() + "/")
  ,m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDownloadFinished(QNetworkReply*)));
}

DownloadManager::~DownloadManager()
{
    delete m_manager;
    if (m_currentDownloads.size()) {
        int i = 0;
        while(!m_currentDownloads.isEmpty())
            delete m_currentDownloads.at(i++);
        m_currentDownloads.clear();
    }
}

void DownloadManager::doDownload(const QStringList &urls, const QByteArray &directoryToSave)
{
    doDownload(urls, QVariantMap(), directoryToSave);
}

void DownloadManager::doDownload(const QStringList &urls, const QVariantMap &headers, const QByteArray &directoryToSave)
{
    if (!directoryToSave.isEmpty())
        m_directoryToSave = directoryToSave;
    QNetworkRequest request;
    if (headers.size()) {
        QMapIterator<QString, QVariant> i(headers);
        while (i.hasNext()) {
            i.next();
            request.setRawHeader(i.key().toUtf8(), i.value().toByteArray());
        }
    }
    foreach (const QString &url, urls) {
        request.setUrl(QUrl::fromEncoded(url.toLocal8Bit()));
        QNetworkReply *reply = m_manager->get(request);
        if (reply) {
            QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [this](QNetworkReply::NetworkError e) {
                emit error(e);
            });
            connect(reply, &QNetworkReply::downloadProgress, [this](qint64 bytesReceived, qint64 bytesTotal) {
                emit downloadProgressChanged(bytesReceived, bytesTotal);
            });
            m_currentDownloads << reply;
        }
    }
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    // basename contains the file name from ul origin
    QString baseName(QFileInfo(url.path()).fileName());

    // if can't read the original file name, generate a new name
    // using the current timestamp without extension type.
    if (baseName.isEmpty())
        baseName = QString::number(QDateTime::currentMSecsSinceEpoch() / 1000);

    // if file with same name already exists, will be removed!
    if (QFile::exists(m_directoryToSave + baseName))
        QFile::remove(m_directoryToSave + baseName);

    // return the absolute path with saved file name
    return m_directoryToSave + baseName;
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Could not open %s for writing: %s!\n", qPrintable(filename), qPrintable(file.errorString()));
        return false;
    }
    file.write(data->readAll());
    file.close();
    return true;
}

void DownloadManager::onDownloadFinished(QNetworkReply *reply)
{
    QUrl url(reply->url());
    if (reply->error()) {
        qWarning("Download of %s failed: %s.\n", url.toEncoded().constData(), qPrintable(reply->errorString()));
        m_currentDownloads.removeOne(reply);
        delete reply;
        return;
    }
    QString filename(saveFileName(url));
    if (saveToDisk(filename, reply))
        qDebug("Download of %s succeeded and saved to %s.\n", url.toEncoded().constData(), qPrintable(filename));
    else
        qWarning("Cannot save file to disk!");
    m_currentDownloads.removeOne(reply);
    delete reply;
    if (!filename.isEmpty())
        emit fileSaved(QUrl::fromUserInput(filename).toEncoded());
    if (m_currentDownloads.isEmpty())
        emit finished(this);
}
