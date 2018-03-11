#include "notifyandroid.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QJsonDocument>
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

NotifyAndroid::NotifyAndroid(QObject *parent) : Notification(parent)
{
}

void NotifyAndroid::show(const QString &title, const QString &message, const QVariantMap &argument)
{
    if (title.isEmpty() || message.isEmpty())
        return;
#ifdef Q_OS_ANDROID
    QString messageData;
    if (!argument.isEmpty())
        messageData = QJsonDocument(QJsonDocument::fromVariant(argument)).toJson(QJsonDocument::Compact);

    QAndroidJniObject jTitle(QAndroidJniObject::fromString(title));
    QAndroidJniObject jMessage(QAndroidJniObject::fromString(message));
    QAndroidJniObject jMessageData(QAndroidJniObject::fromString(messageData));

    QtAndroid::androidActivity().callMethod<void>(
        "showNotification",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
        jTitle.object<jstring>(),
        jMessage.object<jstring>(),
        jMessageData.object<jstring>()
    );
#else
    Q_UNUSED(title)
    Q_UNUSED(message)
    Q_UNUSED(argument)
#endif
}

void NotifyAndroid::initialize()
{
}
