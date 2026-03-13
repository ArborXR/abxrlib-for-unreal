package com.abxr.lib;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;

public class AbxrAndroidPackageUtils {
    public static boolean isPackageInstalled(String packageName) {
        try {
            Activity activity = com.epicgames.unreal.GameActivity.Get();
            if (activity == null) {
                return false;
            }

            PackageManager pm = activity.getPackageManager();

            if (Build.VERSION.SDK_INT >= 33) {
                PackageInfo info = pm.getPackageInfo(
                    packageName,
                    PackageManager.PackageInfoFlags.of(0)
                );
                return info != null;
            } else {
                PackageInfo info = pm.getPackageInfo(packageName, 0);
                return info != null;
            }
        } catch (Exception e) {
            return false;
        }
    }
}