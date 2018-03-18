#include "utils.h"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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
    QJsonDocument doc(QJsonDocument::fromJson(jsonStr, &m_jsonParseError));
    if (m_jsonParseError.error == QJsonParseError::NoError && !doc.isEmpty()) {
        if (doc.isArray())
            return doc.array().toVariantList();
        return doc.object().toVariantMap();
    }
    return jsonStr;
}

QString Utils::parseUrl(const QString &url)
{
    if (url.isEmpty())
        return QStringLiteral("");
    QUrl _qurl = QUrl::fromUserInput(url);
    return _qurl.toString();
}

QString Utils::linkifyText(const QByteArray &text) {
    return QString(text).replace(QRegExp("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

QVariant Utils::readFile(const QString &filePath)
{
    QString _path(filePath);
    QFile file(_path.remove(QStringLiteral("file://")));
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QVariant();
    QByteArray fileContent(file.readAll());
    file.close();
    return jsonParse(fileContent);
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
    return QNetworkConfigurationManager().isOnline();
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
