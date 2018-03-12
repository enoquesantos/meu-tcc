#include "utils.h"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkConfigurationManager>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

Utils* Utils::m_instance = nullptr;

Utils::Utils(QObject *parent) : QObject(parent)
{
}

Utils::~Utils()
{
}

Utils *Utils::instance()
{
    if (m_instance == nullptr)
        m_instance = new Utils;
    return m_instance;
}

QString Utils::stringfyJson(const QVariant &data, bool compact)
{
    if (data.userType() == QMetaType::QVariantList) {
        QJsonDocument doc(QJsonArray::fromVariantList(data.toList()));
        if (doc.isArray() && !doc.isEmpty())
            return QString(doc.toJson(compact ? QJsonDocument::Compact : QJsonDocument::Indented));
    } else {
        if (data.userType() == QMetaType::QVariantMap) {
            QJsonDocument doc(QJsonObject::fromVariantMap(data.toMap()));
            if (doc.isObject() && !doc.isEmpty())
                return QString(doc.toJson(compact ? QJsonDocument::Compact : QJsonDocument::Indented));
        }
    }
    return QStringLiteral("");
}

QVariant Utils::jsonParse(const QByteArray &jsonStr)
{
    QJsonDocument qJsonDocument(QJsonDocument::fromJson(jsonStr));
    if (qJsonDocument.isArray())
        return qJsonDocument.array().toVariantList();
    return qJsonDocument.object().toVariantMap();
}

QString Utils::parseUrl(const QString &url)
{
    if (url.isEmpty())
        return QStringLiteral("");
    QUrl _qurl = QUrl::fromUserInput(url);
    if (url.at(0) == QStringLiteral("/"))
        return _qurl.toString();
    return _qurl.toString();
}

QString Utils::linkifyText(const QByteArray &text) {
    return QString(text).replace(QRegExp("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

QVariant Utils::readFile(const QString &filePath)
{
    QString _path(filePath);
    QFile file(_path.remove(QStringLiteral("file://")));
    QJsonParseError error;
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QVariant();
    QByteArray fileContent(file.readAll());
    file.close();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(fileContent, &error);
    if (error.error == QJsonParseError::NoError && !jsonDocument.isEmpty()) {
        if (jsonDocument.isArray())
            return jsonDocument.array().toVariantList();
        else if (jsonDocument.isObject())
            return jsonDocument.object().toVariantMap();
    }
    return fileContent;
}

bool Utils::copyFile(const QString &fileName, const QString &newName, bool overwriteIfExists)
{
    QFile file(fileName);
    if (overwriteIfExists && file.exists())
        file.remove();
    return file.copy(newName);
}

QString Utils::fileBaseName(const QString &filePath)
{
    // basename contains the file name from url origin
    if (filePath.contains(QStringLiteral("http")))
        return QFileInfo(QUrl(filePath).path()).fileName();
    return QFileInfo(filePath).fileName();
}

bool Utils::isDeviceOnline()
{
    QNetworkConfigurationManager qcm(this);
    return qcm.isOnline();
}

QString Utils::readFirebaseToken()
{
    QString token;
#ifdef Q_OS_ANDROID
    return QtAndroid::androidActivity().callObjectMethod("getFirebaseToken", "()Ljava/lang/String;").toString();
#endif
    return token;
}

void Utils::minimizeWindow()
{
#ifdef Q_OS_ANDROID
    QtAndroid::androidActivity().callMethod<jboolean>("moveTaskToBack", "(Z)Z", true);
#endif
}
