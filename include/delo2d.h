/*
 * delo2d.h
 * 
 * A header-only, minimalistic 2D graphics library in C, 
 * focused on providing lightweight and efficient functionality. 
 * Primarily tailored for 2D rendering with limited support for basic 3D operations.
 * 
 * License:
 * MIT License
 * See end of file for license information.
 * 
 * Copyright (c) 2024 Dennis L�vkvist
 *
 * Author: Dennis L�vkvist
 * Date: 2024-01-24
 * 
 * Usage:
 * Define DELO2D_FUNCTION_SIGNATURES for access to function signatures.
 * Define DELO2D_IMPLEMENTATION for implementation.
 * Example:
 * #define DELO2D_IMPLEMENTATION
 * #include <delo2d.h>
 */

#pragma once

#include <stdint.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <float.h>

// ================================
// delo2d
// Define DELO2D_FUNCTION_SIGNATURES for access to function signatures.
// Define DELO2D_IMPLEMENTATION for implementation.
// Example:
// #define DELO2D_IMPLEMENTATION
// #include <delo2d.h>
// ================================

#define DELO_SUCCESS 0
#define DELO_ERROR   1

#define DELO_LINE_LIST     1
#define DELO_TRIANGLE_LIST 2

typedef struct GlfwCallbackData GlfwCallbackData;
struct GlfwCallbackData
{
    GLFWwindow *window;
    void *camera;
    char key_buffer[16];
    uint16_t key_buffer_length;
};
typedef struct HidState HidState;
struct HidState
{
    uint8_t mouse_button_left;
    uint8_t mouse_button_right;
    uint8_t mouse_button_middle;

    double mouse_position_x;
    double mouse_position_y;

    int window_width;
    int window_height;

    int key_escape;
    int key_backspace;
    int key_enter;
    int key_arrow_left;
    int key_arrow_right;
    int key_ctrl_left;
    int key_shift_left;
    int key_c;
    int key_v;
    int key_a;
    int key_tab;
};

typedef struct D2DContext D2DContext;
struct D2DContext
{
    GLint back_buffer_width, back_buffer_height, screen_width, screen_height;
    GLFWwindow *window;
    GlfwCallbackData glfw_callback_data;
    HidState         hid_state;
    HidState         hid_state_prev;
    double t1,t0,dt;
    float t;
    GLfloat projection_matrix[16];
};
typedef struct Texture Texture;
struct Texture
{
    uint8_t initialized;
    uint32_t renderer_id;
    unsigned char *local_buffer;
    int width, height, bytes_per_pixel;
};
typedef struct Vector2f Vector2f;
struct Vector2f
{
    float x,y;
};
typedef struct Vector4f Vector4f;
struct Vector4f
{
    float x,y,z,w;
};
typedef struct Color Color;
struct Color
{
    float r,g,b,a;
};
typedef struct Rectangle_f Rectangle_f;
struct Rectangle_f
{
    float x,y,width,height;
};
typedef struct Sprite Sprite;
struct Sprite
{
    Rectangle_f src_rect;
    Vector2f position;
    Vector2f offset;
    Color color;
    float width, height;
};
typedef struct SpriteAnimation SpriteAnimation;
struct SpriteAnimation
{
    float time, duration;
    uint16_t frame;
    uint16_t frames;
    uint16_t stride;
    uint8_t loop;
};
typedef struct Matrix44 Matrix44;
struct Matrix44
{ 
    float x11, x21, x31, x41,
          x12, x22, x32, x42,
          x13, x23, x33, x43,
          x14, x24, x34, x44;
};
typedef struct RenderTarget RenderTarget;
struct RenderTarget
{
    Matrix44 projection; 
    uint8_t initialized;
    uint32_t vao,vbo,fbo,fbt;
    uint32_t status;
    float vertices[24];
    Texture texture;
};
typedef struct Camera2D Camera2D;
struct Camera2D
{
    Matrix44 projection;
    Matrix44 view;
    Matrix44 view_projection;
    Vector2f position;
    float width;
    float height;
    float view_width_in_world_units;
    float view_height_in_world_units;
};
typedef struct FontMeasurement FontMeasurement;
struct FontMeasurement
{
    uint16_t height_in_px;
    uint16_t width_in_px;
    uint16_t width_in_ch;
    uint8_t limit_ch_hit;
    uint8_t limit_px_hit;
};

typedef struct Glyph Glyph;
struct Glyph
{
    int32_t x, y, w, h, advance, bearing_x, bearing_y;
};
typedef struct SpriteFont SpriteFont;
struct SpriteFont
{
    uint16_t font_size;
    uint32_t line_spacing;

    Glyph glyphs[256];
    int32_t advance;
    Texture texture;
    int8_t padding;
};
typedef struct RendererSprite RendererSprite;
struct RendererSprite
{
    D2DContext*     context;
    Vector2f*       offsets;
    Rectangle_f*    src_rects;
    Color*          colors;
    Texture*        texture;
    float *         texture_indices;
    Matrix44*       transforms;
    Matrix44        projection;
    Vector2f*        limit_ys;
    int32_t         texture_id_0;
    int32_t         texture_id_1;
    int32_t         texture_id_2;
    int32_t         texture_id_3;
    GLuint          shader;
    GLuint          capacity;
    GLuint          count;
    GLuint          uniform_projection;
    GLuint          vao;
    GLuint          vbo_vertices;
    GLuint          vbo_colors;
    GLuint          vbo_transforms;
    GLuint          vbo_offsets;
    GLuint          vbo_src_rects;
    GLint          vbo_tex_indices;
    GLuint          vbo_limit_y;
    GLuint          uniform_location_u_texture0;
    GLuint          uniform_location_u_texture1;
    GLuint          uniform_location_u_texture2;
    GLuint          uniform_location_u_texture3; 
    GLuint          uniform_location_u_flip; 
    GLuint          uniform_location_u_projection; 
    GLuint          uniform_location_u_mvp; 
    uint8_t         flip; 
    uint8_t         change_mask;

};
typedef struct PrimitiveVertex PrimitiveVertex;
struct PrimitiveVertex
{
    Vector2f position;
    Color    color;
};
typedef struct RendererPrimitive RendererPrimitive;
struct RendererPrimitive
{
    PrimitiveVertex* vertices;
    D2DContext*      context;
    Matrix44         projection;
    Matrix44         projection_default;
    GLuint           capacity;
    GLuint           count;
    GLuint           uniform_projection;
    GLuint           uniform_location_u_mvp;
    GLuint           vao;
    GLuint           vbo_vertices;
    GLuint           shader;
    GLuint           shader_default;
    uint8_t          type;
};
typedef struct RendererCircle RendererCircle;
struct RendererCircle
{
    float*           vertices;
    Color*           colors;
    float*           radii;
    Vector2f*        positions;
    D2DContext*      context;
    Matrix44         projection;
    Matrix44         projection_default;
    GLuint           capacity;
    GLuint           count;
    GLuint           uniform_projection;
    GLuint           uniform_location_u_mvp;
    GLuint           uniform_location_u_bbh;
    GLuint           vao;
    GLuint           vbo_vertices;
    GLuint           vbo_colors;
    GLuint           vbo_radii;
    GLuint           vbo_positions;
    GLuint           shader;
    GLuint           shader_default;
    uint8_t          type;
};

typedef struct RendererSpriteFont RendererSpriteFont;
struct RendererSpriteFont
{
    Matrix44        projection;
    D2DContext*     context;
    Color*          colors;
    Matrix44*       transforms;
    Vector2f*       offsets;
    Rectangle_f*    src_rects;
    float*          texture_indices;
    Vector2f*       limit_ys;
    GLuint          capacity;
    GLuint          count;
    GLuint          uniform_projection;
    GLuint          uniform_location_u_texture0;
    GLuint          uniform_location_u_texture1;
    GLuint          uniform_location_u_texture2;
    GLuint          uniform_location_u_texture3;
    GLuint          uniform_location_u_mvp;    
    GLuint          uniform_location_u_flip; 
    GLuint          uniform_location_u_back_buffer_height;
    GLuint          vao;
    GLuint          vbo_vertices;
    GLuint          vbo_colors;
    GLuint          vbo_transforms;
    GLuint          vbo_offsets;
    GLuint          vbo_src_rects;
    GLuint          vbo_tex_indices;
    GLuint          vbo_limit_y;
    GLuint          shader;
    int32_t         texture_id_0;
    int32_t         texture_id_1;
    int32_t         texture_id_2;
    int32_t         texture_id_3;
    uint8_t         flip;
};

#if defined(DELO2D_FUNCTION_SIGNATURES) || defined(DELO2D_IMPLEMENTATION)
// ================================
// OpenGL debugging functions
// ================================
void gl_clear_error();
void gl_check_error();
// ================================
// Graphics context functions
// ================================
int8_t d2d_context_init(D2DContext *context, uint16_t width, uint16_t height, char *window_title);
void   d2d_frame_begin(D2DContext *context);
void   d2d_frame_end(D2DContext *context);
// ================================
// Matrix44 functions
// ================================
Matrix44 d2d_matrix44_identity();
Matrix44 d2d_matrix44_skew(float sx, float sy);
Matrix44 d2d_matrix44_scale(float x, float y, float z);
Matrix44 d2d_matrix44_translation(float x, float y, float z);
Matrix44 d2d_matrix44_rotation_z(float theta);
Matrix44 d2d_matrix44_rotation_y(float theta);
Matrix44 d2d_matrix44_rotation_x(float theta);
Matrix44 d2d_matrix44_multiply(Matrix44 a, Matrix44 b);
Matrix44 d2d_matrix44_add(Matrix44 a, Matrix44 b);
Matrix44 d2d_matrix44_invert(Matrix44 input);
Matrix44 d2d_matrix44_orthographic_projection(float l, float r, float t, float b, float f, float n);
Vector2f d2d_matrix44_multilpy_vector2f(Vector2f vector, Matrix44 transform);
Vector4f d2d_matrix44_multiply_vector4f(Matrix44 matrix, Vector4f vector);
uint8_t  d2d_matrix44_invert2(Matrix44 *input, Matrix44 *output);
float*   d2d_matrix44_to_gl_matrix(Matrix44 *matrix);
float    d2d_matrix44_calculate_sub_determinant(Matrix44 m, int a, int b, int c, int d);
float    d2d_matrix44_calculate_determinant(const Matrix44 *m);
void     d2d_matrix44_print(const Matrix44* matrix);
void     d2d_map_mouse_to_ortho_space(float mouseX, float mouseY, int screenWidth, int screenHeight, Camera2D* camera, float* orthoX, float* orthoY);
// ================================
// Human Input Device functions
// ================================
void d2d_hid_control_init(HidState *hid_state, GLFWwindow *window);
void d2d_hid_control_update(HidState *hid_state, GLFWwindow *window, int window_width, int window_height,int back_buffer_width, int back_buffer_height);
void d2d_append_utf8(uint32_t codepoint, char *buffer, uint16_t *index);
void d2d_callback_character(GLFWwindow *window, unsigned int codepoint);
// ================================
// Sprite functions
// ================================
void d2d_sprite_define(Sprite *sprite, float width, float height, Rectangle_f src_rect);
void d2d_sprite_animation_init(SpriteAnimation *sprite_animation, float duration,uint16_t frames, uint16_t stride, uint8_t loop);
void d2d_sprite_animate_frame(Sprite *sprite, SpriteAnimation *animation, int frame);
void d2d_sprite_animate(Sprite *sprite, SpriteAnimation *animation, float dt, unsigned int reverse);
void d2d_sprite_transform(uint32_t width, uint32_t height, Matrix44 *transform, Vector2f scale, Vector2f skew, float rotation);
// ================================
// Render Target functions
// ================================
int8_t d2d_render_target_init(RenderTarget *rt,float screen_width,float screen_height, float x, float y, float width, float height);
// ================================
// Shader functions
// ================================
void    d2d_shader_check_compile_status(GLuint shader);
int8_t  d2d_shader_load_file(const char *path, char **source_code);
int8_t  d2d_shader_load(char *path_shader_vert, char *path_shader_frag, uint32_t *shader_id);
uint8_t d2d_shader_compile(uint32_t type, char *shader_source_code, uint32_t *id);
uint8_t d2d_shader_create(char *source_code_shader_vertex, char *source_code_shader_fragment, uint32_t *program);
// ================================
// Texture functions
// ================================
int8_t d2d_texture_load(Texture *texture, char file_path[]);
// ================================
// Rectange functions
// ================================
int8_t d2d_rectangle_within_bounds(Rectangle_f *r, int x, int y);
// ================================
// Math functions
// ================================
float d2d_math_radians(float degrees) ;
float d2d_math_distance(Vector2f p1,Vector2f p2) ;
// ================================
// Camera2D functions
// ================================
int8_t d2d_camera2d_init(Camera2D *camera, D2DContext *context,float width, float height);
void   d2d_camera2d_update(Camera2D *camera);
void   d2d_camera2d_move(Camera2D *camera, float tx, float ty);
void   d2d_camera2d_zoom(Camera2D *camera, float zoom_factor);
void   d2d_camera2d_rotate(Camera2D *camera, float t);
// ================================
// Color functions
// ================================
void d2d_color_set_f(Color *color, float r, float g, float b, float a);
void d2d_color_set_i(Color *color, int r, int g, int b, int a);
void d2d_color_set(Color *color1, Color *color2);
void d2d_color_lerp(Color *result, Color *color_a, Color *color_b, float facor);
// ================================
// Renderer Circle functions
// ================================
int8_t d2d_renderer_circle_init(RendererCircle* renderer, uint32_t capacity, D2DContext* context);
int8_t d2d_renderer_circle_apply_shader(RendererCircle* renderer,uint32_t shader);
int8_t d2d_renderer_circle_update(RendererCircle* renderer);
int8_t d2d_renderer_circle_render(RendererCircle* renderer,Matrix44* projection,uint8_t flip);
int8_t d2d_renderer_circle_add(RendererCircle* renderer,Vector2f position,Color color,float radius);
int8_t d2d_renderer_circle_begin(RendererCircle* renderer,Matrix44* projection,GLuint* shader);
int8_t d2d_renderer_circle_end(RendererCircle* renderer);
// ================================
// Renderer Primitive functions
// ================================
int8_t d2d_renderer_primitive_init(RendererPrimitive* renderer,uint32_t capacity,D2DContext* context);
int8_t d2d_renderer_primitive_apply_shader(RendererPrimitive* renderer,uint32_t shader);
int8_t d2d_renderer_primitive_update(RendererPrimitive* renderer);
int8_t d2d_renderer_primitive_render(RendererPrimitive* renderer,Matrix44* projection,uint8_t flip);
int8_t d2d_renderer_primitive_add(RendererPrimitive* renderer, Vector2f position,Color color);
int8_t d2d_renderer_primitive_add_line(RendererPrimitive* renderer,Vector2f position_a,Vector2f position_b,Color color);
int8_t d2d_renderer_primitive_add_rectangle(RendererPrimitive* renderer,Rectangle_f rectangle,Color color);
int8_t d2d_renderer_primitive_add_rectangle_outline(RendererPrimitive* renderer,Rectangle_f rectangle,Color color);
int8_t d2d_renderer_primitive_begin(RendererPrimitive* renderer, Matrix44* projection, GLuint* shader,uint8_t type);
int8_t d2d_renderer_primitive_end(RendererPrimitive* renderer);
// ================================
// Renderer Sprite functions
// ================================
int8_t d2d_renderer_sprite_init(RendererSprite* renderer, uint32_t capacity,D2DContext* context);
int8_t d2d_renderer_sprite_apply_shader(RendererSprite* renderer,uint32_t shader);
int8_t d2d_renderer_sprite_update(RendererSprite* renderer);
int8_t d2d_renderer_sprite_render(RendererSprite* renderer);
int8_t d2d_renderer_sprite_add_texture(RendererSprite* renderer,int32_t texture_id);
int8_t d2d_renderer_sprite_add2(RendererSprite* renderer,Sprite* sprite,Texture* texture);
int8_t d2d_renderer_sprite_add(RendererSprite* renderer,float x,float y,float dest_width,float dest_height,float src_x,float src_y,float src_width,float src_height,Texture* texture,Color* color);
int8_t d2d_renderer_sprite_begin(RendererSprite* renderer,Matrix44 projection);
int8_t d2d_renderer_sprite_end(RendererSprite* renderer);
// ================================
// Renderer SpriteFont functions
// ================================
int8_t d2d_renderer_sprite_font_init(RendererSpriteFont* renderer,D2DContext* context,uint32_t capacity);
int8_t d2d_renderer_sprite_font_apply_shader(RendererSpriteFont* renderer,uint32_t shader);
int8_t d2d_renderer_sprite_font_update(RendererSpriteFont* renderer);
int8_t d2d_renderer_sprite_font_add_font(RendererSpriteFont* renderer,SpriteFont* sprite_font);
int8_t d2d_renderer_sprite_font_render(RendererSpriteFont* renderer);
int8_t d2d_renderer_sprite_font_begin(RendererSpriteFont* renderer,Matrix44 projection);
int8_t d2d_renderer_sprite_font_end(RendererSpriteFont* renderer);
int8_t d2d_renderer_sprite_font_add_text(RendererSpriteFont* renderer,SpriteFont* sprite_font,char* text,uint32_t max_length,Vector2f position,Color color,Vector2f limit_y);
// ================================
// SpriteFont functions
// ================================
int8_t    d2d_sprite_font_load(SpriteFont *sprite_font, char *path, uint16_t font_size);
int32_t   d2d_sprite_font_set_caret_mouse(char *text, uint32_t offset, SpriteFont *sprite_font, Vector2f mp);
uint32_t* d2d_sprite_font_convert_to_unicode(const char *string_utf8);
uint32_t  d2d_sprite_font_calc_char_limit(char *text, SpriteFont *sprite_font, int32_t max_width);
void      d2d_sprite_font_measure_string(char *text, SpriteFont *sprite_font, FontMeasurement *measurement,int16_t limit_ch,int16_t limit_px);
Vector2f  d2d_sprite_font_measure_string2(char *text, SpriteFont *sprite_font);
#endif

