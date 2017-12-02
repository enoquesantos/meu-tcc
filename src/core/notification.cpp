#include "notification.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

#ifdef Q_OS_DARWVIN
// https://useyourloaf.com/blog/local-notifications-with-ios-10/
// https://developer.apple.com/documentation/uikit/uilocalnotification
#endif

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

#ifdef Q_OS_LINUX
#ifndef Q_OS_ANDROID
#include <klocalizedstring.h>

Private::HandleLinuxDesktopClickAction::HandleLinuxDesktopClickAction(QObject *parent) : QObject(parent)
{
}

void Private::HandleLinuxDesktopClickAction::setKNotification(KNotification *kNotification)
{
    m_knotification = kNotification;
    connect(m_knotification, SIGNAL(activated(uint)), this, SLOT(onNotificationClicked(uint)));
}

void Private::HandleLinuxDesktopClickAction::setActionName(const QString &actionName)
{
    m_actionName = actionName;
}

void Private::HandleLinuxDesktopClickAction::onNotificationClicked(uint actionIndex)
{
    Q_UNUSED(actionIndex)
    emit notificationClicked(m_actionName, m_knotification->property(m_actionName.toLatin1().constData()));
    emit finished();
}
#endif
#endif

Notification::Notification(QObject *parent) : QObject(parent)
  ,m_notificationConfigFileName(QStringLiteral("notification-linux-config.notifyrc"))
{
    initialize();
}

void Notification::initialize()
{
#ifdef Q_OS_ANDROID
#endif
#ifdef Q_OS_LINUX
    QFile file(QStringLiteral(":/") + m_notificationConfigFileName);

    if (!file.exists()) {
        #ifdef QT_DEBUG
                qWarning() << QStringLiteral("The notifyrc file cannot found!");
        #endif
        return;
    }

    QDir destinationDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    if (!destinationDir.exists() && !destinationDir.mkdir(destinationDir.absolutePath())) {
        #ifdef QT_DEBUG
                qWarning() << QStringLiteral("The application 'GenericDataLocation' directory cannot be created!");
        #endif
    }

    QString notificationConfigDestination(destinationDir.absolutePath() + QStringLiteral("/"));
    notificationConfigDestination.append(QApplication::applicationName() + QStringLiteral(".notifyrc"));

    if (QFile::exists(notificationConfigDestination))
        return;

    if (!file.copy(notificationConfigDestination)) {
        #ifdef QT_DEBUG
            qWarning() << QStringLiteral("The notifyrc file cannot be copied!");
        #endif
    }
#endif
}

void Notification::sendNotification(const QString &title, const QString &message, const QString &actionName, const QVariant &actionValue)
{
    if (title.isEmpty())
        return;
#ifdef Q_OS_ANDROID
    notifyAndroid(title, message, actionName, actionValue);
#elif defined(Q_OS_IOS)
    notifyIos(title, message, actionName, actionValue);
#elif defined(Q_OS_DARWIN)
    // add macos notification process
    m_trayIcon.showMessage(title, message, QSystemTrayIcon::Information, 3000);
#elif defined(Q_OS_LINUX)
    QPixmap icon;
    icon.load(":app-icon.svg");
    if (icon.isNull())
        qWarning() << "icon is null!";

    auto *notificationHandle = new Private::HandleLinuxDesktopClickAction(this);
    notificationHandle->setActionName(actionName);

    auto *knotification = new KNotification(QStringLiteral("Notify"), KNotification::RaiseWidgetOnActivation, notificationHandle);
    knotification->setProperty(actionName.toLatin1().constData(), actionValue);
    knotification->setTitle(title);
    knotification->setText(message);
    knotification->setPixmap(icon);
    knotification->setDefaultAction(QStringLiteral(""));
    knotification->setActions(QStringList(QStringLiteral("Details")));

    // set this pointer to the handle retrieve the specific action data
    // and notify application with correct notification argument
    notificationHandle->setKNotification(knotification);

    // when user click in the notification, notify the application sending the notification arguments
    connect(notificationHandle, SIGNAL(notificationClicked(QString, QVariant)), this, SIGNAL(notificationClicked(QString, QVariant)));

    // after notification finish, (when user click in the notification) delete the object
    connect(notificationHandle, SIGNAL(finished(void)), notificationHandle, SLOT(deleteLater(void)));

    // send the message to notification server (system tray)
    knotification->sendEvent();
#endif
}

void Notification::notifyAndroid(const QString &title, const QString &message, const QString &actionName, const QVariant &actionValue)
{
#ifdef Q_OS_ANDROID
    QString messageData;
    if (!actionName.isEmpty() && !actionValue.isNull()) {
        QVariantMap map({{actionName, actionValue}});
        QJsonDocument doc(QJsonDocument::fromVariant(map));
        messageData = doc.toJson(QJsonDocument::Compact);
    }

    QAndroidJniObject jTitle(QAndroidJniObject::fromString(title));
    QAndroidJniObject jMessage(QAndroidJniObject::fromString(message));
    QAndroidJniObject jSender(QAndroidJniObject::fromString(QStringLiteral("")));
    QAndroidJniObject jMessageData(QAndroidJniObject::fromString(messageData));

    QtAndroid::androidActivity().callMethod<void>(
        "showNotification",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
        jTitle.object<jstring>(),
        jMessage.object<jstring>(),
        jSender.object<jstring>(),
        jMessageData.object<jstring>()
    );
#elif defined(Q_OS_LINUX)
    Q_UNUSED(title)
    Q_UNUSED(message)
    Q_UNUSED(actionName)
    Q_UNUSED(actionValue)
#endif
}

// https://useyourloaf.com/blog/local-notifications-with-ios-10/
// https://developer.apple.com/documentation/uikit/uilocalnotification
void Notification::notifyIos(const QString &title, const QString &message, const QString &actionName, const QVariant &actionValue)
{
#ifndef Q_OS_IOS
    Q_UNUSED(title)
    Q_UNUSED(message)
    Q_UNUSED(actionName)
    Q_UNUSED(actionValue)
#endif
}
