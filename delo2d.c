#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/delo2d.h"
#include "stb_image.h"
#include <math.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

const unsigned int screen_width = 960;
const unsigned int screen_height = 540;

void GLClearError()
{
    while(!glGetError());
}
void GLCheckError()
{
    GLenum error;
    while(error = glGetError())
    {
        printf("%s",error);
        printf("%c",'\n');
    }
}
void delo2d_rectangle_set(Rectangle *rectengle, int x, int y,int width, int height)
{
    rectengle->x = x;
    rectengle->y = y;
    rectengle->width = width;
    rectengle->height = height;
}

//render code begin
int delo2d_render_setup(GLFWwindow **window, unsigned int width, unsigned int height,const char *title)
{
    *window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!*window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*window);
    return 0;
}
int delo2d_render_initialize()
{
    if(glewInit() != GLEW_OK)
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glEnable(GL_TEXTURE_2D);

    return 0;
}
void delo2d_render(VertexArray *vertex_array,Texture *textures,int texture_count, unsigned int shader)
{   
    delo2d_bind_texture(&textures[0],0); 
    delo2d_bind_texture(&textures[1],1); 
    
    glUseProgram(shader); 
        

    delo2d_vertex_array_bind(vertex_array);

    GLClearError();
    delo2d_vertex_array_draw(vertex_array);
    GLCheckError();

    delo2d_unbind_texture();
}
//render code end

//texture code begin
void delo2d_load_texture(Texture *texture, char file_path[])
{
    stbi_set_flip_vertically_on_load(0);
    texture->local_buffer = stbi_load(file_path,&texture->width,&texture->height,&texture->bytes_per_pixel,4);
    glGenTextures(1,&texture->renderer_id);
    glBindTexture(GL_TEXTURE_2D,texture->renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,texture->width,texture->height,1,GL_RGBA,GL_UNSIGNED_BYTE,texture->local_buffer);


    glBindTexture(GL_TEXTURE_2D,0);
    if(texture->local_buffer)
    {
        stbi_image_free(texture->local_buffer);
    }
}
void delo2d_bind_texture(Texture *texture, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D,texture->renderer_id);
}
void delo2d_unbind_texture()
{
    glBindTexture(GL_TEXTURE_2D,0);
}
void delo2d_delete_texture(Texture *texture)
{
    glDeleteTextures(1,texture->renderer_id);
}
//texture code end

//region matrices begin
void delo2d_matrix_orthographic_projection(float (*matrix)[4][4], float l,float r,float t,float b,float f,float n)
{
    (*matrix)[0][0] = 2.0f/(r-l);  //Scale?  
    (*matrix)[0][1] = 0;              
    (*matrix)[0][2] = 0;                    
    (*matrix)[0][3] = -((r+l)/(r-l));

    (*matrix)[1][0] = 0; 
    (*matrix)[1][1] = 2.0/(t-b); //Scale?             
    (*matrix)[1][2] = 0;                    
    (*matrix)[1][3] = -((t+b)/(t-b));

    (*matrix)[2][0] = 0; 
    (*matrix)[2][1] = 0;              
    (*matrix)[2][2] = 2 / (f-n);                    
    (*matrix)[2][3] = -((f+n)/(f-n));

    (*matrix)[3][0] = 0; 
    (*matrix)[3][1] = 0;              
    (*matrix)[3][2] = 0;                    
    (*matrix)[3][3] = 1;
}
void delo2d_rotation_matrix(float (*R)[3][3],float theta, float tx, float ty)
{
    (*R)[0][0] = cos(theta);
    (*R)[0][1] = sin(theta);
    (*R)[0][2] = tx;

    (*R)[1][0] = -sin(theta);
    (*R)[1][1] = cos(theta);
    (*R)[1][2] = ty;

    (*R)[2][0] = 0;
    (*R)[2][1] = 0;
    (*R)[2][2] = 1;
}
void delo2d_matrix_mul_vector2fp_matrix33(Vector2fp *vector,float (*R)[3][3])
{
    float x =  ((*vector->x) * (*R)[0][0]) + ((*vector->y) * (*R)[0][1]) + (1 * (*R)[0][2]);
    float y = ((*vector->x) * (*R)[1][0]) + ((*vector->y) * (*R)[1][1]) + (1 * (*R)[1][2]);
    (*vector->x) = x;
    (*vector->y) = y;
}
//region matrices end

