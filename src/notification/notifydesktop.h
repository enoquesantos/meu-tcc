#ifndef NOTIFYDESKTOP_H
#define NOTIFYDESKTOP_H

#include <QObject>
#include <QSystemTrayIcon>
#include "notification.h"

class HandleLinuxDesktopClickAction;

class NotifyDesktop : public Notification
{
    Q_OBJECT
public:
    /**
     * @brief NotifyDesktop
     * @param parent QObject* the object parent
     */
    explicit NotifyDesktop(QObject *parent = nullptr);

    /**
     * @brief show
     * @implements
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
     * In Linux Desktop, this method copy the KDE Plasma config file to the application config folder in user home directory.
     */
    void initialize();

private:
    /**
     * @brief m_icon
     */
    QIcon m_icon;

    /**
     * @brief m_trayIcon
     */
    QSystemTrayIcon m_trayIcon;

    /**
     * @brief m_notificationConfigFileName
     */
    QString m_notificationConfigFileName;
};

#endif // NOTIFYDESKTOP_H