#if defined(DELO2D_IMPLEMENTATION)
#include <locale.h>
#include <math.h>
#include <time.h>
#include <stb_image.h>
// ================================
// OpenGL debugging functions
// ================================
void gl_clear_error()
{
    GLenum error;
    int maxIterations = 1000; // or another reasonable limit
    int iteration = 0;

    while ((error = glGetError()) != GL_NO_ERROR && iteration < maxIterations)
    {
        printf("OpenGL Error: %d\n", error);
        iteration++;
    }

    if (iteration >= maxIterations)
    {
        printf("GLClearError: Reached max iteration limit while clearing errors.\n");
    }
}
void gl_check_error()
{
    GLenum error;
    while ((error = glGetError()))
    {
        printf("e: %i", error);
        printf("%c", '\n');
    }
}


// ================================
// Matrix44 functions
// ================================
Matrix44 d2d_matrix44_identity()
{
    struct Matrix44 matrix =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return matrix;
}
Matrix44 d2d_matrix44_skew(float sx
                          ,float sy
                          )
{
    struct Matrix44 matrix =
    {
        1, sx, sy, 0,
        0, 1,  0,  0,
        0, 0,  1,  0,
        0, 0,  0,  1
    };
    return matrix;
}

Matrix44 d2d_matrix44_scale(float x
                           ,float y
                           ,float z
                           )
{
    struct Matrix44 matrix =
    {
        x,0,0,0,
        0,y,0,0,
        0,0,z,0,
        0,0,0,1,
    };
    return matrix;
}Matrix44 d2d_matrix44_translation(float x
                                  ,float y
                                  ,float z
                                  )
{
    struct Matrix44 matrix =
    {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
    return matrix;
}

Matrix44 d2d_matrix44_rotation_z(float theta)
{
    struct Matrix44 matrix =
    {
        cos(theta) ,sin(theta), 0, 0,
       -sin(theta) ,cos(theta), 0, 0,
        0          ,0,          1, 0,
        0          ,0,          0, 1
    };
    return matrix;
}

Matrix44 d2d_matrix44_rotation_y(float theta)
{
    struct Matrix44 matrix =
    {
        cos(theta) ,0, -sin(theta), 0,
        0          ,1,  0,          0,
        sin(theta) ,0,  cos(theta), 0,
        0          ,0,  0,          1
    };
    return matrix;
}

Matrix44 d2d_matrix44_rotation_x(float theta)
{
    struct Matrix44 matrix =
    {
        1 ,0           ,0         ,0,
        0 , cos(theta) ,sin(theta),0,
        0 ,-sin(theta) ,cos(theta),0,
        0 ,0           ,0         ,1
    };
    return matrix;
}
Matrix44 d2d_matrix44_multiply(Matrix44 a, Matrix44 b)
{
    Matrix44 result = {0};

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++) 
        {
            result.x11 = (i == 0 && j == 0) ? a.x11 * b.x11 + a.x12 * b.x21 + a.x13 * b.x31 + a.x14 * b.x41 : result.x11;
            result.x21 = (i == 1 && j == 0) ? a.x21 * b.x11 + a.x22 * b.x21 + a.x23 * b.x31 + a.x24 * b.x41 : result.x21;
            result.x31 = (i == 2 && j == 0) ? a.x31 * b.x11 + a.x32 * b.x21 + a.x33 * b.x31 + a.x34 * b.x41 : result.x31;
            result.x41 = (i == 3 && j == 0) ? a.x41 * b.x11 + a.x42 * b.x21 + a.x43 * b.x31 + a.x44 * b.x41 : result.x41;

            result.x12 = (i == 0 && j == 1) ? a.x11 * b.x12 + a.x12 * b.x22 + a.x13 * b.x32 + a.x14 * b.x42 : result.x12;
            result.x22 = (i == 1 && j == 1) ? a.x21 * b.x12 + a.x22 * b.x22 + a.x23 * b.x32 + a.x24 * b.x42 : result.x22;
            result.x32 = (i == 2 && j == 1) ? a.x31 * b.x12 + a.x32 * b.x22 + a.x33 * b.x32 + a.x34 * b.x42 : result.x32;
            result.x42 = (i == 3 && j == 1) ? a.x41 * b.x12 + a.x42 * b.x22 + a.x43 * b.x32 + a.x44 * b.x42 : result.x42;

            result.x13 = (i == 0 && j == 2) ? a.x11 * b.x13 + a.x12 * b.x23 + a.x13 * b.x33 + a.x14 * b.x43 : result.x13;
            result.x23 = (i == 1 && j == 2) ? a.x21 * b.x13 + a.x22 * b.x23 + a.x23 * b.x33 + a.x24 * b.x43 : result.x23;
            result.x33 = (i == 2 && j == 2) ? a.x31 * b.x13 + a.x32 * b.x23 + a.x33 * b.x33 + a.x34 * b.x43 : result.x33;
            result.x43 = (i == 3 && j == 2) ? a.x41 * b.x13 + a.x42 * b.x23 + a.x43 * b.x33 + a.x44 * b.x43 : result.x43;

            result.x14 = (i == 0 && j == 3) ? a.x11 * b.x14 + a.x12 * b.x24 + a.x13 * b.x34 + a.x14 * b.x44 : result.x14;
            result.x24 = (i == 1 && j == 3) ? a.x21 * b.x14 + a.x22 * b.x24 + a.x23 * b.x34 + a.x24 * b.x44 : result.x24;
            result.x34 = (i == 2 && j == 3) ? a.x31 * b.x14 + a.x32 * b.x24 + a.x33 * b.x34 + a.x34 * b.x44 : result.x34;
            result.x44 = (i == 3 && j == 3) ? a.x41 * b.x14 + a.x42 * b.x24 + a.x43 * b.x34 + a.x44 * b.x44 : result.x44;
        }
    }

    return result;
}
Matrix44 d2d_matrix44_add(Matrix44 a
                         ,Matrix44 b
                         )
{
    struct Matrix44 matrix =
    {
        a.x11 + b.x11,
        a.x12 + b.x12,
        a.x13 + b.x13,
        a.x14 + b.x14,
        a.x21 + b.x21,
        a.x22 + b.x22,
        a.x23 + b.x23,
        a.x24 + b.x24,
        a.x31 + b.x31,
        a.x32 + b.x32,
        a.x33 + b.x33,
        a.x34 + b.x34,
        a.x41 + b.x41,
        a.x42 + b.x42,
        a.x43 + b.x43,
        a.x44 + b.x44
    };
    return matrix;
}
Matrix44 d2d_matrix44_orthographic_projection(float l
                                             ,float r
                                             ,float t
                                             ,float b
                                             ,float f
                                             ,float n
                                             )
{
    struct Matrix44 matrix =
    {
        2.0f/(r-l) ,0         ,0          ,-((r+l)/(r-l)),
        0          ,2.0/(t-b) ,0          ,-((t+b)/(t-b)),
        0          ,0         ,-2.0/(f-n) ,-((f+n)/(f-n)),
        0          ,0         ,0          ,1
    };
    return matrix;
}

