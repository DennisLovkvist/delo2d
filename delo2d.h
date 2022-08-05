/**
 * Author:    Dennis Lövkvist
 * Created:   2022-08-05
 * Version: 1.0
 **/
#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELO_LINE_LIST 2
#define DELO_TRIANGLE_LIST 3
#define DELO_QUAD_LIST 4


typedef struct KeyboardInput KeyboardInput;
struct KeyboardInput
{
    unsigned int move_l;
    unsigned int move_r;
    unsigned int move_up;
    unsigned int move_dn;
};
typedef struct Vector2fp Vector2fp;
struct Vector2fp
{
    float* x;
    float* y;
};
typedef struct Vector2f Vector2f;
struct Vector2f
{
    float x;
    float y;
};
typedef struct Quad Quad;
struct Quad
{
    Vector2fp v0;
    Vector2fp v1;
    Vector2fp v2;
    Vector2fp v3;
};

typedef struct VertexArray VertexArray;
struct VertexArray
{
    unsigned int ibo;
    unsigned int vao;
    unsigned int vbo;

    unsigned int id;
    unsigned int type;

    unsigned int count_elements;
    unsigned int indices_per_element;
    unsigned int count_index;
    unsigned int count_position;
    unsigned int layout_float_count;

    float *buffer_position;
    //[0,1]=position, [2,3]=texture coordinates [4]=texture index
    unsigned int *buffer_index;
};

typedef struct Texture Texture;
struct Texture
{
    unsigned int renderer_id;
    unsigned char* local_buffer;
    int width,height,bytes_per_pixel;

};
typedef struct RenderTarget RenderTarget;
struct RenderTarget
{
    unsigned int vao,vbo,fbo,framebufferTexture,rbo;
    GLenum status;
    float vertices[24];
};
typedef struct Rectangle Rectangle;
struct Rectangle
{
    int x,y, width,height;
};
typedef struct Rectangle_f Rectangle_f;
struct Rectangle_f
{
    float x,y, width,height;
};
typedef struct Color Color;
struct Color
{
    float r,g,b,a;
};
typedef struct SpriteBatch SpriteBatch;
struct SpriteBatch
{
    unsigned int called_begin;
    unsigned int called_end;
    float projection[4][4];
    unsigned int shader_id;
    VertexArray vertex_array;
    unsigned int capacity;
    unsigned int count;
    Rectangle_f *rect_des;
    Rectangle_f *rect_src_normalized;
    unsigned int *texture_index;
    unsigned int *quad_index;
    Color *color;
    unsigned int *flip_horizontally;
    unsigned int *flip_vertically;
    unsigned int *updated;
    Vector2f *scale;
    Vector2f *skew;
    Vector2f *pivot_point;
    Vector2f *position;
    Texture *textures;
    unsigned int texture_count;
    float *orientation;
    Color clear_color;

};
typedef struct Sprite Sprite;
struct Sprite
{
    unsigned int batch_index,frame,frames,stride,texture_index,texture_width,texture_height,quad_index,flip_horizontally,flip_vertically;
    float orientation,time,duration;
    Rectangle_f rect_src;
    Rectangle_f rect_des;
    unsigned int updated_tex_coords;
    Color color;
    Vector2f position;
    Vector2f scale;
    Vector2f skew;
    Vector2f pivot_point;
};


void GLClearError();
void GLCheckError();

void delo2d_rectangle_set(Rectangle *rectengle, int x, int y,int width, int height);
int delo2d_render_setup(GLFWwindow **window, unsigned int width, unsigned int height,const char *title);
int delo2d_render_initialize();

//region render target begin
void delo2d_render_target_create(RenderTarget *rt,float screen_width,float screen_height);
void delo2d_render_target_draw(RenderTarget *render_target, unsigned int shader_id);
void delo2d_render_target_set(unsigned int frame_buffer,float r, float g, float b, float a);
//region rendering end

//region texture begin
void delo2d_texture_load(Texture *texture, char file_path[]);
void delo2d_texture_bind(unsigned int texture, unsigned int slot);
void delo2d_texture_unbind();
void delo2d_texture_delete(Texture *texture);
void delo2d_texture_copy(Texture *texture_src,Texture *texture_des);
//region texture end

//region matrices begin
void delo2d_matrix_mul_vector2fp_matrix33(Vector2fp *vector,float (*R)[3][3]);
void delo2d_rotation_matrix(float (*R)[3][3],float theta, float tx, float ty);
void delo2d_matrix_orthographic_projection(float (*matrix)[4][4], float l,float r,float t,float b,float f,float n);
void delo2d_projection_matrix_set(float *projection_src,float *projection_des);
//region matrices end

