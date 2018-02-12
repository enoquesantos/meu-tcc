#ifndef HANDLELINUXDESKTOPCLICKACTION_H
#define HANDLELINUXDESKTOPCLICKACTION_H

#include <QObject>

#ifdef Q_OS_ANDROID
// pass
#elif defined Q_OS_LINUX
#include <KNotification>
#endif

class HandleLinuxDesktopClickAction : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief HandleLinuxDesktopClickAction
     * @param parent QObject*
     */
    explicit HandleLinuxDesktopClickAction(QObject *parent = nullptr);

#ifdef Q_OS_ANDROID
    // pass
#elif defined Q_OS_LINUX
    /**
     * @brief setKNotification
     * @param kNotification KNotification
     */
    void setKNotification(KNotification *kNotification);
#endif

    /**
     * @brief setActionName
     * @param actionName QString
     */
    void setActionName(const QString &actionName);

public slots:
    /**
     * @brief onNotificationClicked
     * @param actionIndex uint
     */
    void onNotificationClicked(uint actionIndex);

signals:
    /**
     * @brief finished
     */
    void finished();

    /**
     * @brief notificationClicked
     * @param actionName QString
     * @param actionValue QVariant
     */
    void notificationClicked(const QString &actionName, const QVariant &actionValue);

private:
    /**
     * @brief m_actionName
     */
    QString m_actionName;

    /**
     * @brief m_knotification
     */
#ifdef Q_OS_ANDROID
    // pass
#elif defined Q_OS_LINUX
    KNotification *m_knotification;
#endif

};

#endif // HANDLELINUXDESKTOPCLICKACTION_H
