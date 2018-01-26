#ifndef NOTIFYANDROID_H
#define NOTIFYANDROID_H

#include <QObject>
#include "notification.h"

class NotifyAndroid : public Notification
{
    Q_OBJECT
public:
    /**
     * @brief NotifyAndroid
     * @param parent QObject* the object parent
     */
    explicit NotifyAndroid(QObject *parent = nullptr);

    /**
     * @brief show
     * Show a system notification using a title, message and some action.
     * The action is used to pass arguments to application after user click in notification
     * The action argument needs to have a key and value and will be used to identify the action when are sent to application, after user click in notification
     * @param title QString the notification title
     * @param message QString the notification body message
     * @param argument QVariantMap the custom action to append in the notification.
     */
    Q_INVOKABLE void show(const QString &title, const QString &message, const QVariantMap &argument = QVariantMap()) override;

protected:
    /**
     * @brief initialize
     * Initialize the notification tasks.
     * In Android, the permission dialog request is displayed to the user only in version 7 (nougat) or above,
     * in versions before android 7 the permission is made in AndroidManifest.xml.
     */
    void initialize();
};

#endif // NOTIFYANDROID_H
