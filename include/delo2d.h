#pragma once

#define DELO_LINE_LIST 2
#define DELO_TRIANGLE_LIST 3
#define DELO_QUAD_LIST 4

#define DELO_SUCCESS 1
#define DELO_ERROR   0

#include <stdint.h>

typedef struct Vector3 Vector3;
struct Vector3
{
    float x,y,z;
};
typedef struct Color Color;
struct Color
{
    float r,g,b,a;
};
typedef struct Vector4 Vector4;
struct Vector4
{
    float x,y,z,w;
};
typedef struct Vector2 Vector2;
struct Vector2
{
    float x,y;
};
typedef struct Rectangle Rectangle;
struct Rectangle
{
    int x,y,width,height;
};

typedef struct Matrix44 Matrix44;
struct Matrix44
{	// Column-major order
	float x11, x21, x31, x41,
		  x12, x22, x32, x42,
		  x13, x23, x33, x43,
		  x14, x24, x34, x44;
};
typedef struct KeyboardInput KeyboardInput;
struct KeyboardInput
{
    uint8_t move_l;
    uint8_t move_r;
    uint8_t move_up;
    uint8_t move_dn;
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
    uint8_t initialized,type,layout_float_count,indices_per_element;
    uint32_t id,count_elements,count_index,count_position;
    uint32_t ibo,vao,vbo;
    uint32_t *buffer_index;
    float *buffer_position;
};
typedef struct VertexArrayPrimitives VertexArrayPrimitives;
struct VertexArrayPrimitives
{
    uint8_t initialized,type;
    uint32_t layout_float_count,capacity,count;
    uint32_t vao,vbo;
    float *buffer_position;
};

typedef struct Texture Texture;
struct Texture
{
    uint8_t initialized;
    uint32_t renderer_id;
    unsigned char* local_buffer;
    int width,height,bytes_per_pixel;

};
typedef struct RenderTarget RenderTarget;
struct RenderTarget
{
    Matrix44 projection; 
    uint8_t initialized;
    uint32_t vao,vbo,fbo,fbt;
    uint32_t status;
    float vertices[24];
};
typedef struct Rectangle_f Rectangle_f;
struct Rectangle_f
{
    float x,y, width,height;
};
typedef struct SpriteBatch SpriteBatch;
struct SpriteBatch
{
    uint8_t initialized;
    uint8_t called_begin;
    uint8_t called_end;
    uint8_t *flip_horizontally;
    uint8_t *flip_vertically;
    uint8_t *updated;
    uint32_t capacity;
    uint32_t count;
    uint32_t texture_count;
    int32_t *quad_index;
    int32_t shader_id;
    int32_t *texture_index;
    Vector2f *scale;
    Vector2f *skew;
    Vector2f *pivot_point;
    Vector2f *position;
    Matrix44 projection;
    float *orientation;
    Rectangle_f *rect_des;
    Rectangle_f *rect_src;
    VertexArray vertex_array;
    Color *color;
    Texture *textures;
    Color clear_color;

};
typedef struct Sprite Sprite;
struct Sprite
{
    uint32_t batch_index,frame,frames,stride,texture_width,texture_height,quad_index;
    uint8_t updated_tex_coords,flip_horizontally,flip_vertically,loop;
    Rectangle rect_src;
    Rectangle rect_des;
    int32_t texture_index;
    Color color;
    Vector2f position;
    Vector2f scale;
    Vector2f skew;
    Vector2f pivot_point;
    Vector2f offset;
    float orientation,time,duration;
};
typedef struct PrimitiveBatch PrimitiveBatch;
struct PrimitiveBatch
{
    uint8_t initialized,called_begin,called_end;
    Matrix44 projection;
    int32_t shader_id;
    VertexArrayPrimitives vertex_array;
};
typedef struct Glyph Glyph;
struct Glyph
{
    int32_t x,y,w,h,advance,bearing_x;
};
typedef struct SpriteFont SpriteFont;
struct SpriteFont
{
    int font_size;
    int blank_space_offset_x;
    int line_spacing;
    Texture texture;
    Glyph glyphs[512];
    Sprite sprites[512];
};

typedef struct Renderer2D Renderer2D;
struct Renderer2D
{
    unsigned int shader_primitive_default;
    unsigned int shader_sprite_default;
    unsigned int shader_sprite_aa;
    Matrix44 projection;
    SpriteBatch sprite_batch;
    PrimitiveBatch primitive_batch;
    SpriteFont sprite_font;
    Texture gui_sprite_sheet;
};

void GLClearError();
void GLCheckError();

