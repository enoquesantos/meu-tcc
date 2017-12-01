package org.qtproject.qt5.android.bindings;

// import android.util.Log;

import java.util.Random;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.provider.Settings.System;

public class SystemNotification
{
    private static Class<?> clazz;
    private static Context context;
    private static String packageName;
    private static Resources resources;
    private static SharedPreferences sharedPreferences;
    private static String TAG = "SystemNotification";

    public SystemNotification(Context context, Class<?> clazz)
    {
        this.context = context;
        this.clazz = clazz;
        packageName = context.getPackageName();
        sharedPreferences = context.getSharedPreferences(packageName, Context.MODE_PRIVATE);
        try {
            PackageManager pm = context.getPackageManager();
            resources = pm.getResourcesForApplication(packageName);
        }  catch(Exception e) {
            // Log.e(TAG, Log.getStackTraceString(e));
        }
    }

    private int getIconColor()
    {
        return resources.getColor(getResourcesIdentifier("colorPrimary", "color"));
    }

    private int getIconName()
    {
        return getResourcesIdentifier("icon_transparent", "drawable");
    }

    private int getResourcesIdentifier(String name, String resourceOrigin)
    {
        int resId = 0;
        try {
            resId = resources.getIdentifier(name, resourceOrigin, packageName);
            // Drawable myDrawable = resources.getDrawable(resId);
        } catch(Exception e) {
            // Log.e(TAG, Log.getStackTraceString(e));
        }
        return resId;
    }

    // android system notification has many options!
    // take a look in documentation for more details in follow links:
    // https://developer.android.com/guide/topics/ui/notifiers/notifications.html
    // https://developer.android.com/reference/android/support/v4/app/NotificationCompat.Builder.html
    // messageData will be pass to QtApplication as argument when user click in notification
    public void notify(String title, String message, String sender, String messageData)
    {
        Intent intent = null;
        int messageId = new Random().nextInt(99999);
        int badgeNum = sharedPreferences.getInt("badgeNumber", 0);

        // create a new intent to send to the Activity that will be pass to QtApplication.
        // The push message pay load data will be serialized and sent as argument when user click in the notification
        // the messagaData value can be read as a new Intent extra parameter in onResume(...) method
        // this feature works only if the app is opened!
        if (messageData != null) {
            intent = new Intent(context, clazz);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.putExtra("messageData", messageData);
        }

        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        Notification.Builder notificationBuilder = new Notification.Builder(context);
        // play default notification sound (if available)
        notificationBuilder.setSound(Settings.System.DEFAULT_NOTIFICATION_URI);
        // start vibration if user enable vibration to the app
        notificationBuilder.setVibrate(new long[] {500, 500, 500});
        // start the device LED notification (available only in some devices)
        notificationBuilder.setLights(Color.BLUE, 3000, 3000);

        // create pending intent, a mention to the Activity which needs to be
        // triggered when user clicks in the notification. In this case is QtActivity class
        if (intent != null) {
            PendingIntent contentIntent = PendingIntent.getActivity(context, messageId, intent, Intent.FLAG_ACTIVITY_NEW_TASK);
            notificationBuilder.setContentIntent(contentIntent);
        }

        notificationBuilder.setAutoCancel(true);
        notificationBuilder.setColor(getIconColor());
        notificationBuilder.setSmallIcon(getIconName());
        notificationBuilder.setContentText(message);
        notificationBuilder.setContentTitle(title);
        notificationBuilder.setDefaults(Notification.DEFAULT_SOUND);

        Notification.BigTextStyle bigText = new Notification.BigTextStyle();
        bigText.bigText(message);
        bigText.setBigContentTitle(title);

        try {
            if (message != null && !message.equals("") && sender != null && !sender.equals(""))
                bigText.setSummaryText(":: " + sender);
            notificationBuilder.setStyle(bigText);
        } catch(Exception e) {
            // Log.e("FirebaseListenerService", e);
        }

        // send the message to system tray
        notificationManager.notify(messageId, notificationBuilder.build());

        // update the badge number
        BadgeUtils.incrementBadge(context);

        // send the push notification message to QtApplication
        // if the app is opened (background or foreground)
        if (messageData != null && !messageData.equals("") && CustomActivity.isRunning()) {
            try {
                ActivityToApplication.eventNotify("newPushMessage", messageData);
            } catch(Exception e) {
                // Log.e("FirebaseListenerService", e);
            }
        }
    }
}
