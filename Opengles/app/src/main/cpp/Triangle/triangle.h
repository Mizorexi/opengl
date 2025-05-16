#include <jni.h>
#include <string>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>

class Triangle{
    public:
        Triangle();
        ~Triangle();
        void init();
        void draw();

    public:
        int initOpengl();
        int loadShader(int type,const char **shadercode);
        //创建和链接程序,每个程序对象必须连接一个顶点着色器和片段着色器
        int createProgram(const char *vShaderStr,const char *fShaderStr);


    private:
        GLuint program;
        GLuint vbo;
        GLuint vao;
        GLuint ebo;
        GLuint mvpLoc;

        void* m_nativeWindow;
    
};
