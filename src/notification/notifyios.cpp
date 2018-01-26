#include "notifyios.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

#ifdef Q_OS_IOS
// Objective-C (with modules enabled)
#import <UIKit/UIKit.h>
@import UserNotifications;
#include <QJsonDocument>
#endif

NotifyIOS::NotifyIOS(QObject *parent) : Notification(parent)
{
}

void NotifyIOS::show(const QString &title, const QString &message, const QVariantMap &argument)
{
    if (title.isEmpty() || message.isEmpty())
        return;
#ifdef Q_OS_IOS
    QString messageData(QStringLiteral(""));
    if (!actionName.isEmpty() && !actionValue.isNull()) {
        QVariantMap map({{actionName, actionValue}});
        QJsonDocument doc(QJsonDocument::fromVariant(map));
        messageData = doc.toJson(QJsonDocument::Compact);
    }

    // Objective-C
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];

    // Objective-C
    [center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings * _Nonnull settings) {
      if (settings.authorizationStatus != UNAuthorizationStatusAuthorized) {
          NSLog(@"Notifications not allowed!");
          return;
      }
    }];

    // creating a notification request in Objective-C
    UNMutableNotificationContent *localNotification = [UNMutableNotificationContent new];
    localNotification.title     = @title.toNSString();
    localNotification.body      = @message.toNSString();
    localNotification.userInfo  = @messageData.toNSString();
    localNotification.sound     = [UNNotificationSound defaultSound];
    localNotification.badge     = @([[UIApplication sharedApplication] applicationIconBadgeNumber] +1);

    UNTimeIntervalNotificationTrigger *trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:5 repeats:NO];
    UNNotificationRequest *request = [UNNotificationRequest requestWithIdentifier:@title.toNSString() content:localNotification trigger:trigger];

    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
        NSLog(@"Notification created!");
    }];
#else
    Q_UNUSED(title)
    Q_UNUSED(message)
    Q_UNUSED(argument)
#endif
}

void NotifyIOS::initialize()
{
#ifdef Q_OS_IOS
    // open ios dialog to request to show notifications
    // Objective-C
    UNAuthorizationOptions options = UNAuthorizationOptionAlert + UNAuthorizationOptionSound;
    [center requestAuthorizationWithOptions:options completionHandler:^(BOOL granted, NSError * _Nullable error) {
        if (!granted) NSLog(@"Something went wrong!");
    }];
#endif
}
