#include "notifydesktop.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#ifdef QT_DEBUG
#include <QtDebug>
#endif

#ifdef Q_OS_LINUX
#include "handlelinuxdesktopclickaction.h"
#endif

NotifyDesktop::NotifyDesktop(QObject *parent) : Notification(parent)
  ,m_notificationConfigFileName(QStringLiteral("notification-linux-config.notifyrc"))
{
}

void NotifyDesktop::show(const QString &title, const QString &message, const QVariantMap &argument)
{
    if (title.isEmpty() || message.isEmpty())
        return;
#ifdef Q_OS_ANDROID
    // pass
    Q_UNUSED(argument)
#elif defined(Q_OS_DARWIN)
    // sent to macOS SystemTray with QSystemTrayIcon as balloon messages
    m_trayIcon.showMessage(title, message, QSystemTrayIcon::Information, 3000);
#elif defined(Q_OS_LINUX)
    QString key(QStringLiteral(""));

    if (!argument.isEmpty())
        key = argument.keys().first();

    QPixmap icon;
    icon.load(":/icon.png");
    if (icon.isNull())
        qWarning() << "icon is null!";

    auto *notificationHandle = new HandleLinuxDesktopClickAction(this);
    notificationHandle->setActionName(key);

    auto *knotification = new KNotification(QStringLiteral("Notify"), KNotification::RaiseWidgetOnActivation, notificationHandle);
    knotification->setProperty(key.toLatin1().constData(), argument.value(key));
    knotification->setTitle(title);
    knotification->setText(message);
    knotification->setPixmap(icon);
    knotification->setDefaultAction(QStringLiteral(""));
    knotification->setActions(QStringList(QStringLiteral("Details")));

    // set this pointer to the handle retrieve the specific action data
    // and notify application with correct notification argument
    notificationHandle->setKNotification(knotification);

    // when user click in the notification, notify the application sending the notification arguments
    connect(notificationHandle, SIGNAL(notificationClicked(QString, QVariant)), this, SIGNAL(clicked(QString, QVariant)));

    // after notification finish, (when user click in the notification) delete the object
    connect(notificationHandle, SIGNAL(finished(void)), notificationHandle, SLOT(deleteLater(void)));

    // send the message to notification server (system tray)
    knotification->sendEvent();
#endif
}

void NotifyDesktop::initialize()
{
    QFile file(QStringLiteral(":/") + m_notificationConfigFileName);

    if (!file.exists()) {
    #ifdef QT_DEBUG
        qWarning() << QStringLiteral("The notifyrc file cannot found!");
        qWarning() << QStringLiteral("The notifyrc file path: ") << file.fileName();
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
}
