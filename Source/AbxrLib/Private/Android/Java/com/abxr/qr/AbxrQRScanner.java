package com.abxr.qr;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Looper;
import android.os.SystemClock;
import android.util.Size;

import androidx.annotation.NonNull;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.lifecycle.ProcessCameraProvider;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.LifecycleRegistry;

import com.google.common.util.concurrent.ListenableFuture;
import com.google.zxing.BarcodeFormat;
import com.google.zxing.BinaryBitmap;
import com.google.zxing.DecodeHintType;
import com.google.zxing.MultiFormatReader;
import com.google.zxing.NotFoundException;
import com.google.zxing.PlanarYUVLuminanceSource;
import com.google.zxing.Result;
import com.google.zxing.common.HybridBinarizer;
import com.google.zxing.multi.GenericMultipleBarcodeReader;

import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.EnumMap;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AbxrQRScanner {
    private Activity activity;
    private boolean isScanning = false;

    private ExecutorService cameraExecutor;
    private ProcessCameraProvider cameraProvider;
    private ScannerLifecycleOwner lifecycleOwner;

    private byte[] previewRgba;
    private byte[] luma;
    private long lastPreviewMs = 0;
    private long lastDecodeMs = 0;

    public void startScan(Activity activity) {
        if (isScanning) return;

        this.activity = activity;

        if (activity == null) {
            AbxrQRScannerCallback.nativeOnFailed("No activity");
            return;
        }

        activity.runOnUiThread(() -> {
            try {
                isScanning = true;
                lastPreviewMs = 0;
                lastDecodeMs = 0;
                startHeadlessCamera();
            } catch (Exception e) {
                isScanning = false;
                AbxrQRScannerCallback.nativeOnFailed("startScan exception: " + e);
            }
        });
    }

    public void stopScan() {
        isScanning = false;

        if (activity != null) {
            activity.runOnUiThread(this::stopHeadlessCamera);
        } else {
            stopHeadlessCamera();
        }
    }

    private void startHeadlessCamera() {
        if (activity == null) {
            isScanning = false;
            AbxrQRScannerCallback.nativeOnFailed("No activity");
            return;
        }

        Runnable startRunnable = () -> {
            try {
                if (cameraExecutor == null || cameraExecutor.isShutdown()) {
                    cameraExecutor = Executors.newSingleThreadExecutor();
                }

                if (lifecycleOwner == null) {
                    lifecycleOwner = new ScannerLifecycleOwner();
                }
                lifecycleOwner.startAndResume();

                ListenableFuture<ProcessCameraProvider> future = ProcessCameraProvider.getInstance(activity);
                future.addListener(() -> {
                    try {
                        cameraProvider = future.get();
                        bindCamera();
                    } catch (Exception e) {
                        isScanning = false;
                        AbxrQRScannerCallback.nativeOnFailed("Failed to start camera: " + e);
                    }
                }, ContextCompat.getMainExecutor(activity));
            } catch (Exception e) {
                isScanning = false;
                AbxrQRScannerCallback.nativeOnFailed("startHeadlessCamera failed: " + e);
            }
        };

        if (Looper.myLooper() == Looper.getMainLooper()) {
            startRunnable.run();
        } else {
            activity.runOnUiThread(startRunnable);
        }
    }

    private void stopHeadlessCamera() {
        Runnable stopRunnable = () -> {
            if (cameraProvider != null) {
                cameraProvider.unbindAll();
                cameraProvider = null;
            }

            if (lifecycleOwner != null) {
                lifecycleOwner.stopAndDestroy();
                lifecycleOwner = null;
            }

            if (cameraExecutor != null) {
                cameraExecutor.shutdown();
                cameraExecutor = null;
            }
        };

        if (activity != null && Looper.myLooper() != Looper.getMainLooper()) {
            activity.runOnUiThread(stopRunnable);
        } else {
            stopRunnable.run();
        }
    }

    private void bindCamera() {
        if (cameraProvider == null) {
            isScanning = false;
            AbxrQRScannerCallback.nativeOnFailed("Camera provider unavailable");
            return;
        }

        ImageAnalysis imageAnalysis = new ImageAnalysis.Builder()
                .setOutputImageFormat(ImageAnalysis.OUTPUT_IMAGE_FORMAT_RGBA_8888)
                .setTargetResolution(new Size(640, 480))
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build();

        final MultiFormatReader baseReader = new MultiFormatReader();
        final Map<DecodeHintType, Object> hints = new EnumMap<>(DecodeHintType.class);
        hints.put(DecodeHintType.POSSIBLE_FORMATS, Collections.singletonList(BarcodeFormat.QR_CODE));
        hints.put(DecodeHintType.TRY_HARDER, Boolean.TRUE);
        baseReader.setHints(hints);

        imageAnalysis.setAnalyzer(cameraExecutor, imageProxy -> {
            try {
                if (!isScanning) return;

                ByteBuffer buffer = imageProxy.getPlanes()[0].getBuffer();
                int width = imageProxy.getWidth();
                int height = imageProxy.getHeight();
                int rowStride = imageProxy.getPlanes()[0].getRowStride();

                int tightSize = width * height * 4;
                if (previewRgba == null || previewRgba.length != tightSize) {
                    previewRgba = new byte[tightSize];
                }

                copyTightRgba(buffer, rowStride, width, height, previewRgba);

                long now = SystemClock.uptimeMillis();
                if (now - lastPreviewMs >= 33) {
                    lastPreviewMs = now;
                    AbxrQRScannerCallback.nativeOnPreviewFrame(previewRgba, width, height);
                }

                if (now - lastDecodeMs >= 500) {
                    lastDecodeMs = now;

                    int lumaSize = width * height;
                    if (luma == null || luma.length != lumaSize) {
                        luma = new byte[lumaSize];
                    }

                    rgbaToLuma(previewRgba, width, height, luma);

                    try {
                        PlanarYUVLuminanceSource source = new PlanarYUVLuminanceSource(
                                luma, width, height, 0, 0, width, height, false);
                        BinaryBitmap bitmap = new BinaryBitmap(new HybridBinarizer(source));
                        GenericMultipleBarcodeReader multiReader = new GenericMultipleBarcodeReader(baseReader);
                        Result[] results = multiReader.decodeMultiple(bitmap);

                        if (results != null) {
                            for (Result result : results) {
                                if (result == null) continue;

                                String text = result.getText();
                                if (text != null && text.startsWith("ABXR:")) {
                                    String payload = text.substring("ABXR:".length()).trim();
                                    isScanning = false;
                                    AbxrQRScannerCallback.nativeOnScanned(payload);
                                    return;
                                }
                            }
                        }
                    } catch (NotFoundException ignored) {
                    } finally {
                        baseReader.reset();
                    }
                }
            } catch (Exception e) {
                isScanning = false;
                AbxrQRScannerCallback.nativeOnFailed("Decode error: " + e.getMessage());
            } finally {
                imageProxy.close();
            }
        });

        cameraProvider.unbindAll();
        cameraProvider.bindToLifecycle(
                lifecycleOwner,
                CameraSelector.DEFAULT_BACK_CAMERA,
                imageAnalysis);
    }

    private static void copyTightRgba(ByteBuffer src, int rowStride, int width, int height, byte[] out) {
        src.rewind();
        for (int y = 0; y < height; y++) {
            src.position(y * rowStride);
            src.get(out, y * width * 4, width * 4);
        }
    }

    private static void rgbaToLuma(byte[] rgba, int width, int height, byte[] outLuma) {
        int pixelCount = width * height;
        for (int i = 0; i < pixelCount; i++) {
            int base = i * 4;
            int r = rgba[base + 1] & 0xFF;
            int g = rgba[base + 2] & 0xFF;
            int b = rgba[base + 3] & 0xFF;
            outLuma[i] = (byte) ((77 * r + 150 * g + 29 * b) >> 8);
        }
    }

    private static class ScannerLifecycleOwner implements LifecycleOwner {
        private final LifecycleRegistry registry = new LifecycleRegistry(this);

        ScannerLifecycleOwner() {
            registry.setCurrentState(Lifecycle.State.CREATED);
        }

        void startAndResume() {
            registry.handleLifecycleEvent(Lifecycle.Event.ON_START);
            registry.handleLifecycleEvent(Lifecycle.Event.ON_RESUME);
        }

        void stopAndDestroy() {
            registry.handleLifecycleEvent(Lifecycle.Event.ON_PAUSE);
            registry.handleLifecycleEvent(Lifecycle.Event.ON_STOP);
            registry.handleLifecycleEvent(Lifecycle.Event.ON_DESTROY);
        }

        @NonNull
        @Override
        public Lifecycle getLifecycle() {
            return registry;
        }
    }
}
