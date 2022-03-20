#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vertex Vertex;
struct Vertex
{
    float x,y;
    int r,g,b,a;
};
static unsigned int delo2d_compile_shader(unsigned int type,char *shader_source_code);
static int delo2d_create_shader(char *vertex_shader_source_code, char *fragment_shader_source_code);
static char* delo2d_load_shader(char *path);
static int delo2d_find_keyword(char *string, char *sub_string,int tag);
static char* delo2d_parse_shader(char *source_full,char *keyword_begin,char *keyword_end);
unsigned int delo2d_shader_from_file(char *path_shader);