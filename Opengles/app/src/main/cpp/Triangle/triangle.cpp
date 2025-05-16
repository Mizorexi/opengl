#include "triangle.h"
#include "MyLog.h"

#define LOGI(...) MyLog::logi(__VA_ARGS__)
#define LOGD(...) MyLog::logd(__VA_ARGS__)
#define LOGE(...) MyLog::loge(__VA_ARGS__)


#define checkGlError(glFunc) \
    do { \
        GLenum err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            LOGE("GL error 0x%04X after %s() call", err, #glFunc); \
        } \
    } while (0)

Triangle::Triangle(){

}

Triangle::initOpengl(){
    //获取EGLDisplay
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display == EGL_NO_DISPLAY){
        return -1;
    }

    //获取EGL版本
    EGLint major, minor;
    eglInitialize(display, &major, &minor);
    LOGI("EGL version: %d.%d", major, minor);

    //初始化EGL
    EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3BIT_KHR,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 1,
        EGL_NONE
    }

    //获取EGLConfig
    const EGLint Max_configs = 2;
    EGLConfig configs[Max_configs];
    EGLint num_configs;
    eglChooseConfig(display, attribs, configs, Max_configs, &num_configs);
    checkGlError("eglChooseConfig");

    //创建EGLSurface
    EGLSurface window;
    window = eglCreateWindowSurface(display, configs[0], m_nativeWindow , NULL);
    checkGlError("eglCreateWindowSurface");
    if(surface == EGL_NO_SURFACE){
        return -1;
    }

    //创建上下文
    EGLint attrib_list[]={
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    }

    EGLContext context = eglCreateContext(display, configs[0], EGL_NO_CONTEXT, attrib_list);
    checkGlError("eglCreateContext");
    if(context == EGL_NO_CONTEXT){
        return -1;
    }
    
    eglMakeCurrent(display, window, window, context);
    checkGlError("eglMakeCurrent");

    return 0;
}

//加载着色器
int Triangle::loadShader(int type,const char **shadercode){
    int shader = glCreateShader(type);
    glShaderSource(shader, 1, shadercode, NULL);
    glCompileShader(shader);

    int *compileParams = new int[1];
    glGetShaderiv(shader,GL_COMPILE_STATUS,compileParams);
    if(compileParams[0] == GL_FALSE){
        glDeleteShader(shader);
        shader = 0;
    }
    delete[] compileParams;
    compileParams = nullptr;

    return shader;
}

//创建和链接程序
int Triangle::createProgram(const char *vShaderStr,const char *fShaderStr){
    int vShader = loadShaer(GL_VERTEX_SHADER, vertextSource);
    if(vShader == 0){
        return 0;
    }
    int fShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if(fShader == 0){
        return 0;
    }
    
    int program = glCreateProgram();
    if(program != 0){
        glAttachShader(program, vShader);
        checkGlError("glAttachShader");

        glAttachShader(program, fShader);
        checkGlError("glAttachShader");

        glLinkProgram(program);

        int *linkParams = new int[1];
        glGetProgramiv(program,GL_LINK_STATUS,linkParams);
        if(linkParams[0] == GL_FALSE){
            glDeleteProgram(program);
            program = 0;
        }
        delete[] linkParams;
        linkParams = nullptr;
    }
    return program;
}