#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QVariant>

#ifdef Q_OS_LINUX
#ifndef Q_OS_ANDROID
#include <KNotification>
#endif
#endif

class Notification : public QObject
{
    Q_OBJECT
public:
    explicit Notification(QObject *parent = nullptr);
    Q_INVOKABLE void sendNotification(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));

private:
    void initialize();
    void notifyAndroid(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));
    void notifyIos(const QString &title, const QString &message, const QString &actionName = QStringLiteral(""), const QVariant &actionValue = QStringLiteral(""));

signals:
    void notificationClicked(const QString &actionName, const QVariant &actionValue);

private:
    QIcon m_icon;
    QStringList m_actions;
    QSystemTrayIcon m_trayIcon;
    QString m_notificationConfigFileName;
};

#ifdef Q_OS_LINUX
#ifndef Q_OS_ANDROID
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
#endif
#endif // NOTIFICATION_H
