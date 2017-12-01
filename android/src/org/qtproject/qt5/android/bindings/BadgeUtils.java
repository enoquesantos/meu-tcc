package org.qtproject.qt5.android.bindings;

import java.lang.Thread;
import java.util.List;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.SharedPreferences;

public class BadgeUtils
{
    /**
     * Clear notification badge for lauch icon
     * Available only for Samsung, LG and Sony devices!
     * @param Context context the application context instance
     * @return void
     */
    public static void resetBadge(final Context c)
    {
        new Thread(new Runnable() {
            @Override
            public void run()
            {
                try  {
                    SharedPreferences sharedPreferences = c.getSharedPreferences(c.getPackageName(), Context.MODE_PRIVATE);
                    SharedPreferences.Editor prefsEditor = sharedPreferences.edit();
                    prefsEditor.putInt("badgeNumber", 0);
                    prefsEditor.apply();
                    setBadgeSamsung(c, 0);
                    clearBadgeSony(c);
                } catch (Exception e) {
                    // Log.e(TAG, Log.getStackTraceString(e));
                }
            }
        }).start();
    }

    public static void incrementBadge(final Context c)
    {
        new java.lang.Thread(new Runnable() {
            @Override
            public void run()
            {
                SharedPreferences sp = c.getSharedPreferences(c.getPackageName(), Context.MODE_PRIVATE);
                int badgeNum = sp.getInt("badgeNumber", 0) + 1;
                SharedPreferences.Editor prefsEditor = sp.edit();
                prefsEditor.putInt("badgeNumber", badgeNum);
                prefsEditor.apply();

                try  {
                    setBadgeSamsung(c, badgeNum);
                    setBadgeSony(c, badgeNum);
                } catch (Exception e) {
                    // android.util.Log.e("BadgeUtils", e);
                }
            }
        }).start();
    }

    private static void setBadgeSamsung(Context context, int count)
    {
        String launcherClassName = getLauncherClassName(context);
        if (launcherClassName == null)
            return;
        Intent intent = new Intent("android.intent.action.BADGE_COUNT_UPDATE");
        intent.putExtra("badge_count", count);
        intent.putExtra("badge_count_package_name", context.getPackageName());
        intent.putExtra("badge_count_class_name", launcherClassName);
        context.sendBroadcast(intent);
    }

    private static void setBadgeSony(Context context, int count)
    {
        String launcherClassName = getLauncherClassName(context);
        if (launcherClassName == null)
            return;
        Intent intent = new Intent();
        intent.setAction("com.sonyericsson.home.action.UPDATE_BADGE");
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.ACTIVITY_NAME", launcherClassName);
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.SHOW_MESSAGE", true);
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.MESSAGE", String.valueOf(count));
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.PACKAGE_NAME", context.getPackageName());
        context.sendBroadcast(intent);
    }

    private static void clearBadgeSony(Context context)
    {
        String launcherClassName = getLauncherClassName(context);
        if (launcherClassName == null)
            return;
        Intent intent = new Intent();
        intent.setAction("com.sonyericsson.home.action.UPDATE_BADGE");
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.ACTIVITY_NAME", launcherClassName);
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.SHOW_MESSAGE", false);
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.MESSAGE", String.valueOf(0));
        intent.putExtra("com.sonyericsson.home.intent.extra.badge.PACKAGE_NAME", context.getPackageName());
        context.sendBroadcast(intent);
    }

    private static String getLauncherClassName(Context context)
    {
        PackageManager pm = context.getPackageManager();
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        List<ResolveInfo> resolveInfos = pm.queryIntentActivities(intent, 0);
        for (ResolveInfo resolveInfo : resolveInfos) {
            String pkgName = resolveInfo.activityInfo.applicationInfo.packageName;
            if (pkgName.equalsIgnoreCase(context.getPackageName())) {
                String className = resolveInfo.activityInfo.name;
                return className;
            }
        }
        return null;
    }
}
