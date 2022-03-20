//glfw crosd-platform windows
//glew fetches function pointers from graphics card
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int find_keyword(char *string, char *sub_string,int tag)
{
    int n = 0;
    size_t length = strlen(string);
    for (size_t i = 0; i < length; i++)
    {
        if(string[i] == sub_string[n])
        {
            n++;
        }
        else
        {
            n = 0;
        }
        if(n == strlen(sub_string))
        {            
            if(tag == 0)
            {
                return i+1;
            }
            else
            {
                return i - (strlen(sub_string)-1);
            }
        }
    }
    return 0;
}
static char* parse_shader(char *source_full,char *keyword_begin,char *keyword_end)
{
    int begin = find_keyword(source_full,keyword_begin,0);
    int end = find_keyword(source_full,keyword_end,1);

    if(end > begin)
    {
        int length = end - begin;
        char *src = malloc(sizeof(char)*length+1);
        memcpy(src,&source_full[begin],sizeof(char)*(length));
        src[length] = '\0'; 
        return src; 
    }
    else
    {
        return NULL;
    }
}
unsigned int shader_from_file(char *path_shader)
{
    char *src = load_shader(path_shader); 
    char *src_vert = parse_shader(src,"#VERT_BEGIN","#VERT_END");
    char *src_frag = parse_shader(src,"#FRAG_BEGIN","#FRAG_END");    

    if(src_vert != NULL && src_frag != NULL)
    {
        unsigned int shader = create_shader(src_vert,src_frag); 
        
        free(src);
        free(src_vert);
        free(src_frag);
        return shader;
    }
    else
    {
        return 0;
    }

    
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

    unsigned int shader = shader_from_file("default_shader.glsl");
    
    glUseProgram(shader);

    

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES,0,3);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}