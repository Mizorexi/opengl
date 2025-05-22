#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "Triangle/triangle.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_opengles_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_opengles_MainActivity_initContext(JNIEnv* env, jobject thiz) {
    ESContext* esContext = new ESContext();
    if (!esContext) return 0;
    // 初始化ESContext结构体
    memset(esContext, 0, sizeof(ESContext));
    return reinterpret_cast<jlong>(esContext);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_opengles_MainActivity_destroyContext(JNIEnv* env, jobject thiz, jlong esContextPtr) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) return;
    
    // 如果有关联的ANativeWindow，释放它
    if (esContext->eglNativeWindow) {
        ANativeWindow_release(reinterpret_cast<ANativeWindow*>(esContext->eglNativeWindow));
        esContext->eglNativeWindow = nullptr;
    }
    
    delete esContext;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_opengles_MainActivity_setSurface(JNIEnv* env, jobject thiz, jlong esContextPtr, jobject surface) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) return 0;
    
    // 如果已经有关联的ANativeWindow，先释放它
    if (esContext->eglNativeWindow) {
        ANativeWindow_release(reinterpret_cast<ANativeWindow*>(esContext->eglNativeWindow));
    }
    
    // 从Java的Surface获取ANativeWindow
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (!window) return 0;
    
    // 保存到ESContext中
    esContext->eglNativeWindow = window;
    return 1;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_opengles_MainActivity_setWindowSize(JNIEnv* env, jobject thiz, jlong esContextPtr, jint width, jint height) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) return 0;
    
    // 设置窗口尺寸
    esContext->width = width;
    esContext->height = height;
    return 1;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_opengles_MainActivity_esMain(JNIEnv* env, jobject thiz, jlong esContextPtr) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) {
//        LOGE("esContext is null in esMain");
        return 0;
    }
    
//    LOGE("Calling esMain with window: %p, width: %d, height: %d",
//     esContext->eglNativeWindow, esContext->width, esContext->height);
    
    int result = esMain(esContext);
//    LOGE("esMain returned: %d", result);
    return result;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_opengles_MainActivity_initOpengl(JNIEnv* env, jobject thiz, jlong esContextPtr) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) return 0;
    // 假设esMain函数负责初始化OpenGL ES环境
    return esMain(esContext);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_opengles_MainActivity_draw(JNIEnv* env, jobject thiz, jlong esContextPtr) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) return;
    Draw(esContext);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_opengles_MainActivity_shutdown(JNIEnv* env, jobject thiz, jlong esContextPtr) {
    ESContext* esContext = reinterpret_cast<ESContext*>(esContextPtr);
    if (!esContext) return;
    Shutdown(esContext);
}