#ifndef OPENGL_ANDROID_MYLOG_H
#define OPENGL_ANDROID_MYLOG_H
#include <android/log.h>
#define TAG "NativeLog"
class MyLog {
public:
    static void logv(const char* format, ...) {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_VERBOSE, TAG, format, args);
        va_end(args);
    }
    static void logd(const char* format, ...) {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_DEBUG, TAG, format, args);
        va_end(args);
    }
    static void logi(const char* format, ...) {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_INFO, TAG, format, args);
        va_end(args);
    }
    static void logw(const char* format, ...) {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_WARN, TAG, format, args);
        va_end(args);
    }
    static void loge(const char* format, ...) {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_ERROR, TAG, format, args);
        va_end(args);
    }
};
#endif //OPENGL_ANDROID_MYLOG_H
