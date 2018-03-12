package org.qtproject.qt5.android.bindings;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;

import com.google.firebase.iid.FirebaseInstanceId;
import com.google.firebase.iid.FirebaseInstanceIdService;

public class FirebaseInstanceIDListenerService extends FirebaseInstanceIdService
{
    private static String TAG = "FirebaseInstanceIDListenerService";

    /**
     * call by Firebase API when token is registered or when is updated
     * 1: When app is started at first time, a new token is registered
     * 2: when updated, that occur if the security of the previous token had been compromised
     * This call is initiated by the InstanceID provider for android service
     */
    @Override
    public void onTokenRefresh()
    {
        String token = FirebaseInstanceId.getInstance().getToken();
        Context c = getApplicationContext();
        SharedPreferences sp = c.getSharedPreferences(c.getPackageName(), Context.MODE_PRIVATE);

        // when token is generated, we need to save. because, the firebase
        // register the token once time, and if user close the app
        // or some event occur (battery is down, or device is poweroff)
        // in next app startup, we need the token to send to Qt Apllication
        if (token != null && !token.equals("")) {
            SharedPreferences.Editor prefsEditor = sp.edit();
            prefsEditor.putString("push_notification_token_id", token);
            prefsEditor.apply();

            Log.i(TAG, "Firebase token is: ");
            Log.i(TAG, token);

            // after save, we need to pass the token to Qt application.
            // this method is connected with Qt application via JNI
            // the method assignature is:
            // ActivityToApplication.eventNotify(String eventName, String data);
            try {
                if (CustomActivity.isRunning()) {
                    Log.i(TAG, "sending firebase token to QtApplication...");
                    ActivityToApplication.eventNotify("newPushNotificationToken", token);
                }
            } catch(Exception e) {
                Log.e(TAG, Log.getStackTraceString(e));
            }
        }
    }
}
