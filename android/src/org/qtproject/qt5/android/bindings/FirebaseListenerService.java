package org.qtproject.qt5.android.bindings;

// import android.util.Log;

import java.util.Map;

import android.content.Context;

import com.google.firebase.messaging.RemoteMessage;
import com.google.firebase.messaging.FirebaseMessagingService;

import org.json.JSONObject;

/**
 * Please! To read about Firebase Push Notification, take a look in Firebase Documentation Message API
 * https://developers.google.com/android/reference/com/google/firebase/messaging/RemoteMessage
 * The version of Firebase Messaging used is especify in build.gradle file in android folder
 * at: compile 'com.google.firebase:firebase-messaging:9.6.1'
 * The next job is to append messages in single notification like whatsapp:
 * https://stackoverflow.com/questions/33040737/how-to-group-android-notifications-like-whatsapp
 */
public class FirebaseListenerService extends FirebaseMessagingService
{
    /**
     * Called when message is received and app is running (background or foreground).
     *
     * @param from SenderID of the sender.
     * @param data Data bundle containing message data as key/value pairs.
     *             For Set of keys use data.keySet().
     * To receive notification when app is in background or closed, the message send to firebase
     * cannot have the 'notification' key! And all data needs to be in 'data' keywork.
     * To more details, check the follow links:
     * https://developers.google.com/android/reference/com/google/firebase/messaging/RemoteMessage.Notification
     * https://stackoverflow.com/questions/42480002/findout-when-firebase-message-recieved-when-app-in-background
     * https://stackoverflow.com/questions/37711082/how-to-handle-notification-when-app-in-background-in-firebase#answer-37845174
     */
    @Override
    public void onMessageReceived(RemoteMessage remoteMessage)
    {
        //
        RemoteMessage.Notification notification = remoteMessage.getNotification();
        Map<String,String> extraData = remoteMessage.getData();

        // whe app is in background or closed, the notification object
        // is null, because all push data become in remoteMessage.getData()
        if (notification != null) {
            String title = notification.getTitle();
            String message = notification.getBody();
            String messageData = "";

            if (message == null)
                message = extraData.get("body");

            // this JSONObject is needed to serialize the Map as string
            // to pass to QtApplication as argument when user click in notification
            JSONObject json = null;
            try {
                json = new JSONObject(extraData);
                messageData = json.toString();
            } catch(Exception e) {
                // Log.e("FirebaseListenerService", e);
            }

            // show system notification
            new SystemNotification(getApplicationContext(), CustomActivity.class).notify(title, message, remoteMessage.getFrom(), messageData);
        }
    }
}