Vector2f d2d_matrix44_multilpy_vector2f(Vector2f vector
                                       ,Matrix44 transform
                                       )
{
    Vector2f result =
    {
        (vector.x * transform.x11) + (vector.y * transform.x21) + (1 * transform.x31),
        (vector.x * transform.x12) + (vector.y * transform.x22) + (1 * transform.x32)
    };
    return result;
}
Vector4f d2d_matrix44_multiply_vector4f(Matrix44 matrix
                                       ,Vector4f vector
                                       )
{
    Vector4f result = 
    {
        matrix.x11 * vector.x + matrix.x21 * vector.y + matrix.x31 * vector.z + matrix.x41 * vector.w,
        matrix.x12 * vector.x + matrix.x22 * vector.y + matrix.x32 * vector.z + matrix.x42 * vector.w,
        matrix.x13 * vector.x + matrix.x23 * vector.y + matrix.x33 * vector.z + matrix.x43 * vector.w,
        matrix.x14 * vector.x + matrix.x24 * vector.y + matrix.x34 * vector.z + matrix.x44 * vector.w
    };
    return result;
}
void d2d_matrix44_print(const Matrix44* matrix) 
{
    printf("[\n");
    printf("    [ %10.4f, %10.4f, %10.4f, %10.4f ],\n", matrix->x11, matrix->x12, matrix->x13, matrix->x14);
    printf("    [ %10.4f, %10.4f, %10.4f, %10.4f ],\n", matrix->x21, matrix->x22, matrix->x23, matrix->x24);
    printf("    [ %10.4f, %10.4f, %10.4f, %10.4f ],\n", matrix->x31, matrix->x32, matrix->x33, matrix->x34);
    printf("    [ %10.4f, %10.4f, %10.4f, %10.4f ]\n", matrix->x41, matrix->x42, matrix->x43, matrix->x44);
    printf("]\n");
}
float d2d_matrix44_calculate_sub_determinant(Matrix44 m
                                            ,int      a
                                            ,int      b
                                            ,int      c
                                            ,int      d
                                            )
{
    float a11 = m.x11;
    float a12 = m.x12;
    float a13 = m.x13;

    float a21 = m.x21;
    float a22 = m.x22;
    float a23 = m.x23;

    float a31 = m.x31;
    float a32 = m.x32;
    float a33 = m.x33;

    float det = a11 * (a22 * a33 - a23 * a32) -
                a12 * (a21 * a33 - a23 * a31) +
                a13 * (a21 * a32 - a22 * a31);

    return det;
}
float d2d_matrix44_calculate_determinant(const Matrix44 *m)
{
    float a11 = m->x11;
    float a12 = m->x12;
    float a13 = m->x13;
    float a14 = m->x14;

    float a21 = m->x21;
    float a22 = m->x22;
    float a23 = m->x23;
    float a24 = m->x24;

    float a31 = m->x31;
    float a32 = m->x32;
    float a33 = m->x33;
    float a34 = m->x34;

    float a41 = m->x41;
    float a42 = m->x42;
    float a43 = m->x43;
    float a44 = m->x44;

    float det = a11 * (a22 * (a33 * a44 - a34 * a43) - a23 * (a32 * a44 - a34 * a42) + a24 * (a32 * a43 - a33 * a42)) -
                a12 * (a21 * (a33 * a44 - a34 * a43) - a23 * (a31 * a44 - a34 * a41) + a24 * (a31 * a43 - a33 * a41)) +
                a13 * (a21 * (a32 * a44 - a34 * a42) - a22 * (a31 * a44 - a34 * a41) + a24 * (a31 * a42 - a32 * a41)) -
                a14 * (a21 * (a32 * a43 - a33 * a42) - a22 * (a31 * a43 - a33 * a41) + a23 * (a31 * a42 - a32 * a41));

    return det;
}
float *matrix44_to_gl_matrix(Matrix44 *matrix)
{
    return &matrix->x11;
}
uint8_t d2d_matrix44_invert2(Matrix44 *input
                            ,Matrix44 *output
                            ) 
{
    float det;
    float inv[16];
    float m[16] = 
    {
        input->x11, input->x21, input->x31, input->x41,
        input->x12, input->x22, input->x32, input->x42,
        input->x13, input->x23, input->x33, input->x43,
        input->x14, input->x24, input->x34, input->x44
    };

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9]  * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5]  * m[15] + 
             m[8]  * m[7]  * m[13] + 
             m[12] * m[5]  * m[11] - 
             m[12] * m[7]  * m[9];

    inv[12] = -m[4]  * m[9]  * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5]  * m[14] - 
               m[8]  * m[6]  * m[13] - 
               m[12] * m[5]  * m[10] + 
               m[12] * m[6]  * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2]  * m[15] - 
              m[9]  * m[3]  * m[14] - 
              m[13] * m[2]  * m[11] + 
              m[13] * m[3]  * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2]  * m[15] + 
             m[8]  * m[3]  * m[14] + 
             m[12] * m[2]  * m[11] - 
             m[12] * m[3]  * m[10];

    inv[9] = -m[0]  * m[9]  * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1]  * m[15] - 
              m[8]  * m[3]  * m[13] - 
              m[12] * m[1]  * m[11] + 
              m[12] * m[3]  * m[9];

    inv[13] = m[0]  * m[9]  * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1]  * m[14] + 
              m[8]  * m[2]  * m[13] + 
              m[12] * m[1]  * m[10] - 
              m[12] * m[2]  * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0) 
    {
        return 0;
    }

    det = 1.0 / det;

    for (int i = 0; i < 16; i++) 
    {
        inv[i] *= det;
    }

    *output = (Matrix44)
    {
        inv[0], inv[1], inv[2], inv[3],
        inv[4], inv[5], inv[6], inv[7],
        inv[8], inv[9], inv[10], inv[11],
        inv[12], inv[13], inv[14], inv[15]
    };

    return 1;
}
Matrix44 d2d_matrix44_invert(Matrix44 input)
{
    float det = d2d_matrix44_calculate_determinant(&input);

    if (det == 0.0f)
    {
        printf("Matrix is not invertible (determinant is zero).\n");
        return input;
    }

    Matrix44 result = {0};

    result.x11 =  d2d_matrix44_calculate_sub_determinant(input, 1, 2, 3, 1) / det;
    result.x21 = -d2d_matrix44_calculate_sub_determinant(input, 0, 2, 3, 1) / det;
    result.x31 =  d2d_matrix44_calculate_sub_determinant(input, 0, 1, 3, 1) / det;
    result.x41 = -d2d_matrix44_calculate_sub_determinant(input, 0, 1, 2, 1) / det;

    result.x12 = -d2d_matrix44_calculate_sub_determinant(input, 1, 2, 3, 0) / det;
    result.x22 =  d2d_matrix44_calculate_sub_determinant(input, 0, 2, 3, 0) / det;
    result.x32 = -d2d_matrix44_calculate_sub_determinant(input, 0, 1, 3, 0) / det;
    result.x42 =  d2d_matrix44_calculate_sub_determinant(input, 0, 1, 2, 0) / det;

    result.x13 =  d2d_matrix44_calculate_sub_determinant(input, 1, 2, 3, 3) / det;
    result.x23 = -d2d_matrix44_calculate_sub_determinant(input, 0, 2, 3, 3) / det;
    result.x33 =  d2d_matrix44_calculate_sub_determinant(input, 0, 1, 3, 3) / det;
    result.x43 = -d2d_matrix44_calculate_sub_determinant(input, 0, 1, 2, 3) / det;

    result.x14 = -d2d_matrix44_calculate_sub_determinant(input, 1, 2, 3, 2) / det;
    result.x24 =  d2d_matrix44_calculate_sub_determinant(input, 0, 2, 3, 2) / det;
    result.x34 = -d2d_matrix44_calculate_sub_determinant(input, 0, 1, 3, 2) / det;
    result.x44 =  d2d_matrix44_calculate_sub_determinant(input, 0, 1, 2, 2) / det;

    return result;
}
void d2d_map_mouse_to_ortho_space(float mouse_x
                                 ,float mouse_y
                                 ,int screen_width
                                 ,int screen_height
                                 ,Camera2D* camera
                                 ,float* orthoX
                                 ,float* orthoY
                                 )
{
    float ndc_x = (2.0f * mouse_x / screen_width) - 1.0f;
    float ndc_y = 1.0f - (2.0f * mouse_y / screen_height);

    Vector2f ndc_position = { ndc_x, ndc_y};

    Matrix44 inverse_projection = d2d_matrix44_invert(camera->projection);

    Vector2f orthographic_position = d2d_matrix44_multilpy_vector2f(ndc_position,inverse_projection);

    *orthoX = orthographic_position.x;
    *orthoY = orthographic_position.y;
}
// ================================
// Graphics context functions
// ================================
int8_t d2d_context_init(D2DContext*     context
                       ,uint16_t        width
                       ,uint16_t        height
                       ,char*           window_title
                       )
{
    if (!glfwInit())
    {
        printf("GLFW initialization failed!");
        glfwTerminate();
        return DELO_ERROR;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    context->window = glfwCreateWindow(width, height, window_title, NULL, NULL);

    int left, top, right, bottom;
    glfwGetWindowFrameSize(context->window, &left, &top, &right, &bottom);

    context->screen_width  = width  + left + right;
    context->screen_height = height + top  + bottom;

    if (!context->window)
    {
        printf("GLFW window creation failed!");
        glfwTerminate();
        return DELO_ERROR;
    }

    glfwMakeContextCurrent(context->window);

    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    
    glewExperimental = GL_TRUE;
    
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialization failed!");
        glfwDestroyWindow(context->window);
        glfwTerminate();
        return DELO_ERROR;
    }

    int buffer_width, buffer_height;
    glfwGetFramebufferSize(context->window, &buffer_width, &buffer_height);
    glViewport(0, 0, buffer_width, buffer_height);

    context->back_buffer_width = buffer_width;
    context->back_buffer_height = buffer_height;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    context->t0 = glfwGetTime();
    context->t = 0;

}
void d2d_frame_begin(D2DContext *context)
{
    glViewport(0, 0, context->back_buffer_width, context->back_buffer_height);
    context->t1 = glfwGetTime();
    context->dt = context->t1 - context->t0;
}
void d2d_frame_end(D2DContext *context)
{
    context->glfw_callback_data.key_buffer[0] = '\0';
    context->glfw_callback_data.key_buffer_length = 0;

    context->hid_state_prev = context->hid_state;
    context->t += 0.1f;
    glfwSwapBuffers(context->window);
    context->t0 = context->t1;

    glfwPollEvents();
}
// ================================
// Human Input Device functions
// ================================
void d2d_hid_control_init(HidState*   hid_state
                         ,GLFWwindow* window
                         )
{
    hid_state->mouse_button_left = GLFW_RELEASE;
    hid_state->mouse_button_right = GLFW_RELEASE;
    hid_state->mouse_button_middle = GLFW_RELEASE;

    glfwGetCursorPos(window, &hid_state->mouse_position_x, &hid_state->mouse_position_y);

    int fbx,fby;
    glfwGetFramebufferSize(window, &fbx, &fby);
    int wx,wy;
    glfwGetWindowSize(window,&wx,&wy);

    float mpx = (float)hid_state->mouse_position_x/(float)wx;
    float mpy = (float)hid_state->mouse_position_y/(float)wy;

    hid_state->mouse_position_x = mpx * fbx;
    hid_state->mouse_position_y = mpy * fby;


}

void d2d_hid_control_update(HidState*   hid_state
                           ,GLFWwindow* window
                           ,int         window_width
                           ,int         window_height
                           ,int         back_buffer_width
                           ,int         back_buffer_height
                           )
{
    hid_state->mouse_button_left   = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    hid_state->mouse_button_right  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    hid_state->mouse_button_middle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

    glfwGetCursorPos(window, &hid_state->mouse_position_x, &hid_state->mouse_position_y);

    hid_state->key_escape      = glfwGetKey(window, GLFW_KEY_ESCAPE);
    hid_state->key_backspace   = glfwGetKey(window, GLFW_KEY_BACKSPACE);
    hid_state->key_enter       = glfwGetKey(window, GLFW_KEY_ENTER);
    hid_state->key_arrow_left  = glfwGetKey(window, GLFW_KEY_LEFT);
    hid_state->key_arrow_right = glfwGetKey(window, GLFW_KEY_RIGHT);
    hid_state->key_ctrl_left   = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    hid_state->key_c           = glfwGetKey(window, GLFW_KEY_C);
    hid_state->key_v           = glfwGetKey(window, GLFW_KEY_V);
    hid_state->key_a           = glfwGetKey(window, GLFW_KEY_A);
    hid_state->key_tab         = glfwGetKey(window, GLFW_KEY_TAB);
    hid_state->key_shift_left  = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
}

void d2d_append_utf8(uint32_t  codepoint
                    ,char*     buffer
                    ,uint16_t* index
                    )
{
    if (codepoint <= 0x7F)
    {
        buffer[(*index)++] = (char)codepoint;
    }
    else if (codepoint <= 0x7FF)
    {
        buffer[(*index)++] = (char)((codepoint >> 6) | 0xC0);
        buffer[(*index)++] = (char)((codepoint & 0x3F) | 0x80);
    }
    else if (codepoint <= 0xFFFF)
    {
        buffer[(*index)++] = (char)((codepoint >> 12) | 0xE0);
        buffer[(*index)++] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        buffer[(*index)++] = (char)((codepoint & 0x3F) | 0x80);
    }
    else
    {
        buffer[(*index)++] = (char)((codepoint >> 18) | 0xF0);
        buffer[(*index)++] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        buffer[(*index)++] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        buffer[(*index)++] = (char)((codepoint & 0x3F) | 0x80);
    }
}
void d2d_callback_character(GLFWwindow*  window
                           ,unsigned int codepoint
                           )
{
    GlfwCallbackData *glfw_callback_data = (GlfwCallbackData *)(glfwGetWindowUserPointer(window));
    d2d_append_utf8(codepoint, &glfw_callback_data->key_buffer[0], &glfw_callback_data->key_buffer_length);
}
// ================================
// Sprite functions
// ================================
void d2d_sprite_define(Sprite*     sprite
                      ,float       width
                      ,float       height
                      ,Rectangle_f src_rect
                      )
{
    sprite->width      = width;
    sprite->height     = height;
    sprite->position.x = 0;
    sprite->position.y = 0;
    sprite->src_rect   = src_rect;
    sprite->offset     = (Vector2f){0,0};
    sprite->color      = (Color){1,1,1,1};
}
void d2d_sprite_animation_init(SpriteAnimation* sprite_animation
                              ,float            duration
                              ,uint16_t         frames
                              ,uint16_t         stride
                              ,uint8_t          loop
                              )
{
    sprite_animation->time     = 0;
    sprite_animation->duration = duration;
    sprite_animation->frame    = 0;
    sprite_animation->frames   = frames;
    sprite_animation->stride   = stride;
    sprite_animation->loop     = loop;
}
void d2d_sprite_animate_frame(Sprite*          sprite
                             ,SpriteAnimation* animation
                             ,int              frame
                             )
{
    if(frame >= (int)animation->frames)
    {
        frame = 0;
    }
    else if (frame < 0)
    {
        frame = animation->frames - 1;
    }
    animation->frame   = frame;
    animation->time    = (float)animation->frame / animation->duration;
    sprite->src_rect.x = sprite->offset.x + (animation->frame % animation->stride) * sprite->src_rect.width;
    sprite->src_rect.y = sprite->offset.y + (animation->frame / animation->stride) * sprite->src_rect.height;
}
void d2d_sprite_animate(Sprite*          sprite
                       ,SpriteAnimation* animation
                       ,float            dt
                       ,unsigned int     reverse
                       )
{
    if (!reverse)
    {
        animation->time += dt;

        if (animation->time > animation->duration)
        {
            if (animation->loop == 1)
            {
                animation->time = 0;
            }
            else
            {
                return;
            }
        }
    }
    else
    {
        animation->time -= dt;
        if (animation->time < 0)
        {
            if (animation->loop == 1)
            {
                animation->time = animation->duration;
            }
            else
            {
                return;
            }
        }
    }

    animation->frame   = (animation->time / animation->duration) * (float)animation->frames;
    animation->frame   = (animation->frame == animation->frames) ? animation->frames - 1 : animation->frame;

    sprite->src_rect.x = sprite->offset.x + (animation->frame % animation->stride) * sprite->src_rect.width;
    sprite->src_rect.y = sprite->offset.y + (animation->frame / animation->stride) * sprite->src_rect.height;
}
void d2d_sprite_transform(uint32_t  width
                         ,uint32_t  height
                         ,Matrix44* transform
                         ,Vector2f  scale
                         ,Vector2f  skew
                         ,float     rotation
                         )
{
    Matrix44 matrix          = d2d_matrix44_scale(width / 2, height / 2, 1);
    Matrix44 matrix_scale    = d2d_matrix44_scale(scale.x, scale.y, 1);
    Matrix44 matrix_skew     = d2d_matrix44_skew(skew.x, skew.y);
    Matrix44 matrix_rotation = d2d_matrix44_rotation_z(rotation);

    matrix = d2d_matrix44_multiply(matrix, matrix_scale);
    matrix = d2d_matrix44_multiply(matrix, matrix_skew);
    matrix = d2d_matrix44_multiply(matrix, matrix_rotation);

    *transform = matrix;
}

// ================================
// Render Target functions
// ================================

