package com.abxr.qr;

/**
 * Holds the JNI native method declarations that Unreal registers via FAbxrQRService::RegisterNativeMethods().
 *
 * AbxrQRScanner calls these static helpers from the main thread after a scan
 * result is available; the C++ side then marshals to the game thread.
 */
public class AbxrQRScannerCallback {
    public static native void nativeOnScanned(String decodedText);
    public static native void nativeOnFailed(String reason);
    public static native void nativeOnCancelled();
}