//region quads begin
void delo2d_quad_translate(Quad *quad,float tx, float ty)
{
    *(quad->v0.x) += tx;
    *(quad->v0.y) += ty;
    *(quad->v1.x) += tx;
    *(quad->v1.y) += ty;
    *(quad->v2.x) += tx;
    *(quad->v2.y) += ty;
    *(quad->v3.x) += tx;
    *(quad->v3.y) += ty;
}
void delo2d_quad_rotate(Quad *quad, float theta)
{
    Vector2f center;
    center.x = (*(quad->v0.x) + *(quad->v1.x) + *(quad->v2.x) + *(quad->v3.x))/4;
    center.y = (*(quad->v0.y) + *(quad->v1.y) + *(quad->v2.y) + *(quad->v3.y))/4;


    float R[3][3];
    delo2d_rotation_matrix(&R, 0,-center.x,-center.y);

    delo2d_matrix_mul_vector2fp_matrix33(&quad->v0, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v1, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v2, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v3, &R);

    delo2d_rotation_matrix(&R, theta,0,0);

    delo2d_matrix_mul_vector2fp_matrix33(&quad->v0, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v1, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v2, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v3, &R);

    delo2d_rotation_matrix(&R, 0,center.x,center.y);

    delo2d_matrix_mul_vector2fp_matrix33(&quad->v0, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v1, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v2, &R);
    delo2d_matrix_mul_vector2fp_matrix33(&quad->v3, &R);
}
void delo2d_get_quad(Quad *quad, VertexArray *vertex_array, int element_index)
{    
    int quad_index = element_index*vertex_array->layout_float_count*4;
    int stride = vertex_array->layout_float_count;
    int vertex_index = quad_index;

    quad->v0.x = &(vertex_array->buffer_position[quad_index]);
    quad->v0.y = &(vertex_array->buffer_position[quad_index+1]);

    quad->v1.x = &(vertex_array->buffer_position[quad_index + (stride)]);
    quad->v1.y = &(vertex_array->buffer_position[quad_index + (stride+1)]); 

    quad->v2.x = &(vertex_array->buffer_position[quad_index + (stride*2)]);
    quad->v2.y = &(vertex_array->buffer_position[quad_index + (stride*2+1)]); 


    quad->v3.x = &(vertex_array->buffer_position[quad_index + (stride*3)]);
    quad->v3.y = &(vertex_array->buffer_position[quad_index + (stride*3+1)]); 
}

//region quads end

