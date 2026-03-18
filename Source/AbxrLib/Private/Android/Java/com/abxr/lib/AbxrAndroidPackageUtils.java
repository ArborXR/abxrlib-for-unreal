package com.abxr.lib;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;

public class AbxrAndroidPackageUtils {
    @SuppressWarnings("deprecation")
    public static boolean isPackageInstalled(String packageName) {
        try {
            Activity activity = com.epicgames.unreal.GameActivity.Get();
            if (activity == null) return false;

            PackageManager pm = activity.getPackageManager();
            PackageInfo info = pm.getPackageInfo(packageName, 0);
            return info != null;
        } catch (Exception e) {
            return false;
        }
    }
}