void delo2d_rectangle_set(Rectangle *rectengle, int x, int y,int width, int height);
int delo2d_render_setup(void **window, unsigned int width, unsigned int height,const char *title);

//region render target begin
void delo2d_render_target_create(RenderTarget *rt,float screen_width,float screen_height, float x, float y, float width, float height);
void delo2d_render_target_delete(RenderTarget *rt);
void delo2d_render_target_draw(RenderTarget *render_target, unsigned int shader_id);
void delo2d_render_target_set(unsigned int frame_buffer,float r, float g, float b, float a);
//region rendering end

//region texture begin
uint8_t delo2d_texture_load(Texture *texture, char file_path[]);
void delo2d_texture_bind(unsigned int texture, unsigned int slot);
void delo2d_texture_unbind();
void delo2d_texture_delete(Texture *texture);
void delo2d_texture_copy(Texture *texture_src,Texture *texture_des);
//region texture end

//matrix code begin
Matrix44 matrix44_identity();
Matrix44 matrix44_scale(float x, float y, float z);
Matrix44 matrix44_translation(float x, float y, float z);
Matrix44 matrix44_rotation_z(float theta);
Matrix44 matrix44_rotation_y(float theta);
Matrix44 matrix44_rotation_x(float theta);
Matrix44 matrix44_multiply(Matrix44 a, Matrix44 b);
Matrix44 matrix44_add(Matrix44 a, Matrix44 b);
Matrix44 matrix44_perspective();
Matrix44 matrix44_orthographic_projection(float l,float r,float t,float b,float f,float n);
float* matrix44_to_gl_matrix(Matrix44 *matrix);
void matrix44_multilpy_vector2fp(Vector2fp *vector, Matrix44 transform);
Vector2f matrix44_multilpy_vector2f(Vector2f vector, Matrix44 transform);
Matrix44 matrix44_invert(Matrix44 input);
//matrix code end

//region quads begin
void delo2d_quad_get(Quad *quad, VertexArray *vertex_array, int element_index);
void delo2d_quad_define(VertexArray *vertex_array, int quad_index, Rectangle_f *rect_des,Rectangle_f *rect_src, int texture_index,int texture_width, int texture_height,Color color,int flip_horizontally,int flip_vertically);
void delo2d_quad_translate(Quad *quad, float delta_x, float delta_y);
void delo2d_quad_set_position(Quad *quad,int x,int y);
void delo2d_quad_get_center(Quad *quad,Vector2f *center);
void delo2d_quad_rotate(Quad *quad, float theta);
void delo2d_quad_rotate_around_point(Quad *quad, float theta,float point_x, float point_y);
void delo2d_quad_scale(Quad *quad,float scale_x,float scale_y);
void delo2d_quad_skew(Quad *quad,float skew_x,float skew_y,float pivot_point_x,float pivot_point_y);
//region quads end

//region vertex array code begin
void delo2d_sprite_vertex_array_create(VertexArray *vertex_array,unsigned int type, unsigned int element_count);
void delo2d_sprite_vertex_array_delete(VertexArray *vertex_array);
void delo2d_sprite_vertex_set_element_sprite(VertexArray *vertex_array, int element,float x, float y, float tex_x,float tex_y,float tex_w,float tex_h,unsigned int texture_slot,Color color);
void delo2d_sprite_vertex_set_tex_data(VertexArray *vertex_array, int element,float tex_x,float tex_y,unsigned int texture_slot);
void delo2d_sprite_vertex_array_draw(VertexArray *vertex_array,unsigned int count_elements,unsigned int shader_id,Texture *textures,int texture_count,Matrix44 projection);
void delo2d_sprite_vertex_array_to_graphics_device(VertexArray *vertex_array, intptr_t offset);
void delo2d_sprite_vertex_array_bind(VertexArray *vertex_array);
void delo2d_sprite_vertex_array_unbind();
void delo2d_sprite_vertex_array_delete(VertexArray *vertex_array);
//region vertex array code end

//region shader code begin
uint8_t delo2d_shader_from_file(char *path_shader, uint32_t *shader_id);
//region shader code end

