#ifndef NOTIFICATIONHANDLE_H
#define NOTIFICATIONHANDLE_H

#include "notification.h"

#ifdef Q_OS_ANDROID
#include "notifyandroid.h"
#elif defined Q_OS_IOS
#include "notifyios.h"
#else
#include "notifydesktop.h"
#endif

static Notification* notificationHandle(QObject *parent = nullptr)
{
    Notification* notification = nullptr;
#ifdef Q_OS_ANDROID
    notification = new NotifyAndroid(parent);
#elif defined Q_OS_IOS
    notification = new NotifyIOS(parent);
#else
    notification = new NotifyDesktop(parent);
#endif
    notification->initialize();
    return notification;
}

#endif // NOTIFICATIONHANDLE_H
