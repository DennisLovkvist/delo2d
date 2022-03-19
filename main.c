//glfw crosd-platform windows
//glew fetches function pointers from graphics card
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC 1
typedef struct Vertex Vertex;
struct Vertex
{
    float x,y;
    int r,g,b,a;
};
static unsigned int compile_shader(unsigned int type,char *shader_source_code)
{    
    unsigned int id = glCreateShader(type);
    char const* src = shader_source_code;
    glShaderSource(id,1,&src,NULL);
    glCompileShader(id);


    return id;

}
static int create_shader(char *vertex_shader_source_code, char *fragment_shader_source_code)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compile_shader(GL_VERTEX_SHADER,vertex_shader_source_code);
    unsigned int fs = compile_shader(GL_FRAGMENT_SHADER,fragment_shader_source_code);

    glAttachShader(program,vs);
    glAttachShader(program,fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
static char* load_shader(char *path)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *source_code = malloc(fsize + sizeof(char));
    fread(source_code, fsize, 1, f);
    fclose(f);
    source_code[fsize] = '\0';
    return source_code;
}
unsigned int shader_from_files(char *path_vertex_shader, char *path_fragment_shader)
{
    char *source_code_vertex_shader = load_shader(path_vertex_shader);
    char *source_code_fragment_shader = load_shader(path_fragment_shader);    

    unsigned int shader = create_shader(source_code_vertex_shader,source_code_fragment_shader);
   
    free(source_code_vertex_shader);
    free(source_code_fragment_shader);

    return shader;
}
int main(void)
{
    



    GLFWwindow* window;
   

    if (!glfwInit())
    {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK)
    {
        return -1;
    }

    float vertices[6] = { -0.5f,-0.5f,0.0f,0.5f,0.5f,-0.5f};

    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,6 * sizeof(float),vertices,GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, sizeof(float)*2,0);
    glEnableVertexAttribArray(0);

    unsigned int shader = shader_from_files("shader.vert","shader.frag");
    glUseProgram(shader);

    

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES,0,3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteShader(shader);
    glfwTerminate();
    return 0;
}