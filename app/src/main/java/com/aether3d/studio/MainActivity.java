package com.aether3d.studio;



import android.app.Activity;

import android.content.res.AssetManager;

import android.graphics.Color;

import android.net.Uri;

import android.os.Build;

import android.os.Bundle;

import android.os.Handler;

import android.os.Looper;

import android.util.Log;

import android.view.View;

import android.view.Window;

import android.view.WindowInsets;

import android.view.WindowInsetsController;

import android.webkit.JavascriptInterface;

import android.webkit.WebChromeClient;

import android.webkit.WebResourceError;

import android.webkit.WebResourceRequest;

import android.webkit.WebResourceResponse;

import android.webkit.WebSettings;

import android.webkit.WebView;

import android.webkit.WebViewClient;

import android.widget.FrameLayout;

import android.widget.Toast;



import androidx.webkit.WebViewAssetLoader;



import org.json.JSONArray;

import org.json.JSONException;

import org.json.JSONObject;



import java.io.IOException;

import java.io.InputStream;

import java.util.Locale;



public final class MainActivity extends Activity {



    private static final String TAG =
        "Aether3D.MainActivity";



    private static final String LOCAL_SCHEME =
        "https";



    private static final String LOCAL_HOST =
        "androidplatform.net";



    private static final String LOCAL_ENTRY_POINT =
        "https://androidplatform.net/index.html";



    private static final String JAVASCRIPT_BRIDGE_NAME =
        "NativePhysicsBridge";



    private static final boolean NATIVE_LIBRARY_LOADED;



    static {

        boolean loaded = false;



        try {

            System.loadLibrary(
                "aether3d_physics"
            );

            loaded = true;

        } catch (
            UnsatisfiedLinkError error
        ) {

            Log.e(
                TAG,
                "Native physics library could not be loaded.",
                error
            );

        }



        NATIVE_LIBRARY_LOADED =
            loaded;

    }



    private final Handler mainHandler =
        new Handler(
            Looper.getMainLooper()
        );



    private FrameLayout rootLayout;

    private WebView webView;

    private WebViewAssetLoader assetLoader;



    @Override
    protected void onCreate(
        Bundle savedInstanceState
    ) {

        super.onCreate(
            savedInstanceState
        );



        requestLandscapeImmersiveWindow();



        rootLayout =
            new FrameLayout(
                this
            );



        rootLayout.setBackgroundColor(
            Color.rgb(
                7,
                9,
                13
            )
        );



        setContentView(
            rootLayout
        );



        assetLoader =
            new WebViewAssetLoader.Builder()

                .setDomain(
                    LOCAL_HOST
                )

                .addPathHandler(
                    "/assets/",
                    new WebViewAssetLoader.AssetsPathHandler(
                        this
                    )
                )

                .build();



        createAndLoadWebView();

    }