int8_t d2d_render_target_init(RenderTarget *rt,float screen_width,float screen_height, float x, float y, float width, float height)
{
    rt->projection = d2d_matrix44_orthographic_projection(0.0f,(float)width,(float)height,0.0f,1,-1);

    y = -y;
    y += (screen_height-height);

    float quad_x = ((2*x)/screen_width  -1);
    float quad_y = ((2*y)/screen_height  -1);

    float quad_w = ((2*(width +x)) /screen_width  -1);
    float quad_h = ((2*(height+y)) /screen_height  -1);


    //delo2d_render_target_default.glsl
    //position.X                 position.Y                    tex_coord.x                   tex_coord.y                     
    rt->vertices[0] = quad_w;    rt->vertices[1] = quad_y;     rt->vertices[2]  = 1.0f;    rt->vertices[3]  = 1.0f;
    rt->vertices[4] = quad_x;    rt->vertices[5] = quad_y;     rt->vertices[6]  = 0.0f;    rt->vertices[7]  = 1.0f;
    rt->vertices[8] = quad_x;    rt->vertices[9] = quad_h;     rt->vertices[10] = 0.0f;    rt->vertices[11] = 0.0f;

    rt->vertices[12] = quad_w;   rt->vertices[13] = quad_h;    rt->vertices[14] = 1.0f;    rt->vertices[15] = 0.0f;
    rt->vertices[16] = quad_w;   rt->vertices[17] = quad_y;    rt->vertices[18] = 1.0f;    rt->vertices[19] = 1.0f;
    rt->vertices[20] = quad_x;   rt->vertices[21] = quad_h;    rt->vertices[22] = 0.0f;    rt->vertices[23] = 0.0f;

	glGenVertexArrays(1, &rt->vao);
	glGenBuffers(1, &rt->vbo);

	glBindVertexArray(rt->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rt->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rt->vertices), &rt->vertices, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenFramebuffers(1, &rt->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);

	glGenTextures(1, &rt->fbt);
    glBindTexture(GL_TEXTURE_2D, rt->fbt);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->fbt, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    rt->texture.renderer_id = rt->fbt;
    rt->texture.width = width;
    rt->texture.height = height;
}
// ================================
// Shader functions
// ================================
void d2d_delo2d_shader_check_compile_status(GLuint shader)
{
    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE)
    {
        GLint infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (infoLogLength > 0)
        {
            char *infoLog = (char *)malloc(infoLogLength);
            if (infoLog)
            {
                glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
                printf("Shader compile error: %s\n", infoLog);
                free(infoLog);
            }
            else
            {
                printf("Failed to allocate memory for shader compile log\n");
            }
        }
        else
        {
            printf("Shader compile error: (no additional information available)\n");
        }
    }
    else
    {
        //printf("Shader compiled successfully.\n");
    }
}
uint8_t d2d_shader_compile(uint32_t  type
                                 ,char*     shader_source_code
                                 ,uint32_t* id
                                 )
{
    gl_clear_error();

    if (shader_source_code == NULL)
    {
        fprintf(stderr, "Error: Shader source code is NULL\n");
        return DELO_ERROR;
    }

    *id = glCreateShader(type);
    if (*id == 0)
    {
        fprintf(stderr, "Error creating shader\n");
        return DELO_ERROR;
    }

    const char *src = shader_source_code;
    glShaderSource(*id, 1, &src, NULL);

    glCompileShader(*id);

    GLint compile_status;

    glGetShaderiv(*id, GL_COMPILE_STATUS, &compile_status);

    d2d_delo2d_shader_check_compile_status(*id);

    if (compile_status == GL_FALSE)
    {
        fprintf(stderr, "Shader failed to compile\n");
        glDeleteShader(*id);
        *id = 0;
        return DELO_ERROR;
    }

    return DELO_SUCCESS;
}
uint8_t d2d_shader_create(char*     source_code_shader_vertex
                         ,char*     source_code_shader_fragment
                         ,uint32_t* program
                         )
{
    gl_clear_error();

    *program = glCreateProgram();

    if (*program == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        return DELO_ERROR;
    }

    uint32_t vs;
    uint32_t fs;

    if (d2d_shader_compile(GL_VERTEX_SHADER, source_code_shader_vertex, &vs) == DELO_ERROR)
    {
        printf("Could not compiler vertex shader.\n");
        glDeleteProgram(*program);
        return DELO_ERROR;
    }
    if (d2d_shader_compile(GL_FRAGMENT_SHADER, source_code_shader_fragment, &fs) == DELO_ERROR)
    {
        printf("Could not compiler fragment shader.\n");
        glDeleteProgram(*program);
        return DELO_ERROR;
    }

    glAttachShader(*program, vs);
    glAttachShader(*program, fs);

    glLinkProgram(*program);
    glValidateProgram(*program);

    GLint link_status;
    glGetProgramiv(*program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        fprintf(stderr, "Shader program linking failed.\n");

        glDeleteProgram(*program);
        return DELO_ERROR;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return DELO_SUCCESS;
}
int8_t d2d_load_file(const char* path
                    ,char**      source_code
                    )
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", path);
        return DELO_ERROR;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    *source_code = (char *)malloc(fsize + 1);
    if (*source_code == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        fclose(f);
        return DELO_ERROR;
    }

    fread(*source_code, fsize, 1, f);
    fclose(f);
    (*source_code)[fsize] = '\0';

    return DELO_SUCCESS;
}
int8_t d2d_shader_load(char*     path_shader_vert
                      ,char*     path_shader_frag
                      ,uint32_t* shader_id
                      )
{
    *shader_id = 0;
    char *source_code_vert;
    char *source_code_frag;

    if (d2d_load_file(path_shader_vert, &source_code_vert) == DELO_ERROR)
    {
        printf("[delo2d] Could not load shader file %s\n", path_shader_vert);
        return DELO_ERROR;
    }
    if (d2d_load_file(path_shader_frag, &source_code_frag) == DELO_ERROR)
    {
        printf("[delo2d] Could not load shader file %s\n", path_shader_frag);
        return DELO_ERROR;
    }

    uint8_t status = DELO_SUCCESS;

    if (d2d_shader_create(source_code_vert, source_code_frag, shader_id) == DELO_ERROR)
    {
        if (source_code_vert != NULL)
            free(source_code_vert);
        if (source_code_frag != NULL)
            free(source_code_frag);
        return DELO_ERROR;
    }
    else
    {
        return DELO_SUCCESS;
    }
}
// ================================
// Texture functions
// ================================
int8_t d2d_texture_load(Texture* texture
                       ,char     file_path[]
                       )
{
    stbi_set_flip_vertically_on_load(0);
    texture->local_buffer = stbi_load(file_path
                                     ,&texture->width
                                     ,&texture->height
                                     ,&texture->bytes_per_pixel
                                     ,4
                                     );

    if (texture->local_buffer == NULL)
    {
        fprintf(stderr, "Error loading texture: %s\n", file_path);
        return DELO_ERROR;
    }

    glGenTextures(1, &texture->renderer_id);
    if (texture->renderer_id == 0)
    {
        fprintf(stderr, "Error generating texture ID\n");
        stbi_image_free(texture->local_buffer);
        return DELO_ERROR;
    }

    glBindTexture(GL_TEXTURE_2D, texture->renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D
                ,0
                ,GL_RGBA8
                ,texture->width
                ,texture->height
                ,0
                ,GL_RGBA
                ,GL_UNSIGNED_BYTE
                ,texture->local_buffer
                );

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texture->local_buffer);

    texture->initialized = 1;
    return DELO_SUCCESS;
}
// ================================
// Rectange functions
// ================================
int8_t d2d_rectangle_within_bounds(Rectangle_f* r
                                  ,int          x
                                  ,int          y
                                  )
{
    return (x > r->x && x < r->x + r->width) && (y > r->y && y < r->y + r->height);
}
// ================================
// Math functions
// ================================
float d2d_math_radians(float degrees) 
{
    return degrees * (float)(3.14) / 180.0f;
}
float d2d_math_distance(Vector2f p1
                       ,Vector2f p2
                       ) 
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx*dx+dy*dy);
}
// ================================
// Camera2D functions
// ================================
int8_t d2d_camera2d_init(Camera2D*       camera
                        ,D2DContext*     context
                        ,float           width
                        ,float           height
                        )
{
    camera->view_width_in_world_units = width;      
    camera->view_height_in_world_units = width * (height / width);

    float l = -camera->view_width_in_world_units  / 2;
    float r =  camera->view_width_in_world_units  / 2;
    float b =  camera->view_height_in_world_units / 2;
    float t = -camera->view_height_in_world_units / 2;

    float f = (float)1;
    float n = (float)-1;

    camera->projection = d2d_matrix44_orthographic_projection(l,r,t,b,f,n);


    camera->width = width;
    camera->height = height;

    camera->position = (Vector2f){0, 0};

    camera->view = d2d_matrix44_identity();

    return DELO_SUCCESS;
}
void d2d_camera2d_update(Camera2D *camera)
{
    camera->view_projection = d2d_matrix44_multiply(camera->view,camera->projection);
}
void d2d_camera2d_move(Camera2D* camera
                      ,float     tx
                      ,float     ty
                      )
{
    camera->position.x += tx/camera->view_width_in_world_units;
    camera->position.y += ty/camera->view_height_in_world_units;

    Matrix44 translation = d2d_matrix44_translation(-tx, -ty, 0);

    camera->view = d2d_matrix44_multiply(translation, camera->view);
}
void d2d_camera2d_zoom(Camera2D* camera
                      ,float     zoom_factor
                      )
{
    float half_width = camera->width / 2.0f;
    float half_height = camera->height / 2.0f;

    float l = -half_width / zoom_factor;
    float r = half_width / zoom_factor;
    float b = -half_height / zoom_factor;
    float t = half_height / zoom_factor;

    camera->projection = d2d_matrix44_orthographic_projection(l, r, b, t, 1.0f, -1.0f);

    camera->view_projection = d2d_matrix44_multiply(camera->projection, camera->view);
}
void d2d_camera2d_rotate(Camera2D* camera
                        ,float     t
                        )
{
    Matrix44 transformation = d2d_matrix44_rotation_z(t);

    Matrix44 translation_0 = d2d_matrix44_translation(-camera->position.x, -camera->position.y, 0);
    Matrix44 translation_1 = d2d_matrix44_translation(camera->position.x, camera->position.y, 0);

    camera->view = d2d_matrix44_multiply(camera->view, translation_0);
    camera->view = d2d_matrix44_multiply(camera->view, transformation);
    camera->view = d2d_matrix44_multiply(camera->view, translation_1);
}
// ================================
// Color functions
// ================================
void d2d_color_set_f(Color* color
                    ,float  r
                    ,float  g
                    ,float  b
                    ,float  a
                    )
{
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}
void d2d_color_set_i(Color* color
                    ,int    r
                    ,int    g
                    ,int    b
                    ,int    a
                    )
{
    color->r = (float)r / 255.0f;
    color->g = (float)g / 255.0f;
    color->b = (float)b / 255.0f;
    color->a = (float)a / 255.0f;
}
void d2d_color_set(Color *color1
, Color *color2)
{
    color1->r = color2->r;
    color1->g = color2->g;
    color1->b = color2->b;
    color1->a = color2->a;
}
void d2d_color_lerp(Color* result
                   ,Color* color_a
                   ,Color* color_b
                   ,float  facor
                   )
{
    result->r = (color_a->r + color_b->r) / 2;
    result->g = (color_a->g + color_b->g) / 2;
    result->b = (color_a->b + color_b->b) / 2;
    result->a = (color_a->a + color_b->a) / 2;
}
// ================================
// Renderer Circle functions
// ================================
int8_t d2d_renderer_circle_init(RendererCircle*    renderer
                                  ,uint32_t           capacity
                                  ,D2DContext*    context
                                  )
{
    renderer->context = context;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return DELO_ERROR;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo_vertices);
    glGenBuffers(1, &renderer->vbo_colors);
    glGenBuffers(1, &renderer->vbo_positions);
    glGenBuffers(1, &renderer->vbo_radii);



    GLfloat vertices[] =
    {
        // positions     // texCoords
        -1.0f, -1.0f, // bottom-left
        1.0f, -1.0f,  // bottom-right
        1.0f, 1.0f,   // top-right
        -1.0f, 1.0f,   // top-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_colors);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Color), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_positions);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2f), (void *)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_radii);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);


    glBindVertexArray(0);
    glUseProgram(0);

    renderer->colors = malloc(sizeof(Color) * capacity);
    renderer->positions = malloc(sizeof(Vector2f) * capacity);
    renderer->radii = malloc(sizeof(float) * capacity);

    renderer->capacity = capacity;
    renderer->count = 0;

    renderer->projection = d2d_matrix44_orthographic_projection((float)0.0f, (float)context->back_buffer_width, (float)0.0f, (float)context->back_buffer_height, (float)1, (float)-1);
    renderer->projection_default = renderer->projection;
}
int8_t d2d_renderer_circle_apply_shader(RendererCircle* renderer
                                      ,uint32_t           shader
                                      )
{
    renderer->shader = shader;
    renderer->shader_default = shader;

    glUseProgram(shader);
    renderer->uniform_location_u_mvp = glGetUniformLocation(shader, "u_mvp");
    renderer->uniform_location_u_bbh = glGetUniformLocation(shader, "u_back_buffer_height");
    glUseProgram(0);
}
int8_t d2d_renderer_circle_update(RendererCircle* renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * renderer->count, (float *)renderer->colors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * renderer->count, (float *)renderer->positions, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_radii);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderer->count, (float *)renderer->radii, GL_STATIC_DRAW);
}

int8_t d2d_renderer_circle_render(RendererCircle* renderer
                                ,Matrix44*          projection
                                ,uint8_t            flip
                                )
{

    /*------------------Draw instances-----------------*/
    glBindVertexArray(renderer->vao);

    glUseProgram(renderer->shader);
    glUniformMatrix4fv(renderer->uniform_location_u_mvp, 1, GL_FALSE, &projection->x11);

    glUniform1f(renderer->uniform_location_u_bbh,(float)renderer->context->back_buffer_height);

    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, renderer->count);

    glBindVertexArray(0);
    glUseProgram(0);
    /*------------------Draw instances-----------------*/
}
int8_t d2d_renderer_circle_add(RendererCircle* renderer
                                    ,Vector2f        position
                                    ,Color           color
                                    ,float           radius
                                    )
{

    int32_t index = renderer->count;

    if (index < renderer->capacity)
    {
        renderer->positions[index].x    = position.x;
        renderer->positions[index].y    = position.y;
        renderer->colors[index] = color;
        renderer->radii[index] = radius;
        renderer->count++;
    }
}

int8_t d2d_renderer_circle_begin(RendererCircle* renderer
                                      ,Matrix44*          projection
                                      ,GLuint*            shader
                                      )
{
    renderer->count = 0;
    renderer->projection = (projection == NULL) ? renderer->projection : *projection;
    renderer->shader = (shader == NULL) ? renderer->shader : *shader;
}
int8_t d2d_renderer_circle_end(RendererCircle* renderer)
{
    d2d_renderer_circle_update(renderer);
    d2d_renderer_circle_render(renderer, &renderer->projection, 0);
    renderer->shader = renderer->shader_default;
    renderer->projection = renderer->projection_default;
}


