#include <jni.h>
#include <string>
#include "triangle.h"

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

typedef struct
{
   // Handle to a program object
   GLuint programObject;

} UserData;

const char *vertexShaderSource =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

const char *fragmentShaderSource =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

//加载着色器
int loadShader(int type, const char *shadercode){
    int shader = glCreateShader(type);
    if (shader == 0) {
        LOGE("Failed to create shader of type %d", type);
        return 0;
    }
    
    glShaderSource(shader, 1, &shadercode, NULL);
    glCompileShader(shader);

    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            LOGE("Error compiling shader:\n%s\n", infoLog);
            delete[] infoLog;
        }
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

//创建和链接程序
int createProgram(ESContext *esContext, const char *vShaderStr, const char *fShaderStr) {
    UserData* userData = (UserData*)esContext->userData;
    
    LOGE("开始创建着色器程序");
    
    // 创建着色器
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vShaderStr);
    if (!vertexShader) {
        LOGE("顶点着色器创建失败");
        return GL_FALSE;
    }
    
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fShaderStr);
    if (!fragmentShader) {
        LOGE("片段着色器创建失败");
        glDeleteShader(vertexShader);
        return GL_FALSE;
    }
    
    // 创建程序
    GLuint program = glCreateProgram();
    if (!program) {
        LOGE("程序对象创建失败");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return GL_FALSE;
    }
    
    // 附加着色器
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    // 链接程序
    glLinkProgram(program);
    
    // 检查链接状态
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            LOGE("链接程序失败: %s", infoLog);
            free(infoLog);
        }
        
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return GL_FALSE;
    }
    
    // 存储程序对象
    userData->programObject = program;
    
    // 清理
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    LOGE("着色器程序创建成功: ID=%d", program);
    return GL_TRUE;
}

void Draw(ESContext *esContext){
    LOGE("Draw开始执行");
    
    // 检查OpenGL上下文是否有效
    if (!eglGetCurrentContext()) {
        LOGE("没有有效的OpenGL上下文");
        return;
    }
    
    UserData *userData = (UserData *)esContext->userData;
    if (!userData || !userData->programObject) {
        LOGE("无效的userData或programObject: %p, program: %d", 
             userData, userData ? userData->programObject : 0);
        return;
    }
    
    // 设置明显的背景颜色，改为黑色以便更容易看到红色三角形
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // 黑色背景
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 检查OpenGL错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("清除缓冲区后OpenGL错误: 0x%04X", error);
    }
    
    // 设置视口
    glViewport(0, 0, esContext->width, esContext->height);
    
    // 使用着色器程序
    glUseProgram(userData->programObject);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("glUseProgram后OpenGL错误: 0x%04X", error);
    }
    
    // 准备顶点数据
    GLfloat vVertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    // 设置顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("glVertexAttribPointer后OpenGL错误: 0x%04X", error);
    }
    
    glEnableVertexAttribArray(0);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("glEnableVertexAttribArray后OpenGL错误: 0x%04X", error);
    }
    
    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("glDrawArrays后OpenGL错误: 0x%04X", error);
    }
    
    // 交换缓冲区
    EGLBoolean swapResult = eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
    if (!swapResult) {
        LOGE("eglSwapBuffers失败: 0x%04X", eglGetError());
    } else {
        LOGE("缓冲区交换成功");
    }
}

void Shutdown(ESContext *esContext){
    UserData *userData = (UserData *)esContext->userData;
    glDeleteProgram(userData->programObject);
}

int esMain(ESContext *esContext){
    LOGE("esMain");
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext , "Hello triangle", 400, 400, ES_WINDOW_RGB);
    createProgram(esContext,vertexShaderSource, fragmentShaderSource);
    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, Shutdown);
    return GL_TRUE;
}