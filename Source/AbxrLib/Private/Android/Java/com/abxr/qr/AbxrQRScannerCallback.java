package com.abxr.qr;

public class AbxrQRScannerCallback {
    public static native void nativeOnScanned(String decodedText);
    public static native void nativeOnFailed(String reason);
    public static native void nativeOnCancelled();
    public static native void nativeOnPreviewFrame(byte[] rgba, int width, int height);
}