// ================================
// Renderer Primitive functions
// ================================
int8_t d2d_renderer_primitive_init(RendererPrimitive* renderer
                                  ,uint32_t           capacity
                                  ,D2DContext*        context
                                  )
{
    renderer->context = context;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return DELO_ERROR;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vbo_vertices);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_vertices);

    renderer->vertices = malloc(sizeof(PrimitiveVertex) * capacity);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PrimitiveVertex) * capacity, renderer->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glUseProgram(0);

    renderer->capacity = capacity;
    renderer->count = 0;

    renderer->projection = d2d_matrix44_orthographic_projection((float)0.0f
                                                               ,(float)context->back_buffer_width
                                                               ,(float)0.0f
                                                               ,(float)context->back_buffer_height
                                                               ,(float)1
                                                               ,(float)-1
                                                               );
    renderer->projection_default = renderer->projection;
}
int8_t d2d_renderer_primitive_apply_shader(RendererPrimitive* renderer
                                          ,uint32_t           shader
                                          )
{
    renderer->shader = shader;
    renderer->shader_default = shader;

    glUseProgram(shader);
    renderer->uniform_location_u_mvp = glGetUniformLocation(shader, "u_mvp");
    glUseProgram(0);
}
int8_t d2d_renderer_primitive_update(RendererPrimitive* renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RendererPrimitive) * renderer->count, (float *)renderer->vertices, GL_STATIC_DRAW);
}
int8_t d2d_renderer_primitive_render(RendererPrimitive* renderer
                                    ,Matrix44*          projection
                                    ,uint8_t            flip
                                    )
{

    /*------------------Draw instances-----------------*/
    glBindVertexArray(renderer->vao);

    glUseProgram(renderer->shader);
    glUniformMatrix4fv(renderer->uniform_location_u_mvp, 1, GL_FALSE, &projection->x11);

    switch (renderer->type)
    {
    case DELO_TRIANGLE_LIST:
        glDrawArrays(GL_TRIANGLES, 0, renderer->count);
        break;
    case DELO_LINE_LIST:
        glDrawArrays(GL_LINES, 0, renderer->count);
        break;
    }

    glBindVertexArray(0);
    glUseProgram(0);
    /*------------------Draw instances-----------------*/
}
int8_t d2d_renderer_primitive_add(RendererPrimitive* renderer
                                 ,Vector2f           position
                                 ,Color              color
                                 )
{

    int32_t index = renderer->count;

    if (index < renderer->capacity)
    {
        renderer->vertices[index].position = position;
        renderer->vertices[index].color = color;
        renderer->count++;
    }
}
int8_t d2d_renderer_primitive_add_line(RendererPrimitive* renderer
                                      ,Vector2f           position_a
                                      ,Vector2f           position_b
                                      ,Color              color
                                      )
{

    int32_t index = renderer->count;

    if (index +2 < renderer->capacity)
    {
        renderer->vertices[index].position = position_a;
        renderer->vertices[index].color    = color;

        renderer->vertices[index+1].position = position_b;
        renderer->vertices[index+1].color    = color;
        renderer->count+=2;
    }
}
int8_t d2d_renderer_primitive_add_rectangle(RendererPrimitive* renderer
                                           ,Rectangle_f        rectangle
                                           ,Color              color
                                           )
{

    int32_t index = renderer->count;

    if (index + 6 <= renderer->capacity)
    {
        renderer->vertices[index + 0].position = (Vector2f){rectangle.x, rectangle.y};
        renderer->vertices[index + 1].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y};
        renderer->vertices[index + 2].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y + rectangle.height};
        renderer->vertices[index + 3].position = (Vector2f){rectangle.x, rectangle.y};
        renderer->vertices[index + 4].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y + rectangle.height};
        renderer->vertices[index + 5].position = (Vector2f){rectangle.x, rectangle.y + rectangle.height};

        renderer->vertices[index + 0].color = color;
        renderer->vertices[index + 1].color = color;
        renderer->vertices[index + 2].color = color;
        renderer->vertices[index + 3].color = color;
        renderer->vertices[index + 4].color = color;
        renderer->vertices[index + 5].color = color;

        renderer->count += 6;
    }
}
int8_t d2d_renderer_primitive_add_rectangle_outline(RendererPrimitive* renderer
                                                   ,Rectangle_f        rectangle
                                                   ,Color              color
                                                   )
{
    int32_t index = renderer->count;

    if (index + 8 <= renderer->capacity)
    {
        renderer->vertices[index + 0].position = (Vector2f){rectangle.x, rectangle.y};
        renderer->vertices[index + 1].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y};

        renderer->vertices[index + 2].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y};
        renderer->vertices[index + 3].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y + rectangle.height};

        renderer->vertices[index + 4].position = (Vector2f){rectangle.x + rectangle.width, rectangle.y + rectangle.height};
        renderer->vertices[index + 5].position = (Vector2f){rectangle.x, rectangle.y + rectangle.height};

        renderer->vertices[index + 6].position = (Vector2f){rectangle.x, rectangle.y + rectangle.height};
        renderer->vertices[index + 7].position = (Vector2f){rectangle.x, rectangle.y};

        renderer->vertices[index + 0].color = color;
        renderer->vertices[index + 1].color = color;
        renderer->vertices[index + 2].color = color;
        renderer->vertices[index + 3].color = color;
        renderer->vertices[index + 4].color = color;
        renderer->vertices[index + 5].color = color;
        renderer->vertices[index + 6].color = color;
        renderer->vertices[index + 7].color = color;

        renderer->count += 8;
    }
}
int8_t d2d_renderer_primitive_begin(RendererPrimitive* renderer
                                   ,Matrix44*          projection
                                   ,GLuint*            shader
                                   ,uint8_t            type
                                   )
{
    renderer->count = 0;
    renderer->projection = (projection == NULL) ? renderer->projection : *projection;
    renderer->shader = (shader == NULL) ? renderer->shader : *shader;
    renderer->type = type;
}
int8_t d2d_renderer_primitive_end(RendererPrimitive* renderer)
{
    d2d_renderer_primitive_update(renderer);
    d2d_renderer_primitive_render(renderer, &renderer->projection, 0);
    renderer->shader = renderer->shader_default;
    renderer->projection = renderer->projection_default;
}
// ================================
// Renderer Sprite functions
// ================================
int8_t d2d_renderer_sprite_init(RendererSprite* renderer
                               ,uint32_t        capacity
                               ,D2DContext*     context
                               )
{

    renderer->context = context;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return DELO_ERROR;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    GLfloat vertices[] =
    {
        //positions         //texCoords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top-left
    };

    glGenBuffers(1, &renderer->vbo_vertices);
    glGenBuffers(1, &renderer->vbo_colors);
    glGenBuffers(1, &renderer->vbo_transforms);
    glGenBuffers(1, &renderer->vbo_offsets);
    glGenBuffers(1, &renderer->vbo_src_rects);
    glGenBuffers(1, &renderer->vbo_tex_indices);
    glGenBuffers(1, &renderer->vbo_limit_y);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_colors);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_offsets);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_src_rects);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_transforms);
    for (int32_t i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(5 + i);
        glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void *)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(5 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_tex_indices);
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
    glEnableVertexAttribArray(9);
    glVertexAttribDivisor(9, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_limit_y);
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE,  2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(10);
    glVertexAttribDivisor(10, 1);

    glBindVertexArray(0);
    glUseProgram(0);

    renderer->capacity = capacity;
    renderer->count    = 0;

    renderer->colors          = malloc(sizeof(Color)       * capacity);
    renderer->transforms      = malloc(sizeof(Matrix44)    * capacity);
    renderer->offsets         = malloc(sizeof(Vector2f)    * capacity);
    renderer->src_rects       = malloc(sizeof(Rectangle_f) * capacity);
    renderer->texture_indices = malloc(sizeof(float)       * capacity);
    renderer->limit_ys        = malloc(sizeof(Vector2f)    * capacity);

    renderer->change_mask = 0b11111111;

    renderer->texture = NULL;

    renderer->projection = d2d_matrix44_orthographic_projection((float)0.0f
                                                               ,(float)context->back_buffer_width
                                                               ,(float)0.0f
                                                               ,(float)context->back_buffer_height
                                                               ,(float)1
                                                               ,(float)-1
                                                               );

    renderer->texture_id_0 = -1;
    renderer->texture_id_1 = -1;
    renderer->texture_id_2 = -1;
    renderer->texture_id_3 = -1;

    renderer->flip = 0;
}
int8_t d2d_renderer_sprite_apply_shader(RendererSprite* renderer
                                       ,uint32_t        shader
                                       )
{
    renderer->shader = shader;

    glUseProgram(shader);
    renderer->uniform_location_u_texture0 = glGetUniformLocation(shader, "u_texture0");
    renderer->uniform_location_u_texture1 = glGetUniformLocation(shader, "u_texture1");
    renderer->uniform_location_u_texture2 = glGetUniformLocation(shader, "u_texture2");
    renderer->uniform_location_u_texture3 = glGetUniformLocation(shader, "u_texture3");
    renderer->uniform_location_u_mvp      = glGetUniformLocation(shader, "u_mvp");
    renderer->uniform_location_u_flip     = glGetUniformLocation(shader, "u_flip");

    glUniform1i(renderer->uniform_location_u_flip, 0);
    glUseProgram(0);
}
int8_t d2d_renderer_sprite_update(RendererSprite* renderer)
{
    if ((renderer->change_mask >> 0) & 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_colors);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * renderer->count, (float *)renderer->colors, GL_STATIC_DRAW);
        renderer->change_mask &= ~(0 << 0);
    }

    if ((renderer->change_mask >> 1) & 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_transforms);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix44) * renderer->count, (float *)renderer->transforms, GL_STATIC_DRAW);
        renderer->change_mask &= ~(1 << 0);
    }

    if ((renderer->change_mask >> 2) & 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_offsets);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * renderer->count, (float *)renderer->offsets, GL_STATIC_DRAW);
        renderer->change_mask &= ~(2 << 0);
    }

    if ((renderer->change_mask >> 3) & 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_src_rects);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Rectangle_f) * renderer->count, (float *)renderer->src_rects, GL_STATIC_DRAW);
        renderer->change_mask &= ~(3 << 0);
    }
    if ((renderer->change_mask >> 4) & 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_tex_indices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderer->count, (float *)renderer->texture_indices, GL_STATIC_DRAW);
        renderer->change_mask &= ~(4 << 0);
    }

    if ((renderer->change_mask >> 5) & 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_limit_y);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * renderer->count, (Vector2f *)renderer->limit_ys, GL_STATIC_DRAW);
        renderer->change_mask &= ~(5 << 0);
    }
}
int8_t d2d_renderer_sprite_render(RendererSprite* renderer)
{
    /*------------------Draw instances-----------------*/
    glBindVertexArray(renderer->vao);

    glUseProgram(renderer->shader);
    if (renderer->texture_id_0 != -1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_0);
        glUniform1i(renderer->uniform_location_u_texture0, 0);
    }

    if (renderer->texture_id_1 != -1)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_1);
        glUniform1i(renderer->uniform_location_u_texture1, 1);
    }

    if (renderer->texture_id_2 != -1)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_2);
        glUniform1i(renderer->uniform_location_u_texture2, 2);
    }

    if (renderer->texture_id_3 != -1)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_3);
        glUniform1i(renderer->uniform_location_u_texture3, 3);
    }

    Matrix44 *projection = &renderer->projection;
    glUniformMatrix4fv(renderer->uniform_location_u_mvp, 1, GL_FALSE, &projection->x11);

    glUniform1i(renderer->uniform_location_u_flip, renderer->flip);

    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, renderer->count);

    if (renderer->texture_id_3 != -1)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (renderer->texture_id_2 != -1)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (renderer->texture_id_1 != -1)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (renderer->texture_id_0 != -1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
    /*------------------Draw instances-----------------*/
}
int8_t d2d_renderer_sprite_add_texture(RendererSprite* renderer
                                      ,int32_t         texture_id
                                      )
{
    if (renderer->texture_id_0 == -1 || renderer->texture_id_0 == texture_id)
    {
        renderer->texture_id_0 = texture_id;
        return 0;
    }
    else if (renderer->texture_id_1 == -1 || renderer->texture_id_1 == texture_id)
    {
        renderer->texture_id_1 = texture_id;
        return 1;
    }
    else if (renderer->texture_id_2 == -1 || renderer->texture_id_2 == texture_id)
    {
        renderer->texture_id_2 = texture_id;
        return 2;
    }
    else if (renderer->texture_id_3 == -1 || renderer->texture_id_3 == texture_id)
    {
        renderer->texture_id_3 = texture_id;
        return 3;
    }
    else
    {
        return -1;
    }
}
int8_t d2d_renderer_sprite_add2(RendererSprite* renderer
                               ,Sprite*         sprite
                               ,Texture*        texture
                               )
{
    int32_t texture_index = d2d_renderer_sprite_add_texture(renderer, texture->renderer_id);

    if (texture_index == -1)
        return DELO_ERROR;

    int32_t index = renderer->count;

    if (index < renderer->capacity)
    {
        uint16_t texture_width  = (texture == NULL) ? 0.0 : texture->width;
        uint16_t texture_height = (texture == NULL) ? 0.0 : texture->height;

        renderer->colors[index]           = sprite->color;
        renderer->transforms[index]       = d2d_matrix44_scale(sprite->width * 0.5, sprite->height * 0.5, 1);
        renderer->offsets[index].x        = sprite->position.x;
        renderer->offsets[index].y        = sprite->position.y;
        renderer->src_rects[index].x      = sprite->src_rect.x / texture_width;
        renderer->src_rects[index].y      = sprite->src_rect.y / texture_height;
        renderer->src_rects[index].width  = sprite->src_rect.width / texture_width;
        renderer->src_rects[index].height = sprite->src_rect.height / texture_height;
        renderer->texture_indices[index]  = (float)texture_index;
        renderer->limit_ys[index].x       = 0;
        renderer->limit_ys[index].y       = 0;
        renderer->change_mask             = 0b11111111;
        renderer->count++;
    }
}
int8_t d2d_renderer_sprite_add(RendererSprite* renderer
                              ,float           x
                              ,float           y
                              ,float           dest_width
                              ,float           dest_height
                              ,float           src_x
                              ,float           src_y
                              ,float           src_width
                              ,float           src_height
                              ,Texture*        texture
                              ,Color*          color
                              )
{

    int32_t texture_index = d2d_renderer_sprite_add_texture(renderer, texture->renderer_id);
    if (texture_index == -1)
    {
        return DELO_ERROR;
    }
    int32_t index = renderer->count;

    if (index < renderer->capacity)
    {
        uint16_t texture_width  = (texture == NULL) ? 0.0 : texture->width;
        uint16_t texture_height = (texture == NULL) ? 0.0 : texture->height;

        renderer->colors[index]           = *color;
        renderer->transforms[index]       = d2d_matrix44_scale(dest_width * 0.5, dest_height * 0.5, 1);
        renderer->offsets[index].x        = x;
        renderer->offsets[index].y        = y;
        renderer->src_rects[index].x      = src_x / texture_width;
        renderer->src_rects[index].y      = src_y / texture_height;
        renderer->src_rects[index].width  = src_width / texture_width;
        renderer->src_rects[index].height = src_height / texture_height;
        renderer->texture_indices[index]  = (float)texture_index;
        renderer->limit_ys[index].x       = 0;
        renderer->limit_ys[index].y       = 0;
        renderer->change_mask             = 0b11111111;
        renderer->count++;
    }
}
int8_t d2d_renderer_sprite_begin(RendererSprite* renderer
                                ,Matrix44        projection
                                )
{
    renderer->count        = 0;
    renderer->projection   = projection;
    renderer->texture_id_0 = -1;
    renderer->texture_id_1 = -1;
    renderer->texture_id_2 = -1;
    renderer->texture_id_3 = -1;
}
int8_t d2d_renderer_sprite_end(RendererSprite* renderer)
{
    d2d_renderer_sprite_update(renderer);
    d2d_renderer_sprite_render(renderer);
}
// ================================
// Renderer SpriteFont functions
// ================================
int8_t d2d_renderer_sprite_font_init(RendererSpriteFont* renderer
                                    ,D2DContext*         context
                                    ,uint32_t            capacity
                                    )
{
    renderer->context = context;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return DELO_ERROR;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    GLfloat vertices[] =
    {
        // positions        //texCoords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // top-left
    };

    glGenBuffers(1, &renderer->vbo_vertices);
    glGenBuffers(1, &renderer->vbo_colors);
    glGenBuffers(1, &renderer->vbo_transforms);
    glGenBuffers(1, &renderer->vbo_offsets);
    glGenBuffers(1, &renderer->vbo_src_rects);
    glGenBuffers(1, &renderer->vbo_tex_indices);
    glGenBuffers(1, &renderer->vbo_limit_y);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_colors);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_offsets);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_src_rects);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_transforms);
    for (int32_t i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(5 + i);
        glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void *)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(5 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_tex_indices);
    glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
    glEnableVertexAttribArray(9);
    glVertexAttribDivisor(9, 1);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_limit_y);
    glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(10);
    glVertexAttribDivisor(10, 1);

    glBindVertexArray(0);
    glUseProgram(0);

    renderer->capacity = capacity;
    renderer->count    = 0;

    renderer->colors          = malloc(sizeof(Color)       * capacity);
    renderer->transforms      = malloc(sizeof(Matrix44)    * capacity);
    renderer->offsets         = malloc(sizeof(Vector2f)    * capacity);
    renderer->src_rects       = malloc(sizeof(Rectangle_f) * capacity);
    renderer->texture_indices = malloc(sizeof(float)       * capacity);
    renderer->limit_ys        = malloc(sizeof(Vector2f)    * capacity);

    for (uint32_t i = 0; i < capacity; i++)
    {
        renderer->texture_indices[i] = 0;
    }

    renderer->projection = d2d_matrix44_orthographic_projection((float)0.0f
                                                               ,(float)context->back_buffer_width
                                                               ,(float)0.0f
                                                               ,(float)context->back_buffer_height
                                                               ,(float)1
                                                               ,(float)-1
                                                               );
    renderer->texture_id_0 = -1;
    renderer->texture_id_1 = -1;
    renderer->texture_id_2 = -1;
    renderer->texture_id_3 = -1;

    renderer->flip = 0;
}