//vertex array code begin
void delo2d_vertex_array_draw(VertexArray *vertex_array)
{
    int draw_index_count = vertex_array->indices_per_element * vertex_array->count_elements;
    glDrawElements(GL_TRIANGLES,draw_index_count,GL_UNSIGNED_INT,0);
}
void delo2d_vertex_set_element(VertexArray *vertex_array, int position,float x, float y, float tex_x,float tex_y,unsigned int texture_slot,float color[4])
{
    int index = position * vertex_array->layout_float_count;
    vertex_array->buffer_position[index + 0] = x;
    vertex_array->buffer_position[index + 1] = y;
    vertex_array->buffer_position[index + 2] = tex_x;
    vertex_array->buffer_position[index + 3] = tex_y;
    vertex_array->buffer_position[index + 4] = texture_slot;
    vertex_array->buffer_position[index + 5] = color[0];
    vertex_array->buffer_position[index + 6] = color[1];
    vertex_array->buffer_position[index + 7] = color[2];
    vertex_array->buffer_position[index + 8] = color[3];
}
void delo2d_vertex_set_tex_data(VertexArray *vertex_array, int position,float tex_x,float tex_y,unsigned int texture_slot)
{
    int index = position * vertex_array->layout_float_count;
    vertex_array->buffer_position[index + 2] = tex_x;
    vertex_array->buffer_position[index + 3] = tex_y;
    vertex_array->buffer_position[index + 4] = texture_slot;
}
void delo2d_vertex_array_delete(VertexArray *vertex_array)
{
    delo2d_vertex_array_unbind(vertex_array);
    free(vertex_array->buffer_position);
    free(vertex_array->buffer_index);
}
void delo2d_vertex_array_create(VertexArray *vertex_array,unsigned int type, unsigned int element_count)
{

    vertex_array->layout_float_count = 9;
    vertex_array->type = type;
    vertex_array->count_elements = element_count;
    vertex_array->count_position = element_count * (type*vertex_array->layout_float_count);
    vertex_array->buffer_position = malloc(vertex_array->count_position * sizeof(float));

    vertex_array->indices_per_element = 6;
    
    int length = vertex_array->count_position;
    for (size_t i = 0; i < length; i++)
    {
        vertex_array->buffer_position[i] = 0.0f;
    } 

    vertex_array->count_index = vertex_array->indices_per_element * element_count;
    vertex_array->buffer_index = malloc(vertex_array->count_index * sizeof(unsigned int)); 
            
            
    for (size_t i = 0; i < element_count; i++)
    {
        int index = i * 6;
        int vertex_index = i*4;
        vertex_array->buffer_index[index + 0] = vertex_index + 0;
        vertex_array->buffer_index[index + 1] = vertex_index + 1;
        vertex_array->buffer_index[index + 2] = vertex_index + 2;
        vertex_array->buffer_index[index + 3] = vertex_index + 2;
        vertex_array->buffer_index[index + 4] = vertex_index + 3;
        vertex_array->buffer_index[index + 5] = vertex_index + 0;
    }     

    glGenVertexArrays(1,&(vertex_array->vao)); 
    glBindVertexArray(&(vertex_array->vao));

    glGenBuffers(1,&(vertex_array->buffer));
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->buffer);

    glGenBuffers(1,&(vertex_array->ibo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vertex_array->ibo);

    //vertex position float2
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, sizeof(float)*vertex_array->layout_float_count,0);

    //texture coordinate float2
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, sizeof(float)*vertex_array->layout_float_count,(GLvoid*)(sizeof(float)*2));

    //texture index float
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*vertex_array->layout_float_count,(GLvoid*)(4 * sizeof(GLfloat)));

    //color float4
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*vertex_array->layout_float_count,(GLvoid*)(5 * sizeof(GLfloat)));

    //glBufferData(GL_ARRAY_BUFFER,(4 *vertex_array->count_elements * vertex_array->layout_float_count) * sizeof(float),vertex_array->buffer_position,GL_DYNAMIC_DRAW); 
    glBufferData(GL_ARRAY_BUFFER,vertex_array->count_position * sizeof(float),NULL,GL_DYNAMIC_DRAW); 

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertex_array->count_index * sizeof(unsigned int),vertex_array->buffer_index,GL_DYNAMIC_DRAW);

}
void delo2d_set_quad_center_position(VertexArray *vertex_array, int quad_index, Vector2f *center)
{
    for (int i = 0; i < 4; i++)
    {
        center->x += vertex_array->buffer_position[quad_index + (vertex_array->layout_float_count*i)];
        center->y += vertex_array->buffer_position[(quad_index + (vertex_array->layout_float_count*i)) + 1];
    }
    center->x /= 4;
    center->y /= 4;      
}
void delo2d_define_quad(VertexArray *vertex_array, int quad_index, Rectangle_f *rect_des,Rectangle_f *rect_src, int texture_index,float color[4])
{
    int n = quad_index * 4;
    delo2d_vertex_set_element(vertex_array,n,rect_des->x,rect_des->y,rect_src->x,rect_src->y,texture_index,color);
    delo2d_vertex_set_element(vertex_array,n + 1,rect_des->x + rect_des->width,rect_des->y,rect_src->x + rect_src->width,rect_src->y,texture_index,color);
    delo2d_vertex_set_element(vertex_array,n + 2,rect_des->x + rect_des->width,rect_des->y + rect_des->height,rect_src->x + rect_src->width,rect_src->y + rect_src->height,texture_index,color);
    delo2d_vertex_set_element(vertex_array,n + 3,rect_des->x,rect_des->y + rect_des->height,rect_src->x,rect_src->y + rect_src->height,texture_index,color);
}
void delo2d_translate_quad(VertexArray *vertex_array, int quad_index, float delta_x, float delta_y)
{
    for (int i = 0; i < 4; i++)
    {
        vertex_array->buffer_position[quad_index + (vertex_array->layout_float_count*i)] += delta_x;
        vertex_array->buffer_position[(quad_index + (vertex_array->layout_float_count*i)) + 1]+= delta_y;
    }
}
void delo2d_create_sprite_batch(SpriteBatch *sprite_batch,int capacity)
{
    sprite_batch->capacity = capacity;
    sprite_batch->count = 0;
    sprite_batch->rect_des = malloc(sizeof(Rectangle_f)*capacity);
    sprite_batch->rect_src_normalized = malloc(sizeof(Rectangle_f)*capacity);
    sprite_batch->texture_index = malloc(sizeof(unsigned int)*capacity);
    sprite_batch->quad_index = malloc(sizeof(unsigned int)*capacity);

    for (size_t i = 0; i < capacity; i++)
    {
        sprite_batch->texture_index[i] = 0;
        sprite_batch->quad_index[i] = 0;
        sprite_batch->rect_des[i].x = sprite_batch->rect_des[i].y = 0;
        sprite_batch->rect_des[i].width = sprite_batch->rect_des[i].height = 100;

        sprite_batch->rect_src_normalized[i].x = sprite_batch->rect_src_normalized[i].y = 0;
        sprite_batch->rect_src_normalized[i].width = sprite_batch->rect_src_normalized[i].height = 1;
    }
    
}
void delo2d_define_sprite(Sprite *sprite, float dx, float dy,float dw, float dh,float sx, float sy,float sw, float sh,unsigned int texture_index, unsigned int texture_width, unsigned int texture_height, unsigned int stride,unsigned int frames, float duration)
{
    sprite->frame = 0;
    sprite->time = 0;
    sprite->tex_coords_updated = 0;
    sprite->duration = duration;
    sprite->rect_des.x = dx;
    sprite->rect_des.y = dy;
    sprite->rect_des.width = dw;
    sprite->rect_des.height = dh;

    sprite->rect_src.x = sx;
    sprite->rect_src.y = sy;
    sprite->rect_src.width = sw;
    sprite->rect_src.height = sh;

    sprite->quad_index = 0;
    sprite->stride = stride;
    sprite->frames = frames;
    sprite->texture_index = texture_index;
    sprite->texture_width = texture_width;
    sprite->texture_height = texture_height;
}
void delo2d_sprite_batch_to_vertex_array(SpriteBatch *sprite_batch,VertexArray *vertex_array)
{    
    int length = sprite_batch->capacity;
    float color[4] = {0.0f,1.0f,0.0f,1.0f};
    for(int i = 0; i < length; i++)
    {        
        delo2d_define_quad(vertex_array,i,&(sprite_batch->rect_des)[i],&(sprite_batch->rect_src_normalized)[i],sprite_batch->texture_index[i],color);
    }    
}
void delo2d_sprite_batch_update_tex_coords(VertexArray *vertex_array,SpriteBatch *sprite_batch, Sprite *sprite,int index)
{
    sprite_batch->rect_src_normalized[index].x = sprite->rect_src.x / sprite->texture_width;
    sprite_batch->rect_src_normalized[index].y = sprite->rect_src.y / sprite->texture_height;
    sprite_batch->rect_src_normalized[index].width = (sprite->rect_src.width) / sprite->texture_width;
    sprite_batch->rect_src_normalized[index].height = (sprite->rect_src.height) / sprite->texture_height;

    int n = index * 4;
    unsigned int texture_index = sprite[index].texture_index;
    delo2d_vertex_set_tex_data(vertex_array,n,    sprite_batch->rect_src_normalized[index].x,sprite_batch->rect_src_normalized[index].y,texture_index);
    delo2d_vertex_set_tex_data(vertex_array,n + 1,sprite_batch->rect_src_normalized[index].x + sprite_batch->rect_src_normalized[index].width,sprite_batch->rect_src_normalized[index].y,texture_index);
    delo2d_vertex_set_tex_data(vertex_array,n + 2,sprite_batch->rect_src_normalized[index].x + sprite_batch->rect_src_normalized[index].width,sprite_batch->rect_src_normalized[index].y + sprite_batch->rect_src_normalized[index].height,texture_index);
    delo2d_vertex_set_tex_data(vertex_array,n + 3,sprite_batch->rect_src_normalized[index].x,sprite_batch->rect_src_normalized[index].y + sprite_batch->rect_src_normalized[index].height,texture_index);
}
void delo2d_sprite_batch_add(SpriteBatch *sprite_batch, Sprite *sprite,int index)
{
    sprite_batch->rect_src_normalized[index].x = sprite->rect_src.x / sprite->texture_width;
    sprite_batch->rect_src_normalized[index].y = sprite->rect_src.y / sprite->texture_height;
    sprite_batch->rect_src_normalized[index].width = (sprite->rect_src.width) / sprite->texture_width;
    sprite_batch->rect_src_normalized[index].height = (sprite->rect_src.height) / sprite->texture_height;

    sprite_batch->rect_des[index].x = sprite->rect_des.x;
    sprite_batch->rect_des[index].y = sprite->rect_des.y;
    sprite_batch->rect_des[index].width = sprite->rect_des.width;
    sprite_batch->rect_des[index].height = sprite->rect_des.height;

    sprite->batch_index = index;
    sprite->quad_index = index;
    sprite_batch->texture_index[index] = sprite->texture_index;    
}
void delo2d_sprite_rotate(Sprite *sprite,float rotation,VertexArray *vertex_array)
{
    Quad quad;
    delo2d_get_quad(&quad,vertex_array,sprite->quad_index);
    delo2d_quad_rotate(&quad,rotation);
}
void delo2d_sprite_translate(Sprite *sprite,float tx,float ty,VertexArray *vertex_array)
{
    Quad quad;
    delo2d_get_quad(&quad,vertex_array,sprite->quad_index);
    delo2d_quad_translate(&quad,tx,ty);
}
void delo2d_set_quad_position(VertexArray *vertex_array, int quad_index,int x,int y,int w, int h,Rectangle *rect_src)
{
    Vector2f center;
    delo2d_set_quad_center_position(vertex_array,quad_index,&center);

    Vector2f delta;
    delta.x = center.x - x;
    delta.y = center.y - y;

    delo2d_translate_quad(vertex_array,quad_index,delta.x,delta.y);
}
void delo2d_vertex_array_to_graphics_device(VertexArray *vertex_array, GLintptr offset)
{   
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->buffer);
    glBufferSubData(GL_ARRAY_BUFFER,offset,vertex_array->count_position * sizeof(float),vertex_array->buffer_position);
}
void delo2d_vertex_array_bind(VertexArray *vertex_array)
{
    glBindVertexArray(&(vertex_array->vao));//Does this matter?
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vertex_array->ibo);
}
void delo2d_vertex_array_unbind(VertexArray *vertex_array)
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
//vertex array code end

