#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "esUtils.h"
#include "MyLog.h"
#define LOGI(...) MyLog::logi(__VA_ARGS__)
#define LOGD(...) MyLog::logd(__VA_ARGS__)
#define LOGE(...) MyLog::loge(__VA_ARGS__)
#ifdef ANDROID
#include <android/log.h>
//#include <android_native_app_glue.h>
//#include <android/asset_manager.h>
//typedef AAsset esFile;
#else
typedef FILE esFile;
#endif

EGLint GetContextRenderableType ( EGLDisplay eglDisplay )
{
#ifdef EGL_KHR_create_context
   const char *extensions = eglQueryString ( eglDisplay, EGL_EXTENSIONS );

   // check whether EGL_KHR_create_context is in the extension string
   if ( extensions != NULL && strstr( extensions, "EGL_KHR_create_context" ) )
   {
      // extension is supported
      return EGL_OPENGL_ES3_BIT_KHR;
   }
#endif
   // extension is not supported
   return EGL_OPENGL_ES2_BIT;
}

GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags )
{   
    //获取EGL版本
    EGLConfig eglConfig;
    EGLint major, minor;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};

    if(esContext == NULL){
        return GL_FALSE;
    }

    //初始化escontext成员变量
    esContext->width = ANativeWindow_getWidth(esContext->eglNativeWindow);
    esContext->height = ANativeWindow_getHeight(esContext->eglNativeWindow);

    //获取egldisplay
    esContext->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(esContext->eglDisplay == EGL_NO_DISPLAY){
        return GL_FALSE;
    }
    
    //初始化egl
    if(!eglInitialize(esContext->eglDisplay, &major,&minor)){
        return GL_FALSE;
    }

    //获取eglConfig
    EGLint numConfigs = 0;
    EGLint attribList[] =
    {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     ( flags & ES_WINDOW_ALPHA ) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE,     ( flags & ES_WINDOW_DEPTH ) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE,   ( flags & ES_WINDOW_STENCIL ) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, ( flags & ES_WINDOW_MULTISAMPLE ) ? 1 : 0,
        // if EGL_KHR_create_context extension is supported, then we will use
        // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
        EGL_RENDERABLE_TYPE, GetContextRenderableType ( esContext->eglDisplay ),
        EGL_NONE
    };
    if(!eglChooseConfig(esContext->eglDisplay, attribList, &eglConfig, 1, &numConfigs)){
        return GL_FALSE;
    }
    
    {
        // For Android, need to get the EGL_NATIVE_VISUAL_ID and set it using ANativeWindow_setBuffersGeometry
        EGLint format = 0;
        eglGetConfigAttrib(esContext->eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(esContext->eglNativeWindow, 0, 0, format);
    }

    esContext->eglSurface = eglCreateWindowSurface(esContext->eglDisplay, eglConfig, esContext->eglNativeWindow, NULL);
    if(esContext->eglSurface == EGL_NO_SURFACE){
        return GL_FALSE;
    }

    esContext->eglContext = eglCreateContext(esContext->eglDisplay, eglConfig, NULL, contextAttribs);
    if(esContext->eglContext == EGL_NO_CONTEXT){
        return GL_FALSE;
    }

    if(!eglMakeCurrent(esContext->eglDisplay, esContext->eglSurface, esContext->eglSurface, esContext->eglContext)){
        return GL_FALSE;
    }

    LOGE("EGL配置成功: display=%p, surface=%p, context=%p, width=%d, height=%d", 
         esContext->eglDisplay, esContext->eglSurface, esContext->eglContext,
         esContext->width, esContext->height);

    return GL_TRUE;
}

///
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void ( ESCALLBACK *drawFunc ) ( ESContext * ) )
{
   esContext->drawFunc = drawFunc;
}

///
//  esRegisterShutdownFunc()
//
void ESUTIL_API esRegisterShutdownFunc ( ESContext *esContext, void ( ESCALLBACK *shutdownFunc ) ( ESContext * ) )
{
   esContext->shutdownFunc = shutdownFunc;
}

///
//  esRegisterUpdateFunc()
//
void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void ( ESCALLBACK *updateFunc ) ( ESContext *, float ) )
{
   esContext->updateFunc = updateFunc;
}

