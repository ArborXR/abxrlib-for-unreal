package com.xrdm.xrdmbridge;

import app.xrdm.sdk.external.IConnectionCallback;
import app.xrdm.sdk.external.ISdkServiceSafe;

public class NativeConnectionCallback implements IConnectionCallback {
    @Override
    public void onConnected(ISdkServiceSafe service) {
        nativeOnConnected(service);
    }

    @Override
    public void onDisconnected(boolean wasClean) {
        nativeOnDisconnected(wasClean);
    }

    // JNI entrypoints Unreal will provide
    private static native void nativeOnConnected(Object service);
    private static native void nativeOnDisconnected(boolean wasClean);
}
