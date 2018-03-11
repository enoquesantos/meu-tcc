#include "requesthttp.h"

#include <QDebug>
#include <QJSEngine>
#include <QList>
#include <QJSValueList>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMapIterator>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>
#include <QUrlQuery>

#include "../core/app.h"
#include "uploadmanager.h"
#include "downloadmanager.h"

QByteArray RequestHttp::m_basicAuthorization = QByteArrayLiteral("");
QByteArray RequestHttp::m_basicAuthorizationUser = QByteArrayLiteral("");
QByteArray RequestHttp::m_basicAuthorizationPassword = QByteArrayLiteral("");

RequestHttp::RequestHttp(QObject *parent) : QObject(parent)
  ,m_status(Status::Ready)
{
    static QVariantMap config;
    if (config.isEmpty()) {
        config = App::instance()->config().value(QStringLiteral("restService")).toMap();
        setBaseUrl(config.value(QStringLiteral("baseUrl")).toByteArray());
        setBasicAuthorization(config.value(QStringLiteral("userName")).toByteArray(), config.value(QStringLiteral("userPass")).toByteArray());
    }
}

void RequestHttp::setBasicAuthorizationUser(const QByteArray &user)
{
    m_basicAuthorizationUser = user;
    if (!m_basicAuthorizationPassword.isEmpty() && m_basicAuthorization.isEmpty())
        setBasicAuthorization(m_basicAuthorizationUser, m_basicAuthorizationPassword);
}

void RequestHttp::setBasicAuthorizationPassword(const QByteArray &password)
{
    m_basicAuthorizationPassword = password;
    if (!m_basicAuthorizationUser.isEmpty() && m_basicAuthorization.isEmpty())
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
    QString userPass(username + QStringLiteral(":") + password);
    m_basicAuthorization = QByteArrayLiteral("Basic ") + userPass.toLocal8Bit().toBase64();
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

void RequestHttp::setBaseUrl(const QByteArray &url)
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

void RequestHttp::connectCallback(QNetworkAccessManager *manager, QJSValue callback)
{
    QJSValue *_callback = new QJSValue(callback);
    connect(manager, &QNetworkAccessManager::finished, [this, _callback](QNetworkReply *reply) {
        QJsonParseError parseError;
        QByteArray result(reply->readAll());

        QJSValue response;
        QJSValue status(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());

        // try to parse the response data to json document
        QJsonDocument doc(QJsonDocument::fromJson(result, &parseError));

        // if json is not valid, uses the byte array
        if (parseError.error != QJsonParseError::NoError)
            response = QString(result);
        // if is a valid json object, uses a object
        else if (doc.isObject())
            response = _callback->engine()->toScriptValue<QVariantMap>(doc.object().toVariantMap());
        // if is a valid json array, uses a array
        else if (doc.isArray())
            response = _callback->engine()->toScriptValue<QVariantList>(doc.array().toVariantList());

        setStatus(Status::Finished);
        _callback->call(QJSValueList{status, response});

        reply->deleteLater();
        reply->manager()->deleteLater();
    });
}

void RequestHttp::downloadFile(const QStringList &urls, bool saveInAppDirectory, const QVariantMap &headers)
{
    DownloadManager *downloadManager = new DownloadManager(this);

    // create a connection to emit download progress changes
    connect(downloadManager, SIGNAL(downloadProgressChanged(qint64, qint64)), this, SIGNAL(downloadProgressChanged(qint64, qint64)));

    // create a connection to emit downloaded file saved signal with the file path
    connect(downloadManager, SIGNAL(fileSaved(QByteArray)), this, SIGNAL(downloadFileSaved(QByteArray)));

    // create a connection to handle the pointer deletion
    connect(downloadManager, &DownloadManager::finished, [this](DownloadManager* dm) {
        emit downloadFinished();
        delete dm;
    });

    QVariantMap requestHeaders(headers);

    // append request header authentication if isset
    if (!m_basicAuthorization.isEmpty())
        requestHeaders.insert(QStringLiteral("Authorization"), m_basicAuthorization);

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
    connect(uploadManager, SIGNAL(uploadProgressChanged(qint64, qint64)), this, SIGNAL(uploadProgressChanged(qint64, qint64)));

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
        requestHeaders.insert(QStringLiteral("Authorization"), m_basicAuthorization);

    // set status to loading
    setStatus(Status::Loading);

    // start the upload
    uploadManager->uploadFile(m_baseUrl.isEmpty() || url.contains(QByteArrayLiteral("http")) ? url : m_baseUrl + url, filePathsList, requestHeaders, usesPutMethod);
}

void RequestHttp::get(const QByteArray &url, const QVariantMap &urlArgs, const QVariantMap &headers, QJSValue callback)
{
    QNetworkRequest request;
    initRequest(&request, url, headers, urlArgs);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // if this method was called from QML object and a javascript
    // function was sent, create a connection to function receive the request results
    if (callback.isCallable())
        connectCallback(manager, callback);
    else
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void RequestHttp::post(const QByteArray &url, const QVariant &postData, const QVariantMap &headers, QJSValue callback)
{
    QNetworkRequest request;
    initRequest(&request, url, headers);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // if this method was called from QML object and a javascript
    // function was sent, create a connection to function receive the request results
    if (callback.isCallable())
        connectCallback(manager, callback);
    else
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QNetworkReply *reply = manager->post(request, postData.toByteArray());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void RequestHttp::initRequest(QNetworkRequest *request, const QByteArray &url, const QVariantMap &headers, const QVariantMap &urlArgs)
{
    QUrl qurl(m_baseUrl.isEmpty() || url.contains(QByteArrayLiteral("http")) ? url : m_baseUrl + url);

    if (urlArgs.size())
        qurl.setQuery(urlQueryFromMap(urlArgs));

    request->setUrl(qurl);
    request->setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("application/json"));
    request->setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    if (!m_basicAuthorization.isEmpty())
        request->setRawHeader(QByteArrayLiteral("Authorization"), m_basicAuthorization);

    if (!headers.isEmpty())
        setHeaders(headers, request);

    setStatus(Status::Loading);
}

void RequestHttp::onError(QNetworkReply::NetworkError code)
{
    setStatus(Status::Error);
    emit error(code, QString(QStringLiteral("Network reply with error code %1")).arg(code));
}

void RequestHttp::onFinished(QNetworkReply *reply)
{
    // get the http status code: 200, 400, 500
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    qDebug() << "Response->statusCode: " << statusCode;

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
    reply->deleteLater();

    // delete the reply manager
    reply->manager()->deleteLater();
}
