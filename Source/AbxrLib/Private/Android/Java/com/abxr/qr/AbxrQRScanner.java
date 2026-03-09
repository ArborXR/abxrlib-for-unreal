package com.abxr.qr;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Looper;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

/**
 * Requests CAMERA permission at runtime (required Android 6+), then launches AbxrQRScanActivity
 * which handles the CameraX preview and ZXing decode.
 * Results funnel back through AbxrQRScannerCallback's static native methods.
 */
public class AbxrQRScanner {

    private static final int CAMERA_PERMISSION_REQUEST_CODE = 0x4252; // "BR"
    private static final int QR_SCAN_ACTIVITY_REQUEST_CODE  = 0x5152; // "QR"

    private static AbxrQRScanner sInstance = null;
    private Activity mActivity = null;
    private boolean  mIsScanning = false;
    private final Handler mMainHandler = new Handler(Looper.getMainLooper());

    public void startScan(Activity activity) {
        if (mIsScanning) return;

        mActivity = activity;
        mIsScanning = true;
        sInstance = this;

        if (hasCameraPermission()) {
            launchScanActivity();
        } else {
            requestCameraPermission();
        }
    }

    public void stopScan() {
        mIsScanning = false;
    }

    public void release() {
        mIsScanning = false;
        mActivity = null;
    }

    public static void onScanResult(String decodedText) {
        AbxrQRScannerCallback.nativeOnScanned(decodedText);
    }

    public static void onScanError(String reason) {
        if (sInstance != null) sInstance.mIsScanning = false;
        AbxrQRScannerCallback.nativeOnFailed(reason);
    }

    public static void onScanCancelled() {
        if (sInstance != null) sInstance.mIsScanning = false;
        AbxrQRScannerCallback.nativeOnCancelled();
    }
    
    public static void acceptScanResult() {
        if (sInstance != null) sInstance.mIsScanning = false;
        AbxrQRScanActivity.acceptResult();
    }
    
    public static void rejectScanResult() {
        AbxrQRScanActivity.rejectResult();
    }

    // ------------------------------------------------------------------
    // Internals
    // ------------------------------------------------------------------
    private boolean hasCameraPermission() {
        return ContextCompat.checkSelfPermission(
                mActivity, android.Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestCameraPermission() {
        ActivityCompat.requestPermissions(
                mActivity,
                new String[]{ android.Manifest.permission.CAMERA },
                CAMERA_PERMISSION_REQUEST_CODE
        );
        // Unreal's GameActivity doesn't forward onRequestPermissionsResult to
        // plugins by default, so poll the main thread until the user responds.
        pollForPermission(0);
    }

    // Poll up to ~5 s (50 × 100 ms) waiting for the permission dialog
    private void pollForPermission(final int attempt) {
        if (!mIsScanning) return;

        if (hasCameraPermission()) {
            launchScanActivity();
            return;
        }

        if (attempt >= 50) {
            mIsScanning = false;
            AbxrQRScannerCallback.nativeOnFailed("Camera permission denied");
            return;
        }

        mMainHandler.postDelayed(() -> pollForPermission(attempt + 1), 100);
    }

    private void launchScanActivity() {
        if (mActivity == null || !mIsScanning) return;
        Intent intent = new Intent(mActivity, AbxrQRScanActivity.class);
        mActivity.startActivityForResult(intent, QR_SCAN_ACTIVITY_REQUEST_CODE);
    }
}