int8_t d2d_renderer_sprite_font_apply_shader(RendererSpriteFont* renderer
                                            ,uint32_t            shader
                                            )
{
    renderer->shader = shader;

    glUseProgram(shader);
    renderer->uniform_location_u_texture0           = glGetUniformLocation(shader, "u_texture0");
    renderer->uniform_location_u_texture1           = glGetUniformLocation(shader, "u_texture1");
    renderer->uniform_location_u_texture2           = glGetUniformLocation(shader, "u_texture2");
    renderer->uniform_location_u_texture3           = glGetUniformLocation(shader, "u_texture3");
    renderer->uniform_location_u_mvp                = glGetUniformLocation(shader, "u_mvp");
    renderer->uniform_location_u_flip               = glGetUniformLocation(shader, "u_flip");
    renderer->uniform_location_u_back_buffer_height = glGetUniformLocation(shader, "u_back_buffer_height");
    
    glUniform1i(renderer->uniform_location_u_flip, 0);
    glUniform1f(renderer->uniform_location_u_back_buffer_height,renderer->context->back_buffer_height);
    glUseProgram(0);
}
int8_t d2d_renderer_sprite_font_update(RendererSpriteFont* renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * renderer->count, (float *)renderer->colors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_transforms);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix44) * renderer->count, (float *)renderer->transforms, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_offsets);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * renderer->count, (float *)renderer->offsets, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_src_rects);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Rectangle_f) * renderer->count, (float *)renderer->src_rects, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_tex_indices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderer->count, (float*)renderer->texture_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_limit_y);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * renderer->count, (Vector2f *)renderer->limit_ys, GL_STATIC_DRAW);
    
}
int8_t d2d_renderer_sprite_font_add_font(RendererSpriteFont* renderer
                                        ,SpriteFont*         sprite_font
                                        )
{
    uint32_t texture_id = sprite_font->texture.renderer_id;

    if (renderer->texture_id_0 == -1 || renderer->texture_id_0 == texture_id)
    {
        renderer->texture_id_0 = texture_id;
        return 0;
    }
    else if (renderer->texture_id_1 == -1 || renderer->texture_id_1 == texture_id)
    {
        renderer->texture_id_1 = texture_id;
        return 1;
    }
    else if (renderer->texture_id_2 == -1 || renderer->texture_id_2 == texture_id)
    {
        renderer->texture_id_2 = texture_id;
        return 2;
    }
    else if (renderer->texture_id_3 == -1 || renderer->texture_id_3 == texture_id)
    {
        renderer->texture_id_3 = texture_id;
        return 3;
    }
    else
    {
        return -1;
    }
}
int8_t d2d_renderer_sprite_font_render(RendererSpriteFont* renderer)
{
    /*------------------Draw instances-----------------*/
    glBindVertexArray(renderer->vao);

    // Use the shader program
    glUseProgram(renderer->shader);

    if (renderer->texture_id_0 != -1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_0);
        glUniform1i(renderer->uniform_location_u_texture0, 0);
    }

    if (renderer->texture_id_1 != -1)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_1);
        glUniform1i(renderer->uniform_location_u_texture1, 1);
    }

    if (renderer->texture_id_2 != -1)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_2);
        glUniform1i(renderer->uniform_location_u_texture2, 2);
    }

    if (renderer->texture_id_3 != -1)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, renderer->texture_id_3);
        glUniform1i(renderer->uniform_location_u_texture3, 3);
    }

    // Set the MVP matrix
    Matrix44 *projection = &renderer->projection;
    glUniformMatrix4fv(renderer->uniform_location_u_mvp, 1, GL_FALSE, &projection->x11);

    // Draw the geometry
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, renderer->count);

    if (renderer->texture_id_3 != -1)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (renderer->texture_id_2 != -1)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (renderer->texture_id_1 != -1)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (renderer->texture_id_0 != -1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Cleanup
    glBindVertexArray(0);
    glUseProgram(0);

    /*------------------Draw instances-----------------*/
}
int8_t d2d_renderer_sprite_font_begin(RendererSpriteFont* renderer
                                     ,Matrix44            projection
                                     )
{
    renderer->count        = 0;
    renderer->projection   = projection;

    renderer->texture_id_0 = -1;
    renderer->texture_id_1 = -1;
    renderer->texture_id_2 = -1;
    renderer->texture_id_3 = -1;
}
int8_t d2d_renderer_sprite_font_end(RendererSpriteFont* renderer)
{
    d2d_renderer_sprite_font_update(renderer);
    d2d_renderer_sprite_font_render(renderer);
}
int8_t d2d_renderer_sprite_font_add_text(RendererSpriteFont* renderer
                                        ,SpriteFont*         sprite_font
                                        ,char*               text
                                        ,uint32_t            max_length
                                        ,Vector2f            position
                                        ,Color               color
                                        ,Vector2f            limit_y
                                        )
{
    int32_t texture_index = d2d_renderer_sprite_font_add_font(renderer, sprite_font);

    if (texture_index != -1)
    {
        uint32_t length = strlen(text);

        uint16_t texture_width = sprite_font->texture.width;
        uint16_t texture_height = sprite_font->texture.height;

        for (uint32_t i = 0; i < length; i++)
        {
            if (max_length > 0)
            {
                if (i > max_length)
                {
                    break;
                }
            }
            int32_t index = renderer->count;

            if (index < renderer->capacity)
            {
                int32_t gindex = (int32_t)text[i] - 33;
                Glyph *glyph = &sprite_font->glyphs[gindex];

                if (gindex > 0)
                {
                    renderer->colors[index]           = color;
                    renderer->transforms[index]       = d2d_matrix44_scale((float)glyph->w * 0.5, (float)glyph->h * 0.5, 1);
                    renderer->offsets[index].x        = position.x + glyph->bearing_x + sprite_font->padding;
                    renderer->offsets[index].y        = position.y - glyph->bearing_y + sprite_font->font_size + sprite_font->padding;
                    renderer->src_rects[index].x      = (float)glyph->x / texture_width;
                    renderer->src_rects[index].y      = (float)glyph->y / texture_height;
                    renderer->src_rects[index].width  = (float)glyph->w / texture_width;
                    renderer->src_rects[index].height = (float)glyph->h / texture_height;
                    renderer->limit_ys[index].x       = limit_y.x;
                    renderer->limit_ys[index].y       = limit_y.y;

                    renderer->texture_indices[index] = (float)texture_index;
                    renderer->count++;
                    position.x += glyph->advance;
                }

                position.x += (int32_t)text[i] == 32 ? sprite_font->advance : 0;
            }
        }
    }
}

// ================================
// SpriteFont functions
// ================================
int8_t d2d_sprite_font_load(SpriteFont* sprite_font
                           ,char*       path
                           ,uint16_t    font_size
                           )
{
    setlocale(LC_ALL, "en_US.UTF-8");
    uint32_t max = 256;
    FT_Library ft;
    FT_Face face;

    if (FT_Init_FreeType(&ft))
    {
        fprintf(stderr, "Error: Could not initialize FreeType library\n");
        return DELO_ERROR;
    }

    if (FT_New_Face(ft, path, 0, &face))
    {
        fprintf(stderr, "Error: Could not open font file\n");
        FT_Done_FreeType(ft);
        return DELO_ERROR;
    }

    if (FT_Set_Pixel_Sizes(face, 0, (FT_UInt)font_size))
    {
        FT_Done_FreeType(ft);
        return DELO_ERROR;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &sprite_font->texture.renderer_id);
    glBindTexture(GL_TEXTURE_2D, sprite_font->texture.renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    sprite_font->texture.width = 0;
    sprite_font->texture.height = 0;
    sprite_font->padding = 8;

    int32_t max_ascent = 0;
    int32_t max_descent = 0;

    if (FT_Load_Char(face, 32, FT_LOAD_NO_BITMAP))
    {
        fprintf(stderr, "Failed to load glyph\n");
    }
    else
    {
        sprite_font->advance = face->glyph->advance.x / 64;
    }

    sprite_font->texture.width = sprite_font->padding;
    for (uint32_t c = 33; c < max; c++)
    {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Failed to load glyph\n");
            continue;
        }
        else
        {
            sprite_font->texture.width += face->glyph->bitmap.width + sprite_font->padding;
            sprite_font->texture.height = (face->glyph->bitmap.rows > sprite_font->texture.height) ? face->glyph->bitmap.rows : sprite_font->texture.height;

            sprite_font->line_spacing = face->size->metrics.height >> 6;

            FT_GlyphSlot g = face->glyph;

            int32_t ascent  = g->bitmap_top;
            int32_t descent = g->bitmap.rows - g->bitmap_top;

            if (ascent > max_ascent)
            {
                max_ascent = ascent;
            }
            if (descent > max_descent)
            {
                max_descent = descent;
            }
        }
    }
    int width  = sprite_font->texture.width;
    int height = sprite_font->texture.height;

    unsigned char* black_pixels = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    memset(black_pixels, 0, width * height * sizeof(unsigned char));
    
    glTexImage2D(GL_TEXTURE_2D
                ,0
                ,GL_RED
                ,sprite_font->texture.width
                ,sprite_font->texture.height
                ,0
                ,GL_RED
                ,GL_UNSIGNED_BYTE
                ,black_pixels
                );

    free(black_pixels);

    float offset_x = sprite_font->padding;
    float offset_y = 0;

    int32_t index = 0;

    for (uint32_t c = 33; c < max; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Failed to load glyph\n");
            continue;
        }
        else
        {
            offset_y = ((float)max_ascent) - ((float)face->glyph->bitmap_top);

            sprite_font->glyphs[index].x         = offset_x;
            sprite_font->glyphs[index].y         = 0;
            sprite_font->glyphs[index].w         = face->glyph->bitmap.width + sprite_font->padding;
            sprite_font->glyphs[index].h         = (face->size->metrics.height >> 6);
            sprite_font->glyphs[index].advance   = face->glyph->advance.x >> 6;
            sprite_font->glyphs[index].bearing_x = face->glyph->bitmap_left;
            sprite_font->glyphs[index].bearing_y = face->glyph->bitmap_top;

            index++;
            int32_t length = face->glyph->bitmap.rows * face->glyph->bitmap.pitch;

            glTexSubImage2D(GL_TEXTURE_2D
                           ,0
                           ,offset_x
                           ,0
                           ,face->glyph->bitmap.width
                           ,face->glyph->bitmap.rows
                           ,GL_RED
                           ,GL_UNSIGNED_BYTE
                           ,face->glyph->bitmap.buffer
                           );

            offset_x += face->glyph->bitmap.width + sprite_font->padding;
        }
    }

    sprite_font->font_size               = font_size;
    sprite_font->texture.bytes_per_pixel = 4;
    sprite_font->texture.initialized     = 1;

    return DELO_SUCCESS;
}
uint32_t *sprite_font_convert_to_unicode(const char *string_utf8)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    size_t length_utf8 = strlen(string_utf8);

    size_t wchar_size = mbstowcs(NULL, string_utf8, 0);

    if (wchar_size == (size_t)-1)
    {
        perror("mbstowcs");
        exit(EXIT_FAILURE);
    }

    wchar_t *unicode_string = (wchar_t *)malloc((wchar_size + 1) * sizeof(wchar_t));
    if (unicode_string == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (mbstowcs(unicode_string, string_utf8, length_utf8) == (size_t)-1)
    {
        perror("mbstowcs");
        free(unicode_string);
        exit(EXIT_FAILURE);
    }

    unicode_string[wchar_size] = L'\0';

    uint32_t *result = (uint32_t *)malloc((wchar_size + 1) * sizeof(uint32_t));
    if (result == NULL)
    {
        perror("malloc");
        free(unicode_string);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i <= wchar_size; ++i)
    {
        result[i] = (uint32_t)unicode_string[i];
    }

    free(unicode_string);

    return result;
}
int32_t d2d_sprite_font_set_caret_mouse(char*       text
                                       ,uint32_t    offset
                                       ,SpriteFont* sprite_font
                                       ,Vector2f    mp
                                       )
{
    setlocale(LC_ALL, "en_US.UTF-8");
    Vector2f measure = {0, 0};
    uint32_t *unicode_array = sprite_font_convert_to_unicode(text);
    double min = FLT_MAX;
    int32_t min_index = offset;

    if (mp.x <= 0)
    {
        return 0;
    }

    for (size_t i = offset; unicode_array[i] != 0; ++i)
    {
        uint32_t c = unicode_array[i];

        if (c > 32 && c < 256)
        {
            Glyph *glyph = &sprite_font->glyphs[(int32_t)c - 33];

            measure.x += glyph->advance;
            measure.y = glyph->h;
        }
        else if (c == 32)
        {
            measure.x += sprite_font->advance;
        }

        double dist = floor(fabs(measure.x - mp.x));
        if (dist <= min)
        {
            min = dist;
            min_index = i + 1;
        }
    }
    free(unicode_array);

    return min_index;
}
uint32_t d2d_sprite_font_calc_char_limit(char*       text
                                        ,SpriteFont* sprite_font
                                        ,int32_t     max_width
                                        )
{
    return 1;
}
void d2d_sprite_font_measure_string(char*            text
                                   ,SpriteFont*      sprite_font
                                   ,FontMeasurement* measurement
                                   ,int16_t          limit_ch
                                   ,int16_t          limit_px
                                   )
{
    setlocale(LC_ALL, "en_US.UTF-8");
    Vector2f measure = {0, 0};
    uint32_t *unicode_array = sprite_font_convert_to_unicode(text);

    int16_t width_px = 0;
    int16_t width_ch = 0;

    measurement->limit_ch_hit = 0;
    measurement->limit_px_hit = 0;

    for (size_t i = 0; unicode_array[i] != 0; ++i)
    {
         if(i >= limit_ch && limit_ch != -1)
        {
            measurement->limit_ch_hit = 1;
            break;
        }
        else if (width_px >= limit_px && limit_px != -1)
        {
            measurement->limit_px_hit = 1;
            break;
        }

        uint32_t c = unicode_array[i];

        if (c > 32 && c < 256)
        {
            Glyph *glyph = &sprite_font->glyphs[(int32_t)c - 33];
            width_px += glyph->advance;
            width_ch++;
        }
        else if (c == 32)
        {
            width_px += sprite_font->advance;
            width_ch++;
        }

    }
    measurement->width_in_ch  = width_ch;
    measurement->width_in_px  = width_px;
    measurement->height_in_px = sprite_font->font_size + sprite_font->padding;

    free(unicode_array);
}
Vector2f d2d_sprite_font_measure_string2(char*       text
                                        ,SpriteFont* sprite_font
                                        )
{
    setlocale(LC_ALL, "en_US.UTF-8");
    Vector2f measure = {0, 0};
    uint32_t *unicode_array = sprite_font_convert_to_unicode(text);

    int16_t width_px = 0;

    for (size_t i = 0; unicode_array[i] != 0; ++i)
    {
        uint32_t c = unicode_array[i];

        if (c > 32 && c < 256)
        {
            Glyph *glyph = &sprite_font->glyphs[(int32_t)c - 33];
            width_px += glyph->advance;
        }
        else if (c == 32)
        {
            width_px += sprite_font->advance;
        }

    }
    measure.x = width_px;
    measure.y = sprite_font->font_size + sprite_font->padding;

    free(unicode_array);

    return measure;
}

