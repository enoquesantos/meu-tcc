#include "requesthttp.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMapIterator>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>
#include <QUrlQuery>

#include "uploadmanager.h"
#include "downloadmanager.h"

RequestHttp::RequestHttp(QObject *parent) : QObject(parent)
  ,m_status(Status::Ready)
{
    connect(&m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));
}

void RequestHttp::setBasicAuthorizationUser(const QByteArray &user)
{
    m_basicAuthorizationUser = user;
    if (!m_basicAuthorizationPassword.isEmpty())
        setBasicAuthorization(m_basicAuthorizationUser, m_basicAuthorizationPassword);
}

void RequestHttp::setBasicAuthorizationPassword(const QByteArray &password)
{
    m_basicAuthorizationPassword = password;
    if (!m_basicAuthorizationUser.isEmpty())
        setBasicAuthorization(m_basicAuthorizationUser, m_basicAuthorizationPassword);
}

int RequestHttp::status()
{
    return m_status;
}

int RequestHttp::statusError()
{
    return Status::Error;
}

int RequestHttp::statusFinished()
{
    return Status::Finished;
}

int RequestHttp::statusLoading()
{
    return Status::Loading;
}

int RequestHttp::statusReady()
{
    return Status::Ready;
}

void RequestHttp::setBasicAuthorization(const QByteArray &username, const QByteArray &password)
{
    if (username.isEmpty() || password.isEmpty())
        return;
    QString usernameAndPassword(username + ":" + password);
    m_basicAuthorization = "Basic " + QByteArray(usernameAndPassword.toLocal8Bit().toBase64());
}

QUrlQuery RequestHttp::urlQueryFromMap(const QVariantMap &map)
{
    QUrlQuery qUrlQuery;
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        qUrlQuery.addQueryItem(i.key(), i.value().toString());
    }
    return qUrlQuery;
}

void RequestHttp::setStatus(Status status)
{
    m_status = status;
    emit statusChanged(m_status);
}

void RequestHttp::setUrl(const QByteArray &url)
{
    m_baseUrl = url;
    emit baseUrlChanged(m_baseUrl);
}

void RequestHttp::setHeaders(const QVariantMap &requestHeaders, QNetworkRequest *request)
{
    QMapIterator<QString, QVariant> i(requestHeaders);
    while (i.hasNext()) {
        i.next();
        request->setRawHeader(i.key().toUtf8(), i.value().toByteArray());
    }
}

void RequestHttp::downloadFile(const QStringList &urls, bool saveInAppDirectory, const QVariantMap &headers)
{
    DownloadManager *downloadManager = new DownloadManager(this);

    // create a connection to emit download progress changes
    connect(downloadManager, &DownloadManager::downloadProgressChanged, [this](qint64 bytesReceived, qint64 bytesTotal) {
        emit downloadProgressChanged(bytesReceived, bytesTotal);
    });

    // create a connection to emit downloaded file saved signal with the file path
    connect(downloadManager, &DownloadManager::fileSaved, [this](const QString &filepath) {
        emit downloadedFileSaved(filepath.toUtf8());
    });

    // create a connection to handle the pointer deletion
    connect(downloadManager, &DownloadManager::finished, [this](DownloadManager* dm) {
        emit downloadFinished();
        delete dm;
    });

    QVariantMap requestHeaders(headers);

    // append request header authentication if isset
    if (!m_basicAuthorization.isEmpty())
        requestHeaders.insert("Authorization", m_basicAuthorization);

    // set status to loading
    setStatus(Status::Loading);

    // start the download
    if (saveInAppDirectory)
        downloadManager->doDownload(urls, requestHeaders, QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toUtf8() + "/");
    else
        downloadManager->doDownload(urls, requestHeaders);
}

void RequestHttp::uploadFile(const QByteArray &url, const QStringList &filePathsList, const QVariantMap &headers, bool usesPutMethod)
{
    UploadManager *uploadManager = new UploadManager(this);

    // create the connections to handle error signal
    connect(uploadManager, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));

    // create the connections to handle uploadProgress signal
    connect(uploadManager, &UploadManager::uploadProgressChanged, [this](qint64 bytesReceived, qint64 bytesTotal) {
        emit uploadProgressChanged(bytesReceived, bytesTotal);
    });

    // create the connections to handle upload finished signal
    connect(uploadManager, SIGNAL(uploadFinished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    // create a connection to handle the pointer deletion
    connect(uploadManager, &UploadManager::finished, [this](UploadManager* um) {
        emit uploadFinished();
        delete um;
    });

    QVariantMap requestHeaders(headers);

    // append request header authentication if isset
    if (!m_basicAuthorization.isEmpty())
        requestHeaders.insert("Authorization", m_basicAuthorization);

    // set status to loading
    setStatus(Status::Loading);

    // start the upload
    uploadManager->uploadFile(m_baseUrl.isEmpty() || url.contains("http") ? url : m_baseUrl + url, filePathsList, requestHeaders, usesPutMethod);
}

void RequestHttp::get(const QByteArray &url, const QVariantMap &urlArgs, const QVariantMap &headers)
{
    QNetworkRequest request;
    initRequest(&request, url, headers, urlArgs);

    QNetworkReply *reply = m_networkAccessManager.get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    reply->setParent(this);
}

void RequestHttp::post(const QByteArray &url, const QVariant &postData, const QVariantMap &headers)
{
    QNetworkRequest request;
    initRequest(&request, url, headers);

    QNetworkReply *reply = m_networkAccessManager.post(request, postData.toByteArray());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    reply->setParent(this);
}

void RequestHttp::initRequest(QNetworkRequest *request, const QByteArray &url, const QVariantMap &headers, const QVariantMap &urlArgs)
{
    QUrl qurl(m_baseUrl.isEmpty() || url.contains("http") ? url : m_baseUrl + url);

    if (urlArgs.size())
        qurl.setQuery(urlQueryFromMap(urlArgs));

    request->setUrl(qurl);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request->setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    if (!m_basicAuthorization.isEmpty())
        request->setRawHeader("Authorization", m_basicAuthorization);

    if (!headers.isEmpty())
        setHeaders(headers, request);

    setStatus(Status::Loading);
}

void RequestHttp::onError(QNetworkReply::NetworkError code)
{
    setStatus(Status::Error);
    emit error(code, QString("Network reply with error code %1").arg(code));
}

void RequestHttp::onFinished(QNetworkReply *reply)
{
    // get the http status code: 200, 400, 500
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // get all response data as ByteArray
    QByteArray result(reply->readAll());
    // send the 'finished' signal to object listener
    setStatus(Status::Finished);
    if (!result.isEmpty()) {
        QJsonParseError jsonParseError;
        // try to parse the response data to json document
        QJsonDocument json(QJsonDocument::fromJson(result, &jsonParseError));
        // if json is not valid, send the response as byte array!
        if (jsonParseError.error != QJsonParseError::NoError)
            emit finished(statusCode, result);
        // if is a valid json object, send a object
        else if (json.isObject())
            emit finished(statusCode, json.object().toVariantMap());
        // if is a valid json array, send a array
        else if (json.isArray())
            emit finished(statusCode, json.array().toVariantList());
    } else {
        // some unknow error like: device is not connected, the server not send any response or any other cases...
        if (statusCode <= 0)
            onError(reply->error());
        else
            emit finished(statusCode, result);
    }
    // delet the reply with all files object (if the request upload files)
    delete reply;
}
