#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELO_LINE_LIST 2
#define DELO_TRIANGLE_LIST 3
#define DELO_QUAD_LIST 4

typedef struct VertexArray VertexArray;
struct VertexArray
{
    unsigned int ibo;
    unsigned int vao;
    unsigned int buffer;

    unsigned int id;
    unsigned int type;

    unsigned int count_elements;
    unsigned int indices_per_element;
    unsigned int count_index;
    unsigned int count_position;
    unsigned int layout_float_count;

    float *buffer_position;
    unsigned int *buffer_index;
};

typedef struct Texture Texture;
struct Texture
{
    unsigned int renderer_id;
    unsigned char* local_buffer;
    int width,height,bytes_per_pixel;

};
typedef struct Rectangle Rectangle;
struct Rectangle
{
    int x,y, width,height;
};
typedef struct SpriteBatch SpriteBatch;
struct SpriteBatch
{
    unsigned int count;
    Rectangle *rect_des;
    Rectangle *rect_src;
    VertexArray *vertex_array;

};


void delo2d_matrix_orthographic_projection(float (*matrix)[4][4], float l,float r,float t,float b,float f,float n);

void GLClearError();
void GLCheckError();


void delo2d_rectangle_set(Rectangle *rectengle, int x, int y,int width, int height);

int delo2d_render_setup(GLFWwindow **window, unsigned int width, unsigned int height,const char *title);
int delo2d_render_initialize();
void delo2d_render(VertexArray *vertex_array,Texture *texture,Texture *texture2, unsigned int shader);

void delo2d_load_texture(Texture *texture, char file_path[]);
void delo2d_bind_texture(Texture *texture, unsigned int slot);
void delo2d_unbind_texture();
void delo2d_delete_texture(Texture *texture);



//region vertex array code begin
void delo2d_vertex_set_element(VertexArray *vertex_array, int position,float x, float y, float tex_x,float tex_y,unsigned int texture_slot);
void delo2d_vertex_array_draw(VertexArray *vertex_array);
void delo2d_vertex_array_delete(VertexArray *vertex_array);
void delo2d_vertex_array_create(VertexArray *vertex_array,unsigned int type, unsigned int element_count);
void delo2d_vertex_array_set_data(VertexArray *vertex_array);
void delo2d_vertex_array_bind(VertexArray *vertex_array);
void delo2d_vertex_array_unbind(VertexArray *vertex_array);
//region vertex array code end

//region shader code begin
static unsigned int delo2d_compile_shader(unsigned int type,char *shader_source_code);
static int delo2d_create_shader(char *vertex_shader_source_code, char *fragment_shader_source_code);
static char* delo2d_load_shader(char *path);
static int delo2d_find_keyword(char *string, char *sub_string,int tag);
static char* delo2d_parse_shader(char *source_full,char *keyword_begin,char *keyword_end);
unsigned int delo2d_shader_from_file(char *path_shader);
//region shader code end