//shader code begin
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
void delo2d_sprite_animate(Sprite *sprite,float dt,VertexArray *vertex_array)
{
    sprite->time +=0.0016f;

    if(sprite->time > sprite->duration)
    {
        sprite->time = 0;
    }

    sprite->frame = (sprite->time / sprite->duration)*(float)sprite->frames;

    sprite->rect_src.x = (sprite->frame % sprite->stride)* sprite->rect_src.width;
    
    sprite->rect_src.y = (sprite->frame/sprite->stride) * sprite->rect_src.height;

    sprite->tex_coords_updated = 1;    
}
void delo2d_camera_move(float *ortho_proj, float tx, float ty)
{
    ortho_proj[3] += tx/screen_width;
    ortho_proj[7] -= ty/screen_height;
}
void delo2d_camera_set_position(float *ortho_proj, float x, float y)
{
    ortho_proj[3] = x/screen_width;
    ortho_proj[7] = y/screen_height;
}
void delo2d_camera_set_zoom(float *ortho_proj, float z)
{    
    ortho_proj[0] = z/screen_width;
    ortho_proj[5] = z/(-screen_height);   
}
void delo2d_input_update(GLFWwindow *window, KeyboardInput *ki,KeyboardInput *ki_prev)
{
    ki->move_up = glfwGetKey(window, GLFW_KEY_W);
    ki->move_l = glfwGetKey(window, GLFW_KEY_A);
    ki->move_dn = glfwGetKey(window, GLFW_KEY_S);
    ki->move_r = glfwGetKey(window, GLFW_KEY_D);

    ki_prev->move_up = ki->move_up;
    ki_prev->move_l = ki->move_l;
    ki_prev->move_dn = ki->move_dn;
    ki_prev->move_r = ki->move_r;
}   
void delo2d_input_init(KeyboardInput *ki,KeyboardInput *ki_prev)
{
    ki_prev->move_up = GLFW_RELEASE;
    ki_prev->move_l = GLFW_RELEASE;
    ki_prev->move_dn = GLFW_RELEASE;
    ki_prev->move_r = GLFW_RELEASE;
}  