#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int delo2d_compile_shader(unsigned int type,char *shader_source_code)
{    
    unsigned int id = glCreateShader(type);
    char const* src = shader_source_code;
    glShaderSource(id,1,&src,NULL);
    glCompileShader(id);
    return id;
}
static int delo2d_create_shader(char *vertex_shader_source_code, char *fragment_shader_source_code)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = delo2d_compile_shader(GL_VERTEX_SHADER,vertex_shader_source_code);
    unsigned int fs = delo2d_compile_shader(GL_FRAGMENT_SHADER,fragment_shader_source_code);

    glAttachShader(program,vs);
    glAttachShader(program,fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
static char* delo2d_load_shader(char *path)
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
static int delo2d_find_keyword(char *string, char *sub_string,int tag)
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
static char* delo2d_parse_shader(char *source_full,char *keyword_begin,char *keyword_end)
{
    int begin = delo2d_find_keyword(source_full,keyword_begin,0);
    int end = delo2d_find_keyword(source_full,keyword_end,1);

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
unsigned int delo2d_shader_from_file(char *path_shader)
{
    char *src = delo2d_load_shader(path_shader); 
    char *src_vert = delo2d_parse_shader(src,"#VERT_BEGIN","#VERT_END");
    char *src_frag = delo2d_parse_shader(src,"#FRAG_BEGIN","#FRAG_END");    

    if(src_vert != NULL && src_frag != NULL)
    {
        unsigned int shader = delo2d_create_shader(src_vert,src_frag); 
        
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