    private void requestLandscapeImmersiveWindow() {

        getRequestedOrientation();



        if (Build.VERSION.SDK_INT >= 30) {

            Window window =
                getWindow();



            WindowInsetsController controller =
                window.getInsetsController();



            if (controller != null) {

                controller.hide(
                    WindowInsets.Type.statusBars()
                        |
                    WindowInsets.Type.navigationBars()
                );



                controller.setSystemBarsBehavior(
                    WindowInsetsController
                        .BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
                );

            }

        } else {

            getWindow()
                .getDecorView()
                .setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    |
                    View.SYSTEM_UI_FLAG_FULLSCREEN
                    |
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    |
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    |
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    |
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                );

        }

    }



    private void createAndLoadWebView() {

        destroyWebViewOnly();



        webView =
            new WebView(
                this
            );



        configureWebView(
            webView
        );



        rootLayout.addView(
            webView,
            new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT
            )
        );



        try {

            webView.loadUrl(
                LOCAL_ENTRY_POINT
            );

        } catch (
            RuntimeException error
        ) {

            Log.e(
                TAG,
                "Unable to start the local WebView.",
                error
            );

            showFatalStartupMessage(
                error
            );

        }

    }



    private void configureWebView(
        WebView targetWebView
    ) {

        WebSettings settings =
            targetWebView.getSettings();



        settings.setJavaScriptEnabled(
            true
        );



        settings.setDomStorageEnabled(
            true
        );



        settings.setDatabaseEnabled(
            true
        );



        settings.setAllowFileAccess(
            false
        );



        settings.setAllowContentAccess(
            false
        );



        settings.setBuiltInZoomControls(
            false
        );



        settings.setDisplayZoomControls(
            false
        );



        settings.setSupportZoom(
            false
        );



        settings.setLoadWithOverviewMode(
            false
        );



        settings.setUseWideViewPort(
            false
        );



        settings.setMediaPlaybackRequiresUserGesture(
            false
        );



        settings.setCacheMode(
            WebSettings.LOAD_DEFAULT
        );



        settings.setMixedContentMode(
            WebSettings.MIXED_CONTENT_NEVER_ALLOW
        );



        settings.setOffscreenPreRaster(
            true
        );



        settings.setTextZoom(
            100
        );



        settings.setNeedInitialFocus(
            false
        );



        if (Build.VERSION.SDK_INT >= 26) {

            settings.setSafeBrowsingEnabled(
                true
            );

        }



        if (Build.VERSION.SDK_INT >= 29) {

            settings.setForceDark(
                WebSettings.FORCE_DARK_OFF
            );

        }



        targetWebView.setBackgroundColor(
            Color.rgb(
                7,
                9,
                13
            )
        );



        targetWebView.setVerticalScrollBarEnabled(
            false
        );



        targetWebView.setHorizontalScrollBarEnabled(
            false
        );



        targetWebView.setOverScrollMode(
            View.OVER_SCROLL_NEVER
        );



        targetWebView.setNetworkAvailable(
            true
        );



        targetWebView.setWebChromeClient(
            new WebChromeClient()
        );



        targetWebView.setWebViewClient(
            new LocalWebViewClient(
                assetLoader
            )
        );



        if (
            NATIVE_LIBRARY_LOADED
        ) {

            targetWebView.addJavascriptInterface(
                new NativePhysicsBridge(),
                JAVASCRIPT_BRIDGE_NAME
            );

        }

    }



    private boolean isLocalApplicationUrl(
        Uri uri
    ) {

        if (uri == null) {

            return false;

        }



        return LOCAL_SCHEME.equalsIgnoreCase(
            uri.getScheme()
        )
        &&
        LOCAL_HOST.equalsIgnoreCase(
            uri.getHost()
        );

    }



    private void showFatalStartupMessage(
        Throwable error
    ) {

        String message =
            "Aether3D could not initialize the native viewport.";

        Log.e(
            TAG,
            message,
            error
        );

        Toast.makeText(
            this,
            message,
            Toast.LENGTH_LONG
        ).show();

    }



    private void showRendererRecoveryMessage() {

        Toast.makeText(
            this,
            "Aether3D renderer restarted.",
            Toast.LENGTH_SHORT
        ).show();

    }



    private void destroyWebViewOnly() {

        if (webView == null) {

            return;

        }



        webView.removeJavascriptInterface(
            JAVASCRIPT_BRIDGE_NAME
        );



        if (rootLayout != null) {

            rootLayout.removeView(
                webView
            );

        }



        webView.stopLoading();

        webView.loadUrl(
            "about:blank"
        );

        webView.clearHistory();

        webView.clearCache(
            true
        );

        webView.destroy();



        webView = null;

    }



    @Override
    protected void onDestroy() {

        destroyWebViewOnly();



        super.onDestroy();

    }



    private final class LocalWebViewClient
        extends WebViewClient {



        private final WebViewAssetLoader loader;



        private LocalWebViewClient(
            WebViewAssetLoader loader
        ) {

            this.loader =
                loader;

        }



        @Override
        public WebResourceResponse shouldInterceptRequest(
            WebView view,
            WebResourceRequest request
        ) {

            Uri requestUri =
                request.getUrl();



            if (
                isLocalApplicationUrl(
                    requestUri
                )
            ) {

                return loader.shouldInterceptRequest(
                    requestUri
                );

            }



            return null;

        }



        @Override
        public boolean shouldOverrideUrlLoading(
            WebView view,
            WebResourceRequest request
        ) {

            Uri requestedUri =
                request.getUrl();



            if (
                isLocalApplicationUrl(
                    requestedUri
                )
            ) {

                return false;

            }



            return true;

        }



        @Override
        public boolean shouldOverrideUrlLoading(
            WebView view,
            String url
        ) {

            try {

                Uri requestedUri =
                    Uri.parse(
                        url
                    );



                return !isLocalApplicationUrl(
                    requestedUri
                );

            } catch (Exception error) {

                return true;

            }

        }



        @Override
        public void onReceivedError(
            WebView view,
            WebResourceRequest request,
            WebResourceError error
        ) {

            if (
                request.isForMainFrame()
            ) {

                Log.e(
                    TAG,
                    "WebView local asset load failed: "
                        +
                    error
                        .getDescription()
                );

            }

        }



        @Override
        public boolean onRenderProcessGone(
            WebView view,
            android.webkit.RenderProcessGoneDetail detail
        ) {

            Log.e(
                TAG,
                "WebView renderer process terminated. didCrash="
                    +
                detail.didCrash()
            );



            showRendererRecoveryMessage();



            mainHandler.postDelayed(
                new Runnable() {

                    @Override
                    public void run() {

                        if (!isFinishing()) {

                            createAndLoadWebView();

                        }

                    }

                },
                250L
            );



            return true;

        }

    }



    public final class NativePhysicsBridge {



        private long frameCounter =
            0L;



        @JavascriptInterface
        public synchronized String getEngineVersion() {

            if (
                !NATIVE_LIBRARY_LOADED
            ) {

                return "Native physics unavailable; WebView fallback active.";

            }



            return nativeGetEngineVersion();

        }



        @JavascriptInterface
        public synchronized String stepSimulation(
            String payload
        ) {

            if (
                !NATIVE_LIBRARY_LOADED
            ) {

                return errorResponse(
                    "Native physics library is unavailable."
                );

            }



            try {

                JSONObject request =
                    new JSONObject(
                        payload
                    );



                double requestedDelta =
                    request.optDouble(
                        "dt",
                        1.0 / 60.0
                    );



                float deltaSeconds =
                    (float) Math.max(
                        0.0001,
                        Math.min(
                            requestedDelta,
                            0.0333333
                        )
                    );



                float[] positions =
                    readVectorArray(
                        request.optJSONArray(
                            "positions"
                        )
                    );



                float[] velocities =
                    readVectorArray(
                        request.optJSONArray(
                            "velocities"
                        )
                    );



                float[] nativeFrame =
                    nativeStepSimulation(
                        positions,
                        velocities,
                        deltaSeconds
                    );



                if (
                    nativeFrame == null
                    ||
                    nativeFrame.length < 29
                ) {

                    return errorResponse(
                        "Native frame returned an invalid payload."
                    );

                }



                frameCounter++;



                JSONObject response =
                    new JSONObject();



                response.put(
                    "frame",
                    frameCounter
                );



                response.put(
                    "dt",
                    deltaSeconds
                );



                response.put(
                    "position",
                    vectorJson(
                        nativeFrame,
                        0
                    )
                );



                response.put(
                    "velocity",
                    vectorJson(
                        nativeFrame,
                        3
                    )
                );



                response.put(
                    "orientation",
                    quaternionJson(
                        nativeFrame,
                        6
                    )
                );



                response.put(
                    "angularVelocity",
                    vectorJson(
                        nativeFrame,
                        10
                    )
                );



                response.put(
                    "transformMatrix",
                    matrixJson(
                        nativeFrame,
                        13
                    )
                );



                response.put(
                    "telemetryLine",
                    String.format(
                        Locale.US,
                        "frame=%d dt=%.6f pos=(%.5f,%.5f,%.5f) vel=(%.5f,%.5f,%.5f)",
                        frameCounter,
                        deltaSeconds,
                        nativeFrame[0],
                        nativeFrame[1],
                        nativeFrame[2],
                        nativeFrame[3],
                        nativeFrame[4],
                        nativeFrame[5]
                    )
                );



                return response.toString();

            } catch (
                JSONException
                |
                IllegalArgumentException
                error
            ) {

                return errorResponse(
                    "Invalid physics bridge payload: "
                        +
                    error.getMessage()
                );

            }

        }



        @JavascriptInterface
        public synchronized void resetSimulation() {

            nativeResetSimulation();



            frameCounter =
                0L;

        }



        private float[] readVectorArray(
            JSONArray array
        )
            throws JSONException {

            if (
                array == null
                ||
                array.length() < 3
            ) {

                return new float[] {
                    0.0f,
                    0.0f,
                    0.0f
                };

            }



            float[] values =
                new float[3];



            for (
                int index = 0;
                index < 3;
                index++
            ) {

                values[index] =
                    (float) array.getDouble(
                        index
                    );

            }



            return values;

        }



        private JSONArray vectorJson(
            float[] values,
            int offset
        )
            throws JSONException {

            JSONArray result =
                new JSONArray();



            result.put(
                values[offset]
            );



            result.put(
                values[offset + 1]
            );



            result.put(
                values[offset + 2]
            );



            return result;

        }



        private JSONArray quaternionJson(
            float[] values,
            int offset
        )
            throws JSONException {

            JSONArray result =
                new JSONArray();



            result.put(
                values[offset]
            );



            result.put(
                values[offset + 1]
            );



            result.put(
                values[offset + 2]
            );



            result.put(
                values[offset + 3]
            );



            return result;

        }



        private JSONArray matrixJson(
            float[] values,
            int offset
        )
            throws JSONException {

            JSONArray result =
                new JSONArray();



            for (
                int index = 0;
                index < 16;
                index++
            ) {

                result.put(
                    values[offset + index]
                );

            }



            return result;

        }



        private String errorResponse(
            String message
        ) {

            JSONObject response =
                new JSONObject();



            try {

                response.put(
                    "error",
                    message
                );

            } catch (JSONException ignored) {

            }



            return response.toString();

        }

    }



    private static native String
        nativeGetEngineVersion();



    private static native float[]
        nativeStepSimulation(
            float[] positions,
            float[] velocities,
            float deltaSeconds
        );



    private static native void
        nativeResetSimulation();

}
