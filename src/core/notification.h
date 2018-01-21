#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QVariant>

#ifdef Q_OS_ANDROID
#elif defined(Q_OS_LINUX)
#include <KNotification>
#endif

/**
 * @brief The Notification class
 * @extends QObject
 * This class handle system notifications (local only, sent from the application)
 *
 * In android and iOS, the notification can have some data that will be passed to application
 * when user click in the notification.
 *
 * In Desktop mode, the notification works fine in KDE Plasma, in other systems the notification
 * uses the Qt QSystemTrayIcon to show balloon messages, and the action data may not works well.
 */
class Notification : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Notification
     * @param parent QObject * the object parent
     */
    explicit Notification(QObject *parent = nullptr);

    /**
     * @brief show
     * Show a system notification using a title, message and some action.
     * The action is used to pass arguments to application after user click in notification
     * @param title QString the notification title
     * @param message QString the notification body message
     * @param actionName QString the action name to identify the argument value
     * @param actionValue QVariant the action data to be passed to application when user click in notification
     */
    Q_INVOKABLE void show(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));

private:
    /**
     * @brief initialize
     * Initialize the notification tasks.
     * In iOS, show a permission request dialog to app show notifications.
     * In Android, the permission request is make on app startup by java objects.
     * In Linux Desktop, copy a KDE Plasma config file to the application config folder in user home.
     */
    void initialize();

    /**
     * @brief notifyAndroid
     * This method is called by 'show' method if OS is android.
     * Internally, requests a android java activity method to send a notification to system tray.
     * @param title QString the notification title
     * @param message QString the notification body message
     * @param actionName QString the action name to identify the argument value
     * @param actionValue QVariant the action data to be passed to application when user click in notification
     */
    void notifyAndroid(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));

    /**
     * @brief notifyIos
     * * This method is called by 'show' method if OS is ios.
     * Internally, requests a ObjectiveC method to send a notification to system tray.
     * @param title QString the notification title
     * @param message QString the notification body message
     * @param actionName QString the action name to identify the argument value
     * @param actionValue QVariant the action data to be passed to application when user click in notification
     */
    void notifyIos(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));

signals:
    /**
     * @brief notificationClicked
     * This signal is emitted when user click in notification, valid for desktop only!
     * @param actionName QString the action name read from the notification
     * @param actionValue QVariant the notification action value, like a string or json object
     */
    void notificationClicked(const QString &actionName, const QVariant &actionValue);

private:
    QIcon m_icon;
    QStringList m_actions;
    QSystemTrayIcon m_trayIcon;
    QString m_notificationConfigFileName;
};

#ifdef Q_OS_ANDROID
#elif defined(Q_OS_LINUX)
namespace Private {
class HandleLinuxDesktopClickAction : public QObject
{
    Q_OBJECT
public:
    explicit HandleLinuxDesktopClickAction(QObject *parent = nullptr);
    void setKNotification(KNotification *kNotification);
    void setActionName(const QString &actionName);

public slots:
    void onNotificationClicked(uint actionIndex);

signals:
    void finished();
    void notificationClicked(const QString &actionName, const QVariant &actionValue);

private:
    QString m_actionName;
    KNotification *m_knotification;
};
}
#endif
#endif // NOTIFICATION_H
