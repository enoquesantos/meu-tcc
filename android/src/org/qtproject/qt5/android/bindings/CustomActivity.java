package org.qtproject.qt5.android.bindings;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.widget.Toast;
import android.util.Log;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

public class CustomActivity extends QtActivity
{
    private static Context context;
    private static String TAG = "CustomActivity";
    private static boolean isRunning = false;
    private static final int PERMISSION_REQUEST_CODE = 1;

    public CustomActivity()
    {
        super();
        // set the theme package to configure lollipop (or above) action bar style and others configurations
        // like transparent status bar via style.xml and the window colors via properties from colors.xml.
        QT_ANDROID_THEMES = new String[] {"Theme.AppCompat"};
        QT_ANDROID_DEFAULT_THEME = "Theme.AppCompat";
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        isRunning   = true;
        context     = getApplicationContext();

        // if has badge (pending notification) number in the laucher icon, will be removed
        BadgeUtils.resetBadge(context);

        // if the app was started from a notification click
        // the arguments in notification (from intent extra data) will be sent to Qt application
        onNewIntent(getIntent());

        // if android marshmallow or above
        // show request permission dialog to app read/write in local storage (if is not allowed yet)
        if (Build.VERSION.SDK_INT >= 23 && !isEnabledToHandleStorage())
            requestToHandleStorage();
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        isRunning = false;
    }

    // onNewIntent is a method executed after app is started.
    // can be used to read some argument pass by another app or
    // from system notification (this case). When user click in some notification
    // the argument (messageData) contains a json with push message data that is
    // added by push notification listener. We need to send to QtApplication focus in specific page
    @Override
    protected void onNewIntent(Intent intent)
    {
        super.onNewIntent(intent);
        Bundle bundle = intent.getExtras();
        if (bundle != null) {
            final String messageData = bundle.getString("messageData");
            if (messageData != null && !messageData.equals("")) {
                try {
                    final Handler handler = new Handler();
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            ActivityToApplication.eventNotify("newActionNotification", messageData);
                        }
                    }, 5000);
                } catch(Exception e) {
                    Log.e(TAG, Log.getStackTraceString(e));
                }
            }
        }
        // if has badge (pending notification) number in the laucher icon, will be removed
        BadgeUtils.resetBadge(context);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
    {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        // show system dialog with the permission request
        if (requestCode == PERMISSION_REQUEST_CODE && ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
            sendFirebaseTokenToQtApplication();
            showToast("Permissão para acesso ao armazenamento concedida!", 0);
        } else {
            showToast("O aplicativo precisa de permissão para leitura e escrita no sistema de arquivos. Por favor, autorize nas configurações de aplicativos.", Toast.LENGTH_SHORT);
        }
    }

    /**
     * return the value for application running state. Can be used to prevent call the ActivityToApplication
     * native methods and prevent crash the app in FirebaseNotification background messages.
     * @return boolean
     */
    public static boolean isRunning()
    {
        return isRunning;
    }

    /**
     * check if user is already enable the read/write permission
     * in storage directory for the application
     * @return boolean
     */
    private boolean isEnabledToHandleStorage()
    {
        int result = ContextCompat.checkSelfPermission(context, Manifest.permission.READ_EXTERNAL_STORAGE);
        return (result == PackageManager.PERMISSION_GRANTED);
    }

    /**
     * Show a system dialog for request permission to write in local storage
     * The user need to enable the read/write in internal storage
     * @return void
     */
    private void requestToHandleStorage()
    {
        if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.READ_EXTERNAL_STORAGE))
            showToast("O aplicativo precisa de permissão para leitura e escrita no sistema de arquivos. Por favor, autorize nas configurações de aplicativos.", 1);
        else
            ActivityCompat.requestPermissions(this, new String[] {Manifest.permission.READ_EXTERNAL_STORAGE}, PERMISSION_REQUEST_CODE);
    }

    /**
     * Send a user notification into app window using Toast Widget
     * https://developer.android.com/guide/topics/ui/notifiers/toasts.html
     * @return void
     */
    private void showToast(final String message, final int isLong)
    {
        runOnUiThread(new Runnable() {
            public void run() {
                Toast.makeText(CustomActivity.this, message, isLong == 1 ? Toast.LENGTH_LONG : Toast.LENGTH_SHORT).show();
            }
        });
    }

    /**
     * Get the Firebase Token from application Shared Preferences
     * @return String
     */
    public String getFirebaseToken()
    {
        SharedPreferences sp = context.getSharedPreferences(context.getPackageName(), Context.MODE_PRIVATE);
        return sp.getString("push_notification_token_id", "");
    }

    /**
     * Sent the firebase Token to Qt Application, using a connection with App class
     * with JNI. The ActivityToApplication.eventNotify method is a Java Native method connected
     * with c++ App::eventNotify static method. The connection will be create when main.cpp from
     * Qt application will be loaded.
     * @return void
     */
    public void sendFirebaseTokenToQtApplication()
    {
        try {
            final String token = getFirebaseToken();
            if (token.equals("")) return;
            new java.lang.Thread(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "sending firebase token to QtApplication...");
                    ActivityToApplication.eventNotify("newPushNotificationToken", token);
                }
            }).start();
        } catch(Exception e) {
            Log.e(TAG, Log.getStackTraceString(e));
        }
    }

    /**
     * Send a notification to system tray with a title, a message and some arguments.
     * https://developer.android.com/guide/topics/ui/notifiers/notifications.html
     * @param String title the notification title
     * @param String message the notification message body. Can be a large text show a big notification
     * @param String sender the name of sender to not group messages by same origin. Can be null or empty string
     * @param String messageData a string with some json or string to send to QtApplication when user click in the notification, read as Intent extraData.
     * @return void
     */
    public void showNotification(String title, String message, String messageData)
    {
        new SystemNotification(context, CustomActivity.class).notify(title, message, messageData);
    }
}
