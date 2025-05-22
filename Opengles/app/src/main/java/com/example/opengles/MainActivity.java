package com.example.opengles;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Context;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Choreographer;
import android.util.Log;

import com.example.opengles.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    // Used to load the 'opengles' library on application startup.
    static {
        System.loadLibrary("opengles");
    }

    private ActivityMainBinding binding;
    private SurfaceView surfaceView;
    private long esContextPtr;
    private boolean renderingActive = false;
    private final Runnable renderRunnable = new Runnable() {
        @Override
        public void run() {
            if (renderingActive && esContextPtr != 0) {
                // 添加日志
                Log.e("Renderer", "Drawing frame");
                // 调用原生方法绘制一帧
                draw(esContextPtr);
                // 安排下一帧渲染
                Choreographer.getInstance().postFrameCallback(frameCallback);
            }
        }
    };

    private final Choreographer.FrameCallback frameCallback = new Choreographer.FrameCallback() {
        @Override
        public void doFrame(long frameTimeNanos) {
            renderRunnable.run();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 获取SurfaceView并设置回调
        surfaceView = binding.surfaceView;
        surfaceView.getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        // Surface创建时初始化ESContext
        esContextPtr = initContext();
        if (esContextPtr != 0) {
            // 将Surface传递给原生代码
            setSurface(esContextPtr, holder.getSurface());
            // 设置窗口尺寸
            setWindowSize(esContextPtr, surfaceView.getWidth(), surfaceView.getHeight());
            // 初始化OpenGL
            esMain(esContextPtr);

            draw(esContextPtr);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // Surface尺寸变化时更新
        if (esContextPtr != 0) {
            setWindowSize(esContextPtr, width, height);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // Surface销毁时清理资源
        if (esContextPtr != 0) {
            shutdown(esContextPtr);
            destroyContext(esContextPtr);
            esContextPtr = 0;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        // 开始渲染循环
        renderingActive = true;
        if (esContextPtr != 0) {
            Choreographer.getInstance().postFrameCallback(frameCallback);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        // 停止渲染循环
        renderingActive = false;
    }

    /**
     * A native method that is implemented by the 'opengles' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native long initContext();
    public native void destroyContext(long esContextPtr);
    public native int setSurface(long esContextPtr, Surface surface);
    public native int setWindowSize(long esContextPtr, int width, int height);
    public native int esMain(long esContextPtr);
    public native int initOpengl(long esContextPtr);
    public native void draw(long esContextPtr);
    public native void shutdown(long esContextPtr);
}