//region quads begin
void delo2d_quad_get(Quad *quad, VertexArray *vertex_array, int element_index);
void delo2d_quad_define(VertexArray *vertex_array, int quad_index, Rectangle_f *rect_des,Rectangle_f *rect_src, int texture_index,Color color,int flip_horizontally,int flip_vertically);
void delo2d_quad_translate(Quad *quad, float delta_x, float delta_y);
void delo2d_quad_set_position(Quad *quad,int x,int y);
void delo2d_quad_get_center(Quad *quad,Vector2f *center);
void delo2d_quad_rotate(Quad *quad, float theta);
void delo2d_quad_rotate_around_point(Quad *quad, float theta,float point_x, float point_y);
void delo2d_quad_scale(Quad *quad,float scale_x,float scale_y);
void delo2d_quad_skew_top(Quad *quad,float skew);
//region quads end

//region vertex array code begin

void delo2d_vertex_array_create(VertexArray *vertex_array,unsigned int type, unsigned int element_count);
void delo2d_vertex_set_element(VertexArray *vertex_array, int position,float x, float y, float tex_x,float tex_y,unsigned int texture_slot,Color color);
void delo2d_vertex_set_tex_data(VertexArray *vertex_array, int position,float tex_x,float tex_y,unsigned int texture_slot);
void delo2d_vertex_array_draw(VertexArray *vertex_array,unsigned int count_elements,unsigned int shader_id,Texture *textures,int texture_count,float *projection);
void delo2d_vertex_array_to_graphics_device(VertexArray *vertex_array, GLintptr offset);
void delo2d_vertex_array_bind(VertexArray *vertex_array);
void delo2d_vertex_array_unbind();
void delo2d_vertex_array_delete(VertexArray *vertex_array);
//region vertex array code end

//region shader code begin
static unsigned int delo2d_shader_compile(unsigned int type,char *shader_source_code);
static int delo2d_shader_create(char *vertex_shader_source_code, char *fragment_shader_source_code);
static char* delo2d_shader_load(char *path);
static int delo2d_find_keyword(char *string, char *sub_string,int tag);
static char* delo2d_shader_parse(char *source_full,char *keyword_begin,char *keyword_end);
unsigned int delo2d_shader_from_file(char *path_shader);
//region shader code end

//region sprites code begin
void delo2d_sprite_batch_create(SpriteBatch *sprite_batch,int capacity);
void delo2d_sprite_batch_begin(SpriteBatch *sprite_batch,unsigned int shader, float *projection);
void delo2d_sprite_batch_end(SpriteBatch *sprite_batch);
void delo2d_sprite_define(Sprite *sprite, float dx, float dy,float dw, float dh,float sx, float sy,float sw, float sh,unsigned int texture_index, unsigned int texture_width, unsigned int texture_height, unsigned int stride,unsigned int frames, float duration, Color color,float scale_x,float scale_y,float skew_x,float skew_y,unsigned int flip_horizontally,unsigned int flip_vertically);
void delo2d_sprite_batch_add(SpriteBatch *sprite_batch, Sprite *sprite,Texture *texture);
void delo2d_sprite_batch_add_texture(SpriteBatch *sprite_batch,Texture *texture, unsigned int *texture_index);
void delo2d_sprite_batch_to_vertex_array(SpriteBatch *sprite_batch, VertexArray *vertex_array);
void delo2d_sprite_scale_dest_rect(Sprite *sprite, float scale_x, float scale_y);
void delo2d_sprite_rotate(Sprite *sprite,float rotation,VertexArray *vertex_array);
void delo2d_sprite_rotate_around_point(Sprite *sprite,float rotation,float point_x, float point_y, VertexArray *vertex_array);
void delo2d_sprite_set_orientation_around_point(Sprite *sprite,float orientation,float point_x, float point_y,VertexArray *vertex_array);
void delo2d_sprite_set_orientation(Sprite *sprite,float orientation,VertexArray *vertex_array);
void delo2d_sprite_translate(Sprite *sprite,float tx,float ty,VertexArray *vertex_array);
void delo2d_sprite_animate(Sprite *sprite,float dt);
void delo2d_sprite_batch_update_tex_coords(VertexArray *vertex_array,SpriteBatch *sprite_batch, Sprite *sprite,int index);

//region sprites code end

//region camera code begin
void delo2d_camera_move(float *projection, float tx, float ty, float screen_width, float screen_height);
void delo2d_camera_set_position(float *projection, float x, float y, float screen_width, float screen_height);
void delo2d_camera_set_zoom(float *projection, float z, float screen_width, float screen_height);
//region camera code end

//region input code begin
void delo2d_input_update(GLFWwindow *window, KeyboardInput *ki,KeyboardInput *ki_prev); 
void delo2d_input_init(KeyboardInput *ki,KeyboardInput *ki_prev);
//region input code end

//region color code begin
void delo2d_color_set_f(Color *color,float r, float g, float b, float a);
void delo2d_color_set_i(Color *color,int r, int g, int b, int a);
void delo2d_color_lerp(Color *result, Color *color_a,Color *color_b, float facor);
//region color code end
