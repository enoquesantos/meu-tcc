#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QVariant>

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
     * @param parent QObject* the object parent
     */
    explicit Notification(QObject *parent = nullptr) : QObject(parent) { }

    /**
     * @brief show
     * @abstract
     * Show a system notification using a title, message and some action.
     * The action is used to pass arguments to application after user click in notification
     * The action argument needs to have a key and value and will be used to identify the action when are sent to application, after user click in notification
     * @param title QString the notification title
     * @param message QString the notification body message
     * @param argument QVariantMap the custom action to append in the notification.
     */
    Q_INVOKABLE virtual void show(const QString &title, const QString &message, const QVariantMap &argument = QVariantMap()) = 0;

    /**
     * @brief initialize
     * @abstract
     * Initialize the notification tasks.
     * In iOS, show a permission request dialog to user authorize the app show notifications.
     * In Android, the permission dialog request is displayed to the user only in version 7 (nougat) or above,
     * in versions before android 7 the permission is made in AndroidManifest.xml.
     * In Linux Desktop, copy a KDE Plasma config file to the application config folder in user home.
     */
    virtual void initialize() = 0;

signals:
    /**
     * @brief notificationClicked
     * This signal is emitted when user click in notification and is valid only for desktop.
     * @param actionName QString the action name read from the notification
     * @param actionValue QVariant the notification action value, like a string or json object
     */
    void clicked(const QString &actionName, const QVariant &actionValue);
};

#endif // NOTIFICATION_H
