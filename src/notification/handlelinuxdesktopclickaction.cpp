#include "handlelinuxdesktopclickaction.h"
#include <QVariant>

#ifdef Q_OS_ANDROID
// pass
#elif defined(Q_OS_LINUX)
#include <klocalizedstring.h>
#endif

HandleLinuxDesktopClickAction::HandleLinuxDesktopClickAction(QObject *parent) : QObject(parent)
{
}

#ifdef Q_OS_ANDROID
    // pass
#elif defined Q_OS_LINUX
void HandleLinuxDesktopClickAction::setKNotification(KNotification *kNotification)
{
    m_knotification = kNotification;
    connect(m_knotification, SIGNAL(activated(uint)), this, SLOT(onNotificationClicked(uint)));
}
#endif

void HandleLinuxDesktopClickAction::setActionName(const QString &actionName)
{
    m_actionName = actionName;
}

void HandleLinuxDesktopClickAction::onNotificationClicked(uint actionIndex)
{
    Q_UNUSED(actionIndex)
#ifdef Q_OS_ANDROID
    // pass
#elif defined Q_OS_LINUX
    emit notificationClicked(m_actionName, m_knotification->property(m_actionName.toLatin1().constData()));
    emit finished();
#endif
}
