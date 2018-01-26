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

void HandleLinuxDesktopClickAction::setKNotification(KNotification *kNotification)
{
    m_knotification = kNotification;
    connect(m_knotification, SIGNAL(activated(uint)), this, SLOT(onNotificationClicked(uint)));
}

void HandleLinuxDesktopClickAction::setActionName(const QString &actionName)
{
    m_actionName = actionName;
}

void HandleLinuxDesktopClickAction::onNotificationClicked(uint actionIndex)
{
    Q_UNUSED(actionIndex)
    emit notificationClicked(m_actionName, m_knotification->property(m_actionName.toLatin1().constData()));
    emit finished();
}
