#include "utils.h"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QQuickItem>
#include <QQmlEngine>

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

QString Utils::readFirebaseToken()
{
    QString token;
#ifdef Q_OS_ANDROID
    if (token.isEmpty())
        return QtAndroid::androidActivity().callObjectMethod("getPushNotificationToken", "()Ljava/lang/String;").toString();
#endif
    return token;
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

void Utils::setQmlEngine(QQmlEngine *engine)
{
    m_engine = engine;
}

void Utils::createSwipeViewPages(const QVariantList &pages, QObject *swipeView, QObject *tabBar, const QString &userProfileName)
{
    if (m_engine == nullptr)
        return;
    int index = 0;
    QVariantMap pageMap;
    foreach (const QVariant &page, pages) {
        pageMap = page.toMap();
        if (!pageMap.value(QStringLiteral("showInTabBar")).toBool())
            continue;
        else if (!userProfileName.isEmpty() && !pageMap.value(QStringLiteral("roles")).toStringList().contains(userProfileName))
            continue;
        QQmlComponent component(m_engine, pageMap.value(QStringLiteral("absPath")).toString(), swipeView);
        QQuickItem *item = qobject_cast<QQuickItem*>(component.create(QQmlEngine::contextForObject(swipeView)));
        if (component.isError()) {
            qWarning() << QStringLiteral("Error on create page: ") << component.errors();
        } else {
            QMetaObject::invokeMethod(swipeView, "addItem", Q_ARG(QQuickItem*, item));
            createTabBarButton((index == 0), pageMap.value(QStringLiteral("title")), pageMap.value(QStringLiteral("icon")), tabBar);
        }
        ++index;
    }
}

void Utils::createTabBarButton(bool isChecked, const QVariant &pageTitle, const QVariant pageIcon, QObject *tabBar)
{
    QQmlComponent component(m_engine, QStringLiteral(":/src/qml/TabBarButton.qml"), tabBar);
    QQuickItem *button = qobject_cast<QQuickItem*>(component.create(QQmlEngine::contextForObject(tabBar)));
    button->setProperty("checked", isChecked);
    button->setProperty("iconName", pageIcon);
    button->setProperty("text", pageTitle);
    QMetaObject::invokeMethod(tabBar, "addItem", Q_ARG(QQuickItem*, button));
}
