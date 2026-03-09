package com.abxr.qr;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Size;
import android.view.Gravity;
import android.widget.Button;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.Preview;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.camera.view.PreviewView;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.LifecycleRegistry;

import com.google.common.util.concurrent.ListenableFuture;
import com.google.zxing.BinaryBitmap;
import com.google.zxing.MultiFormatReader;
import com.google.zxing.multi.GenericMultipleBarcodeReader;
import com.google.zxing.NotFoundException;
import com.google.zxing.PlanarYUVLuminanceSource;
import com.google.zxing.Result;
import com.google.zxing.common.HybridBinarizer;

import java.nio.ByteBuffer;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Full-screen Activity that shows a CameraX preview and decodes QR codes using ZXing.
 * On success / cancel / error it calls back into AbxrQRScanner's
 * static helpers, which forward to the native C++ callbacks.
 *
 * Dependencies to add to your project's build.gradle (or the gradle additions
 * block in AbxrLib_UPL.xml):
 *
 *   implementation "androidx.camera:camera-camera2:1.3.0"
 *   implementation "androidx.camera:camera-lifecycle:1.3.0"
 *   implementation "androidx.camera:camera-view:1.3.0"
 *   implementation "com.google.zxing:core:3.5.2"
 *   implementation "com.google.guava:guava:31.1-android"
 *   implementation "androidx.core:core:1.10.1"
 */
public class AbxrQRScanActivity extends Activity implements LifecycleOwner {

    private ExecutorService mCameraExecutor;
    private volatile boolean mResultDelivered = false;
    private final LifecycleRegistry mLifecycleRegistry = new LifecycleRegistry(this);
    private int mFrameCount = 0;
    private static final int DECODE_INTERVAL = 10; // decode every 10th frame
    
    // Static reference so C++ can call back into the active scan
    private static AbxrQRScanActivity sActiveActivity = null;

    // ------------------------------------------------------------------
    // Activity lifecycle
    // ------------------------------------------------------------------

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        sActiveActivity = this;

        mCameraExecutor = Executors.newSingleThreadExecutor();

        // Build UI programmatically — no XML layout needed
        FrameLayout root = new FrameLayout(this);

        PreviewView previewView = new PreviewView(this);
        previewView.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));
        root.addView(previewView);

        Button cancelButton = new Button(this);
        cancelButton.setText("Cancel");
        FrameLayout.LayoutParams btnParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        btnParams.gravity = Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
        btnParams.bottomMargin = 64;
        cancelButton.setLayoutParams(btnParams);
        cancelButton.setOnClickListener(v -> cancelScan());
        root.addView(cancelButton);

        setContentView(root);

        ListenableFuture<ProcessCameraProvider> future = ProcessCameraProvider.getInstance(this);
        future.addListener(() -> {
            try {
                bindCamera(future.get(), previewView);
            } catch (Exception e) {
                deliverError("Failed to start camera: " + e.getMessage());
            }
        }, ContextCompat.getMainExecutor(this));
    }

    @Override
    protected void onStart()  { super.onStart();  mLifecycleRegistry.handleLifecycleEvent(Lifecycle.Event.ON_START);  }
    @Override
    protected void onResume() { super.onResume(); mLifecycleRegistry.handleLifecycleEvent(Lifecycle.Event.ON_RESUME); }
    @Override
    protected void onPause()  { mLifecycleRegistry.handleLifecycleEvent(Lifecycle.Event.ON_PAUSE);  super.onPause();  }
    @Override
    protected void onStop()   { mLifecycleRegistry.handleLifecycleEvent(Lifecycle.Event.ON_STOP);   super.onStop();   }

    @Override
    protected void onDestroy() {
        if (sActiveActivity == this) sActiveActivity = null;
        super.onDestroy();
        if (mCameraExecutor != null) mCameraExecutor.shutdown();
    }

    @Override
    public void onBackPressed() { cancelScan(); }

    @NonNull
    @Override
    public Lifecycle getLifecycle() { return mLifecycleRegistry; }

    // ------------------------------------------------------------------
    // Camera binding
    // ------------------------------------------------------------------

    private void bindCamera(ProcessCameraProvider cameraProvider, PreviewView previewView) {
        Preview preview = new Preview.Builder().build();
        preview.setSurfaceProvider(previewView.getSurfaceProvider());

        ImageAnalysis imageAnalysis = new ImageAnalysis.Builder()
                .setTargetResolution(new Size(1280, 720))
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build();

        imageAnalysis.setAnalyzer(mCameraExecutor, imageProxy -> {
            if (mResultDelivered) { imageProxy.close(); return; }
            
            if (++mFrameCount % DECODE_INTERVAL != 0) { imageProxy.close(); return; }

            @SuppressWarnings("UnsafeOptInUsageError")
            android.media.Image mediaImage = imageProxy.getImage();
            if (mediaImage == null) { imageProxy.close(); return; }

            ByteBuffer yBuffer = mediaImage.getPlanes()[0].getBuffer();
            int width  = mediaImage.getWidth();
            int height = mediaImage.getHeight();
            byte[] yBytes = new byte[yBuffer.remaining()];
            yBuffer.get(yBytes);

            try {
                PlanarYUVLuminanceSource source = new PlanarYUVLuminanceSource(
                        yBytes, width, height, 0, 0, width, height, false);
                BinaryBitmap bitmap = new BinaryBitmap(new HybridBinarizer(source));
                GenericMultipleBarcodeReader multiReader = new GenericMultipleBarcodeReader(new MultiFormatReader());
                Result[] results = multiReader.decodeMultiple(bitmap);
                if (results != null) {
                    for (Result qrResult : results) {
                        deliverResult(qrResult.getText());
                    }
                }
            } catch (NotFoundException ignored) {
                // No QR code in this frame — keep scanning
            } catch (Exception e) {
                deliverError("Decode error: " + e.getMessage());
            } finally {
                imageProxy.close();
            }
        });

        cameraProvider.unbindAll();
        cameraProvider.bindToLifecycle(this, CameraSelector.DEFAULT_BACK_CAMERA, preview, imageAnalysis);
    }

    public static void acceptResult() {
        if (sActiveActivity != null) {
            sActiveActivity.runOnUiThread(() -> sActiveActivity.finish());
        }
    }
    
    public static void rejectResult() {
        if (sActiveActivity != null) {
            sActiveActivity.mResultDelivered = false;
        }
    }

    private void deliverResult(String text) {
        if (mResultDelivered) return;
        mResultDelivered = true;
        runOnUiThread(() -> AbxrQRScanner.onScanResult(text));
    }

    private void deliverError(String reason) {
        if (mResultDelivered) return;
        mResultDelivered = true;
        runOnUiThread(() -> { AbxrQRScanner.onScanError(reason); finish(); });
    }

    private void cancelScan() {
        if (mResultDelivered) return;
        mResultDelivered = true;
        AbxrQRScanner.onScanCancelled();
        finish();
    }
}
