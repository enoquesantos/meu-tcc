#include "Firebase/Firebase.h"
#import <UIKit/UIKit.h>

#include <QString>
#include <QtCore>
#include "../src/core/app.h"

@interface QIOSApplicationDelegate
@end
@interface QIOSApplicationDelegate(QtAppDelegate)
@end

@implementation QIOSApplicationDelegate (QtAppDelegate)

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    Q_UNUSED(application)
    Q_UNUSED(launchOptions)
    [self configFirebase];
    // NSLog(@"Firebase connect start!");
    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    Q_UNUSED(application)
    [[FIRMessaging messaging] disconnect];
    // NSLog(@"Firebase disconnect!");
    // NSLog(@"enter in applicationDidEnterBackground method");
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    Q_UNUSED(application)
    // Restart any tasks that were paused (or not yet started) while the application was inactive.
    // If the application was previously in the background, optionally refresh the user interface.
    [self connectToFcm];

    // NSLog(@"Conectou com o FCM");
    // NSLog(@"enter in applicationDidBecomeActive method");

    // clear the app icon badge with notification count when app started
    // http://stackoverflow.com/questions/27311910/how-to-clear-badge-counter-on-click-of-app-icon-in-iphone
    [[UIApplication sharedApplication] setApplicationIconBadgeNumber:0];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    Q_UNUSED(application)
    // NSLog(@"enter in applicationWillTerminate method");
}

- (void)applicationWillResignActive:(UIApplication *)application {
    Q_UNUSED(application)
    // NSLog(@"enter in applicationWillResignActive method");
}

-(void)configFirebase {
    // iOS 8 or later
    // [START register_for_notifications]
    UIUserNotificationType allNotificationTypes =  (UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge);
    UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:allNotificationTypes categories:nil];
    [[UIApplication sharedApplication] registerUserNotificationSettings:settings];
    [[UIApplication sharedApplication] registerForRemoteNotifications];
    // [END register_for_notifications]

    // [START configure_firebase]
    [FIRApp configure];
    // [END configure_firebase]

    // Add observer for InstanceID token refresh callback.
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tokenRefreshNotification:) name:kFIRInstanceIDTokenRefreshNotification object:nil];

    // NSLog(@"Configurando Firebase...");
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)push
  fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
    Q_UNUSED(completionHandler)
    Q_UNUSED(application)

    // NSLog(@"Nova mensagem do firebase");
    // convert the json nsdictionary to string
    // https://stackoverflow.com/questions/27054352/issue-in-converting-nsdictionary-to-json-string-replacing-with
    NSError *err;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:push options:0 error:&err];
    NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    // send the push message data (as string json) to Qt Application
    App::fireEventNotify("newPushMessage", QString::fromNSString(jsonString));
}

- (void)tokenRefreshNotification:(NSNotification *)notification {
    Q_UNUSED(notification)
    NSString *refreshedToken = [[FIRInstanceID instanceID] token];
    // NSLog(@"The token was updated: %@", refreshedToken);
    App::fireEventNotify("pushNotificationToken", QString::fromNSString(refreshedToken));
    // Connect to FCM since connection may have failed when attempted before having a token.
    [self connectToFcm];
}

- (void)connectToFcm {
    [[FIRMessaging messaging] connectWithCompletion:^(NSError * _Nullable error) {
        Q_UNUSED(error)
//        if (error != nil)
//            NSLog(@"Unable to connect to FCM. %@", error);
//        else
//            NSLog(@"Conectou com o FCM.");
    }];
}

+ (BOOL)notificationServicesEnabled {
    BOOL isEnabled = NO;
    if ([[UIApplication sharedApplication] respondsToSelector:@selector(currentUserNotificationSettings)]) {
        UIUserNotificationSettings *notificationSettings = [[UIApplication sharedApplication] currentUserNotificationSettings];
        isEnabled = (!notificationSettings || (notificationSettings.types == UIUserNotificationTypeNone));
    } else {
        UIRemoteNotificationType types = [[UIApplication sharedApplication] enabledRemoteNotificationTypes];
        isEnabled = (types & UIRemoteNotificationTypeAlert);
    }
    return isEnabled;
}
@end
