#ifndef NOTIFYIOS_H
#define NOTIFYIOS_H

#include <QObject>
#include "notification.h"

class NotifyIOS : public Notification
{
    Q_OBJECT
public:
    /**
     * @brief NotifyIOS
     * @param parent QObject* the object parent
     */
    explicit NotifyIOS(QObject *parent = nullptr);

    /**
     * @brief show
     *
     * Show a system notification using a title, message and some action.
     * The action is used to pass arguments to application after user click in notification
     * The action argument needs to have a key and value and will be used to identify the action when are sent to application, after user click in notification
     *
     * https://useyourloaf.com/blog/local-notifications-with-ios-10/
     * https://developer.apple.com/documentation/uikit/uilocalnotification
     * https://medium.com/@jamesrochabrun/local-notifications-are-a-great-way-to-send-notifications-to-the-user-without-the-necessity-of-an-b3187e7176a3
     *
     * @param title QString the notification title
     * @param message QString the notification body message
     * @param argument QVariantMap the custom action to append in the notification.
     */
    Q_INVOKABLE void show(const QString &title, const QString &message, const QVariantMap &argument = QVariantMap()) override;

protected:
    /**
     * @brief initialize
     * Initialize the notification tasks.
     * In iOS, show a permission request dialog to user authorize the app show notifications.
     */
    void initialize();
};

#endif // NOTIFYIOS_H