#endif

// ================================
// delo3d extension
// Define DELO3D_FUNCTION_SIGNATURES for access to function signatures.
// Define DELO3D_IMPLEMENTATION for implementation.
// Example:
// #define DELO3D_IMPLEMENTATION
// #include <delo2d.h>
// ================================
typedef struct Vector3f Vector3f;
struct Vector3f
{
    float x,y,z;
};

typedef struct Camera3D Camera3D;
struct Camera3D
{
    GLfloat view_matrix[16];
    Vector3f position;
    Vector3f front;
    Vector3f up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float zoom;
    float last_x;
    float last_y;
    int mb1;
    uint8_t block_interaction;
};

typedef struct Mesh Mesh;
struct Mesh
{
    float *vertices;
    int32_t vertices_length;
    uint32_t vertex_count;
    
};

typedef struct RendererCubes RendererCubes;
struct RendererCubes
{
    Mesh    mesh;
    GLfloat projection_matrix[16];
    GLuint  vertex_shader;
    GLuint  fragment_shader;
    GLuint  shader;
    GLuint  uniform_location_view;
    GLuint  uniform_location_projection;
    GLuint  uniform_location_mode;
    GLuint  vbo;
    GLuint  vao;
    GLuint  vbo_instance_positions;
    GLuint  vbo_instance_colors;
    GLuint  vbo_instance_transforms;
    GLuint  vbo_instance_ids;
    GLuint  vbo_instance_selected;
    GLuint  fbo_object_picking;
    GLuint  texture_object_picking;
    uint8_t update_depth_buffer;
    uint8_t update_colors;
    uint8_t update_ids;
    uint8_t update_positions;
    uint8_t update_transforms;
    uint8_t update_selections;
    GLsync fence;
};

#if defined(DELO3D_FUNCTION_SIGNATURES) || defined(DELO3D_IMPLEMENTATION)
// ================================
// Graphics context functions
// ================================
void  d3d_context_init(D2DContext *context);
// ================================
// Matrix44 functions
// ================================
Matrix44 d3d_matrix44_perspective(float fov, float aspect_ratio, float near_plane, float far_plane);
Matrix44 d3d_matrix44_look_at(Vector3f camera, Vector3f target, Vector3f up);
Matrix44 d3d_createPerspectiveMatrix(float fov, float aspect_ratio, float near_plane, float far_plane);
// ================================
// Camera3D functions
// ================================
void  d3d_camera3d_init(Camera3D *camera);
void  d3d_camera3d_look_at(const Vector3f eye, const Vector3f target, const Vector3f up, float* view_matrix);
void  d3d_camera3d_move_front(Camera3D *camera, float distance);
void  d3d_camera3d_move_right(Camera3D *camera, float distance); 
void  d3d_camera3d_move_up(Camera3D *camera, float distance); 
void  d3d_camera3d_move_down(Camera3D *camera, float distance);
void  d3d_camera3d_update_front(Camera3D* camera); 
void  d3d_camera3d_get_view_matrix(Camera3D *camera, float* viewMatrix); 
void  d3d_camera3d_update_view_matrix(Camera3D *camera); 
void  d3d_camera3d_process_input(Camera3D* camera,GLFWwindow *window, float dt); 
// ================================
// Vector3f functions
// ================================
void  d3d_vector3f_normalize(Vector3f *vector); 
void  d3d_vector3f_cross_product(const Vector3f v1, const Vector3f v2, Vector3f* result); 
void  d3d_vector3f_subtract(const Vector3f v1, const Vector3f v2, Vector3f* result); 
void  d3d_callback_scroll(GLFWwindow* window, double offset_x, double offset_y);
void  d3d_callback_mouse_button(GLFWwindow* window, int32_t button, int32_t action, int32_t mods); 
void  d3d_callback_mouse(GLFWwindow* window, double position_x, double position_y);
float d3d_vector3f_dot_product(const Vector3f v1, const Vector3f v2); 
// ================================
// Instance cube renderer functions
// ================================
void  d3d_renderer_cube_instancing_init(RendererCubes*  renderer,D2DContext* context,int32_t instance_count);
void  d3d_renderer_cube_instancing_apply_shader(RendererCubes* renderer,uint32_t shader,GLfloat projection_matrix[16]);
void  d3d_renderer_cube_instancing_update(RendererCubes* renderer,GLfloat* instance_positions,GLfloat* instance_transforms,GLfloat* instance_colors,GLfloat* instance_ids,GLint* instance_selected,int32_t instance_count);
void  d3d_renderer_cube_instancing_render(RendererCubes* renderer,GLfloat view_matrix[16],int32_t instance_count);
// ================================
// Mouse picking functions
// ================================
Color d3d_generate_pick_color(int index);
int   d3d_get_index_from_color(Color pick_color);
int   d3d_pick(GLint fbo, float mouse_x, float mouse_y, int width, int height, int back_buffer_height);
#endif

#ifdef DELO3D_IMPLEMENTATION
#include <math.h>
#include <time.h>
Matrix44 d3d_matrix44_perspective(float fov
                                 ,float aspect_ratio
                                 ,float near_plane
                                 ,float far_plane
                                 ) 
{
    Matrix44 mat;

    float f = 1.0f / tanf(fov / 2.0f);
    float nf = 1.0f / (near_plane - far_plane);

    mat.x11 = f / aspect_ratio; 
    mat.x22 = f;               
    mat.x33 = (far_plane + near_plane) * nf; 
    mat.x34 = (2.0f * far_plane * near_plane) * nf;
    mat.x43 = -1.0f;         
    mat.x44 = 0.0f;           

    mat.x12 = mat.x13 = mat.x14 = 0.0f;
    mat.x21 = mat.x23 = mat.x24 = 0.0f;
    mat.x31 = mat.x32 = mat.x41 = mat.x42 = 0.0f;

    return mat;
}
Matrix44 d3d_matrix44_look_at(Vector3f camera
                             ,Vector3f target
                             ,Vector3f up
                             ) 
{
    Matrix44 matrix;

    Vector3f forward = 
    {
        target.x - camera.x,
        target.y - camera.y,
        target.z - camera.z
    };
    float forwardLength = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);

    forward.x /= forwardLength; forward.y /= forwardLength; forward.z /= forwardLength;

    Vector3f right = 
    {
        up.y * forward.z - up.z * forward.y,
        up.z * forward.x - up.x * forward.z,
        up.x * forward.y - up.y * forward.x
    };
    float rightLength = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
    right.x /= rightLength; right.y /= rightLength; right.z /= rightLength;

    Vector3f trueUp = 
    {
        forward.y * right.z - forward.z * right.y,
        forward.z * right.x - forward.x * right.z,
        forward.x * right.y - forward.y * right.x
    };

    matrix.x11 = right.x;   
    matrix.x21 = trueUp.x;   
    matrix.x31 = -forward.x;  
    matrix.x41 = 0.0f;

    matrix.x12 = right.y;   
    matrix.x22 = trueUp.y;   
    matrix.x32 = -forward.y;  
    matrix.x42 = 0.0f;

    matrix.x13 = right.z;   
    matrix.x23 = trueUp.z;   
    matrix.x33 = -forward.z;  
    matrix.x43 = 0.0f;

    matrix.x14 = -(right.x * camera.x + right.y * camera.y + right.z * camera.z);
    matrix.x24 = -(trueUp.x * camera.x + trueUp.y * camera.y + trueUp.z * camera.z);
    matrix.x34 =  (forward.x * camera.x + forward.y * camera.y + forward.z * camera.z);

    matrix.x44 = 1.0f;

    return matrix;
}