//region sprites code begin
void delo2d_sprite_batch_create(SpriteBatch *sprite_batch,int capacity);
void delo2d_sprite_batch_delete(SpriteBatch *sprite_batch);
void delo2d_sprite_batch_begin(SpriteBatch *sprite_batch,unsigned int shader, Matrix44 projection);
void delo2d_sprite_batch_end(SpriteBatch *sprite_batch);
void delo2d_sprite_define(Sprite *sprite, int dx, int dy,int dw, int dh,int sx, int sy,int sw, int sh,unsigned int texture_index, unsigned int texture_width, unsigned int texture_height, unsigned int stride,unsigned int frames, float duration, Color color,float scale_x,float scale_y,float skew_x,float skew_y,unsigned int flip_horizontally,unsigned int flip_vertically);
void delo2d_sprite_batch_add(SpriteBatch *sprite_batch, Sprite *sprite,Texture *texture);
void delo2d_sprite_batch_add_no_texture(SpriteBatch *sprite_batch, Sprite *sprite);
void delo2d_sprite_batch_add_texture(SpriteBatch *sprite_batch,Texture *texture, int *texture_index);
void delo2d_sprite_batch_to_vertex_array(SpriteBatch *sprite_batch, VertexArray *vertex_array);
void delo2d_sprite_scale_dest_rect(Sprite *sprite, float scale_x, float scale_y);
void delo2d_sprite_rotate(Sprite *sprite,float rotation,VertexArray *vertex_array);
void delo2d_sprite_rotate_around_point(Sprite *sprite,float rotation,float point_x, float point_y, VertexArray *vertex_array);
void delo2d_sprite_set_orientation_around_point(Sprite *sprite,float orientation,float point_x, float point_y,VertexArray *vertex_array);
void delo2d_sprite_set_orientation(Sprite *sprite,float orientation,VertexArray *vertex_array);
void delo2d_sprite_translate(Sprite *sprite,float tx,float ty,VertexArray *vertex_array);
void delo2d_sprite_animate(Sprite *sprite,float dt, unsigned int reverse);
void delo2d_sprite_animate_frame(Sprite *sprite,int frame);
void delo2d_sprite_batch_update_tex_coords(VertexArray *vertex_array,SpriteBatch *sprite_batch, Sprite *sprite,int index);
//region sprites code end

//region camera code begin
void delo2d_camera_move(Matrix44 *projection, float tx, float ty, float screen_width, float screen_height);
void delo2d_camera_set_position(Matrix44 *projection, float x, float y, float screen_width, float screen_height);
void delo2d_camera_set_zoom(Matrix44 *projection, float z, float screen_width, float screen_height);
//region camera code end

//region input code begin
void delo2d_input_update(void *window, KeyboardInput *ki,KeyboardInput *ki_prev); 
void delo2d_input_init(KeyboardInput *ki,KeyboardInput *ki_prev);
//region input code end

//region color code begin
void delo2d_color_set_f(Color *color,float r, float g, float b, float a);
void delo2d_color_set_i(Color *color,int r, int g, int b, int a);
void delo2d_color_set(Color *color1,Color *color2);
void delo2d_color_lerp(Color *result, Color *color_a,Color *color_b, float facor);
//region color code end

//vertex array primitives code begin
void delo2d_primitive_vertex_array_create(VertexArrayPrimitives *vertex_array,unsigned int vertex_capacity,unsigned int shader);
void delo2d_primitive_vertex_array_delete(VertexArrayPrimitives *vertex_array);
void delo2d_primitive_vertex_set_element(VertexArrayPrimitives *vertex_array, int element,float x, float y,float r,float g, float b, float a);
void delo2d_primitive_vertex_array_draw(VertexArrayPrimitives *vertex_array,unsigned int shader_id,Matrix44 projection);
void delo2d_primitive_vertex_array_to_graphics_device(VertexArrayPrimitives *vertex_array, intptr_t offset);
void delo2d_primitive_vertex_array_bind(VertexArrayPrimitives *vertex_array);
void delo2d_primitive_vertex_array_unbind();
//vertex array primitives code end

//primitive batch code begin
void delo2d_primitive_batch_create(PrimitiveBatch *primitive_batch,int capacity);
void delo2d_primitive_batch_delete(PrimitiveBatch *primitive_batch);
void delo2d_primitive_batch_begin(PrimitiveBatch *primitive_batch,unsigned int shader,Matrix44 projection,unsigned int primitive_type);
void delo2d_primitive_batch_add(PrimitiveBatch *primitive_batch,int x, int y,float r,float g, float b, float a);
void delo2d_primitive_batch_end(PrimitiveBatch *primitive_batch);
//primitive batch code end

//text code begin
uint8_t delo2d_sprite_font_load(SpriteFont *sprite_font, char *path, int font_size);
void delo2d_sprite_font_draw(char *text,Vector2f position,Color color,SpriteFont *sprite_font, SpriteBatch *sprite_batch, float scale, int limit_x);
Vector2 delo2d_sprite_font_measure_string(char *text,SpriteFont *sprite_font,float scale, int max);
int delo2d_sprite_font_set_caret_mouse(char *text,SpriteFont *sprite_font,float scale, Vector2 mp);
//text code end