void  d3d_context_init(D2DContext *context)
{
    GLfloat fov          = 45.0f;
    GLfloat aspect_ratio = 1920.0f / 1080.0f;
    GLfloat near_plane   = 0.1f;
    GLfloat far_plane    = 1000.0f;

    fov = 45.0f * (3.14 / 180.0f);
    Matrix44 m = d3d_matrix44_perspective(fov, aspect_ratio, near_plane, far_plane);
    memcpy(context->projection_matrix,&m,sizeof(Matrix44));

}
void d3d_camera3d_init(Camera3D *camera)
{
    camera->front.x = 0.0f;
    camera->front.y = 0.0f;
    camera->front.z = -1.0f;

    camera->up.x = 0.0f;
    camera->up.y = 1.0f;
    camera->up.z = 0.0f;

    camera->yaw         = -133.0f;
    camera->pitch       = -30.0f;
    camera->speed       = 40.5f;
    camera->sensitivity = 0.1f;
    camera->zoom        = 45.0f;

    camera->position.x = 106;
    camera->position.y = 84;
    camera->position.z = 102;

    camera->mb1 = 0;
    camera->block_interaction = 0;
}
void d3d_camera3d_look_at(const Vector3f eye
                         ,const Vector3f target
                         ,const Vector3f up
                         ,float*         view_matrix
                         )
{
    Vector3f f;
    Vector3f r;
    Vector3f u;

    d3d_vector3f_subtract(target, eye, &f);
    d3d_vector3f_normalize(&f);

    d3d_vector3f_cross_product(f, up, &r);
    d3d_vector3f_normalize(&r);

    d3d_vector3f_cross_product(r, f, &u);

    view_matrix[0] =  r.x;
    view_matrix[1] =  u.x;
    view_matrix[2] = -f.x;
    view_matrix[3] =  0.0f;

    view_matrix[4] =  r.y;
    view_matrix[5] =  u.y;
    view_matrix[6] = -f.y;
    view_matrix[7] =  0.0f;

    view_matrix[8]  =  r.z;
    view_matrix[9]  =  u.z;
    view_matrix[10] = -f.z;
    view_matrix[11] =  0.0f;

    view_matrix[12] = -d3d_vector3f_dot_product(r, eye);
    view_matrix[13] = -d3d_vector3f_dot_product(u, eye);
    view_matrix[14] =  d3d_vector3f_dot_product(f, eye);
    view_matrix[15] =  1.0f;
}
void d3d_camera3d_move_front(Camera3D* camera
                            ,float     distance
                            ) 
{
    camera->position.x += camera->front.x * distance;
    camera->position.y += camera->front.y * distance;
    camera->position.z += camera->front.z * distance;
}
void d3d_camera3d_move_right(Camera3D* camera
                            ,float     distance
                            ) 
{
    Vector3f right;
    d3d_vector3f_cross_product(camera->front, camera->up, &right);
    d3d_vector3f_normalize(&right);
    camera->position.x += right.x * distance;
    camera->position.y += right.y * distance;
    camera->position.z += right.z * distance;
}
void d3d_camera3d_move_up(Camera3D* camera
                         ,float     distance
                         ) 
{
    camera->position.y += distance;
}
void d3d_camera3d_move_down(Camera3D* camera
                           ,float     distance
                           ) 
{
    camera->position.y -= distance;
}
void d3d_camera3d_update_front(Camera3D* camera) 
{
    float radYaw   = d2d_math_radians(camera->yaw);
    float radPitch = d2d_math_radians(camera->pitch);

    camera->front.x = cos(radYaw) * cos(radPitch);
    camera->front.y = sin(radPitch);
    camera->front.z = sin(radYaw) * cos(radPitch);

    d3d_vector3f_normalize(&camera->front);
}
void d3d_camera3d_get_view_matrix(Camera3D* camera
                                 ,float*    viewMatrix
                                 ) 
{
    Vector3f target;
    target.x = camera->position.x + camera->front.x;
    target.y = camera->position.y + camera->front.y;
    target.z = camera->position.z + camera->front.z;


    Matrix44 m = d3d_matrix44_look_at(camera->position,target,camera->up);
    memcpy(viewMatrix,&m,sizeof(Matrix44));
}
void d3d_camera3d_update_view_matrix(Camera3D *camera) 
{
    float view_matrix[16];
    d3d_camera3d_get_view_matrix(camera, view_matrix);

    camera->view_matrix[0]  = view_matrix[0];
    camera->view_matrix[1]  = view_matrix[1];
    camera->view_matrix[2]  = view_matrix[2];
    camera->view_matrix[3]  = view_matrix[3];

    camera->view_matrix[4]  = view_matrix[4];
    camera->view_matrix[5]  = view_matrix[5];
    camera->view_matrix[6]  = view_matrix[6];
    camera->view_matrix[7]  = view_matrix[7];

    camera->view_matrix[8]  = view_matrix[8];
    camera->view_matrix[9]  = view_matrix[9];
    camera->view_matrix[10] = view_matrix[10];
    camera->view_matrix[11] = view_matrix[11];

    camera->view_matrix[12] = view_matrix[12];
    camera->view_matrix[13] = view_matrix[13];
    camera->view_matrix[14] = view_matrix[14];
    camera->view_matrix[15] = view_matrix[15];
}
void d3d_camera3d_process_input(Camera3D*   camera
                               ,GLFWwindow* window
                               ,float       dt
                               ) 
{
    if(camera->block_interaction)return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        d3d_camera3d_move_front(camera, camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        d3d_camera3d_move_front(camera, -camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        d3d_camera3d_move_right(camera, camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        d3d_camera3d_move_right(camera, -camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        d3d_camera3d_move_up(camera, camera->speed * dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        d3d_camera3d_move_down(camera, camera->speed * dt);
    }
}

void d3d_vector3f_normalize(Vector3f *vector) 
{
    float length = sqrt(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
}
void d3d_vector3f_cross_product(const Vector3f v1
                               ,const Vector3f v2
                               ,Vector3f*      result
                               ) 
{
    result->x = v1.y * v2.z - v1.z * v2.y;
    result->y = v1.z * v2.x - v1.x * v2.z;
    result->z = v1.x * v2.y - v1.y * v2.x;
}
void d3d_vector3f_subtract(const Vector3f v1
                          ,const Vector3f v2
                          ,Vector3f*      result
                          ) 
{
    result->x = v1.x - v2.x;
    result->y = v1.y - v2.y;
    result->z = v1.z - v2.z;
}
float d3d_vector3f_dot_product(const Vector3f v1
                              ,const Vector3f v2
                              ) 
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void d3d_callback_scroll(GLFWwindow* window
                        ,double      offset_x
                        ,double      offset_y
                        ) 
{
    GlfwCallbackData* glfw_callback_data = (GlfwCallbackData*)(glfwGetWindowUserPointer(window));
    Camera3D* camera = (Camera3D*)glfw_callback_data->camera;

    camera->zoom -= (float)(offset_y);

    if (camera->zoom < 1.0f)
    {
        camera->zoom = 1.0f;
    }
    if (camera->zoom > 45.0f)
    {
        camera->zoom = 45.0f;
    }
}
void d3d_callback_mouse_button(GLFWwindow* window
                              ,int32_t     button
                              ,int32_t     action
                              ,int32_t     mods
                              ) 
{
    GlfwCallbackData* glfw_callback_data = (GlfwCallbackData*)(glfwGetWindowUserPointer(window));
    Camera3D* camera = (Camera3D*)glfw_callback_data->camera;
    
    if(camera->block_interaction)return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
        camera->mb1 = 1;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
    {
        camera->mb1 = 0;
    }
}
void d3d_callback_mouse(GLFWwindow* window
                       ,double      position_x
                       ,double      position_y
                       ) 
{
    if(position_y < 100)return;
    GlfwCallbackData* glfw_callback_data = (GlfwCallbackData*)(glfwGetWindowUserPointer(window));
    Camera3D* camera = (Camera3D*)glfw_callback_data->camera;
    
    if(camera->block_interaction)return;
    
    float offset_x = camera->last_x-position_x;
    float offset_y = camera->last_y - position_y;

    camera->last_x = position_x;
    camera->last_y = position_y;

    offset_x *= camera->sensitivity;
    offset_y *= camera->sensitivity;

    if(camera->mb1)
    {
        camera->yaw   += offset_x;
        camera->pitch += offset_y;

        if (camera->pitch > 89.0f)
        {
            camera->pitch = 89.0f;
        }
        if (camera->pitch < -89.0f)
        {
            camera->pitch = -89.0f;
        }

        d3d_camera3d_update_front(camera);
    }
    
}
void d3d_renderer_cube_instancing_init(RendererCubes*  renderer
                                      ,D2DContext*     context
                                      ,int32_t         instance_count
                                      )
{

    renderer->update_colors     = 0;
    renderer->update_ids        = 0;
    renderer->update_positions  = 0;
    renderer->update_transforms = 0;
    renderer->update_selections = 0;

    srand(time(NULL));

    /*-------------------Define cube-------------------*/
    int32_t cube_vertex_count = 36;
    float cube_vertices[] = 
{
     // Front Face
    -0.5, -0.5, -0.5,   0, 0, -1,   // Vertex 1 + Normal
     0.5, -0.5, -0.5,   0, 0, -1,   // Vertex 2 + Normal
     0.5,  0.5, -0.5,   0, 0, -1,   // Vertex 3 + Normal

    -0.5, -0.5, -0.5,   0, 0, -1,   // Vertex 1 + Normal
     0.5,  0.5, -0.5,   0, 0, -1,   // Vertex 2 + Normal
    -0.5,  0.5, -0.5,   0, 0, -1,   // Vertex 3 + Normal

    // Back Face
    -0.5, -0.5,  0.5,   0, 0, 1,    // Vertex 1 + Normal
     0.5, -0.5,  0.5,   0, 0, 1,    // Vertex 2 + Normal
     0.5,  0.5,  0.5,   0, 0, 1,    // Vertex 3 + Normal

    -0.5, -0.5,  0.5,   0, 0, 1,    // Vertex 1 + Normal
     0.5,  0.5,  0.5,   0, 0, 1,    // Vertex 2 + Normal
    -0.5,  0.5,  0.5,   0, 0, 1,    // Vertex 3 + Normal

    // Left Face
    -0.5, -0.5, -0.5,  -1, 0, 0,     // Vertex 1 + Normal
    -0.5, -0.5,  0.5,  -1, 0, 0,     // Vertex 2 + Normal
    -0.5,  0.5,  0.5,  -1, 0, 0,     // Vertex 3 + Normal

    -0.5, -0.5, -0.5,  -1, 0, 0,     // Vertex 1 + Normal
    -0.5,  0.5,  0.5,  -1, 0, 0,     // Vertex 2 + Normal
    -0.5,  0.5, -0.5,  -1, 0, 0,     // Vertex 3 + Normal

    // Right Face
     0.5, -0.5, -0.5,   1, 0, 0,     // Vertex 1 + Normal
     0.5, -0.5,  0.5,   1, 0, 0,     // Vertex 2 + Normal
     0.5,  0.5,  0.5,   1, 0, 0,     // Vertex 3 + Normal

     0.5, -0.5, -0.5,   1, 0, 0,     // Vertex 1 + Normal
     0.5,  0.5,  0.5,   1, 0, 0,     // Vertex 2 + Normal
     0.5,  0.5, -0.5,   1, 0, 0,     // Vertex 3 + Normal

    // Top Face
    -0.5,  0.5, -0.5,   0, 1, 0,     // Vertex 1 + Normal
     0.5,  0.5, -0.5,   0, 1, 0,     // Vertex 2 + Normal
     0.5,  0.5,  0.5,   0, 1, 0,     // Vertex 3 + Normal

    -0.5,  0.5, -0.5,   0, 1, 0,     // Vertex 1 + Normal
     0.5,  0.5,  0.5,   0, 1, 0,     // Vertex 2 + Normal
    -0.5,  0.5,  0.5,   0, 1, 0,     // Vertex 3 + Normal

    // Bottom Face
    -0.5, -0.5, -0.5,   0, -1, 0,    // Vertex 1 + Normal
     0.5, -0.5, -0.5,   0, -1, 0,    // Vertex 2 + Normal
     0.5, -0.5,  0.5,   0, -1, 0,    // Vertex 3 + Normal

    -0.5, -0.5, -0.5,   0, -1, 0,    // Vertex 1 + Normal
     0.5, -0.5,  0.5,   0, -1, 0,    // Vertex 2 + Normal
    -0.5, -0.5,  0.5,   0, -1, 0     // Vertex 3 + Normal
};
    /*-------------------Define cube-------------------*/

    /*--------------Create vertex buffers--------------*/

    glGenFramebuffers(1, &renderer->fbo_object_picking);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo_object_picking);

    // Create and attach a texture as the color buffer
    GLuint textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // Create and attach a renderbuffer as the depth buffer
    GLuint rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
    {
        
    }

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);                   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &renderer->vbo_instance_positions);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_positions);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glGenBuffers(1, &renderer->vbo_instance_colors);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_colors);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &renderer->vbo_instance_transforms);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_transforms);

    for (int32_t i = 0; i < 4; i++) 
    {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4 * i));
        glVertexAttribDivisor(4 + i, 1);
    }

    glGenBuffers(1, &renderer->vbo_instance_ids);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_ids);
    glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(8);
    glVertexAttribDivisor(8, 1);

    glGenBuffers(1, &renderer->vbo_instance_selected);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_selected);
    glVertexAttribIPointer(9, 1, GL_INT, sizeof(GLint), (void*)0);
    glEnableVertexAttribArray(9);
    glVertexAttribDivisor(9, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*--------------Create vertex buffers--------------*/

    renderer->fence = NULL;

}
void d3d_renderer_cube_instancing_apply_shader(RendererCubes* renderer
                                              ,uint32_t       shader
                                              ,GLfloat        projection_matrix[16]
                                              )
{
    renderer->shader = shader;

    glUseProgram(shader);
    renderer->uniform_location_view       = glGetUniformLocation(renderer->shader, "view"      );
    renderer->uniform_location_projection = glGetUniformLocation(renderer->shader, "projection");
    renderer->uniform_location_mode       = glGetUniformLocation(renderer->shader, "mode");
    
    glUniformMatrix4fv(renderer->uniform_location_projection, 1, GL_FALSE, projection_matrix);

    
    glUseProgram(0);
}
void d3d_renderer_cube_instancing_update(RendererCubes* renderer
                                        ,GLfloat*       instance_positions
                                        ,GLfloat*       instance_transforms
                                        ,GLfloat*       instance_colors
                                        ,GLfloat*       instance_ids
                                        ,GLint*         instance_selected
                                        ,int32_t        instance_count
                                        )
{
    
    
    if(renderer->update_positions)
    {
        renderer->update_positions = 0;
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_positions);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instance_count * 3, instance_positions, GL_DYNAMIC_DRAW);
        renderer->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
    if(renderer->update_transforms)
    {
        renderer->update_transforms = 0;
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_transforms);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*instance_count * 16, instance_transforms, GL_DYNAMIC_DRAW);
        renderer->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
    if(renderer->update_colors)
    {
        renderer->update_colors = 0;
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_colors);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instance_count * 3, instance_colors, GL_DYNAMIC_DRAW);
        renderer->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
    if(renderer->update_ids)
    {
        renderer->update_ids = 0;
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_ids);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instance_count * 3, instance_ids, GL_DYNAMIC_DRAW);
        renderer->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    if(renderer->update_selections)
    {
        renderer->update_selections = 0;
        glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo_instance_selected);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * instance_count, instance_selected, GL_DYNAMIC_DRAW);
        renderer->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void d3d_renderer_cube_instancing_render(RendererCubes* renderer
                                        ,GLfloat        view_matrix[16]
                                        ,int32_t        instance_count
                                        )
{
    if(renderer->fence != NULL)
    {
        GLenum result = glClientWaitSync(renderer->fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
        if(result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED)
        {
            glWaitSync(renderer->fence, 0, GL_TIMEOUT_IGNORED);
        }
        glDeleteSync(renderer->fence);
        renderer->fence = NULL;
    }

    if(renderer->fence == NULL)
    {
        glBindVertexArray(renderer->vao);

        glUseProgram(renderer->shader);
    
        glUniformMatrix4fv(renderer->uniform_location_view, 1, GL_FALSE, view_matrix);

        if(renderer->update_depth_buffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo_object_picking);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUniform1i(renderer->uniform_location_mode, 0);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instance_count);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform1i(renderer->uniform_location_mode, 1);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instance_count);

        glBindVertexArray(0);
        glUseProgram(0);
    }

}
Color d3d_generate_pick_color(int index) 
{
    int r = index & 0xFF;
    int g = (index >> 8) & 0xFF;
    int b = (index >> 16) & 0xFF;

    return (Color){r / 255.0f, g / 255.0f, b / 255.0f};
}
int d3d_get_index_from_color(Color pick_color) 
{
    int r = (int)(pick_color.r * 255.0f);
    int g = (int)(pick_color.g * 255.0f);
    int b = (int)(pick_color.b * 255.0f);

    return (b << 16) | (g << 8) | r;
}
int d3d_pick(GLint fbo
            ,float mouse_x
            ,float mouse_y
            ,int   width
            ,int   height
            ,int   back_buffer_height
            ) 
{
    GLuint buffer[4];
    GLint viewport[4];
    GLubyte pixel[3];

    glViewport(0, 0, width, height);

    glReadBuffer(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGetIntegerv(GL_VIEWPORT, viewport);

    glReadPixels((int)mouse_x,back_buffer_height - (int)mouse_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Color pick_color = {
        pixel[0] / 255.0f,
        pixel[1] / 255.0f,
        pixel[2] / 255.0f
    };
    
    int index = d3d_get_index_from_color(pick_color)-1;

    return index;
}

#endif

/*
MIT License

Copyright (c) 2024 Dennis L�vkvist

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/