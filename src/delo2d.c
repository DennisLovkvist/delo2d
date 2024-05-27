/**
 * Author:    Dennis Lövkvist
 * Created:   2022-08-05
 * Version:   1.0
 * 
 * Version  Author   Date           Change
 * 1.0      delo     2024-01-17     New feature: SpriteFont
 * 1.0      delo     2024-01-18     New feature: Render Target
 * 1.0      delo     2024-05-12     Improvement: SpriteFont - From developing GUI components
 * 1.0      delo     2024-05-18     Improvement: Added Error handling when loading
 **/

#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION

/*System Headers*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <wchar.h>
#include <float.h>
#include <locale.h>
#include <math.h>
#if defined(WIN32) || defined(__WIN32) || defined(__WIN32__)
    #include <malloc.h>
#else
    #include <alloca.h>
#endif
/*Third-Party Headers*/
#include <vendor/stb_image.h>
#include <freetype/ft2build.h>
#include <freetype/freetype.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
/*Custom Headers*/
#include <delo2d.h>


void GLClearError()
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
void GLCheckError()
{
    GLenum error;
    while((error = glGetError()))
    {
        printf("%i",error);
        printf("%c",'\n');
    }
}
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    printf("OpenGL Debug Message:\n");
    printf("    Source: %d\n", source);
    printf("    Type: %d\n", type);
    printf("    ID: %d\n", id);
    printf("    Severity: %d\n", severity);
    printf("    Message: %s\n", message);
    printf("\n");
}
int delo2d_render_setup(void **window, unsigned int width, unsigned int height,const char *title)
{
    GLFWwindow **glfw_window = (GLFWwindow**)window;
    if (!glfwInit()){return -1;} 

    *glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!*glfw_window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(*glfw_window);
    
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
void delo2d_rectangle_set(Rectangle *rectengle, int x, int y,int width, int height)
{
    rectengle->x = x;
    rectengle->y = y;
    rectengle->width = width;
    rectengle->height = height;
}
//region render target begin
void delo2d_render_target_create(RenderTarget *rt,float screen_width,float screen_height, float x, float y, float width, float height)
{
    rt->projection = matrix44_orthographic_projection(0.0f,(float)screen_width,(float)screen_height,0.0f,1,-1);

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

	// Create Framebuffer Texture
	glGenTextures(1, &rt->fbt);
	glBindTexture(GL_TEXTURE_2D, rt->fbt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width,  height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->fbt, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    rt->initialized = 1;
}
void delo2d_render_target_delete(RenderTarget *rt)
{
    if(rt->initialized == 0)return;
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glDeleteVertexArrays(1, &rt->vao);
    glDeleteBuffers(1,&rt->vbo);
    glDeleteFramebuffers(2,&rt->fbo);
	glDeleteTextures(1, &rt->fbt);
    rt->initialized = 0;
}
void delo2d_render_target_draw(RenderTarget *render_target, unsigned int shader_id)
{   
    glBindVertexArray(render_target->vao); 
    delo2d_texture_bind(render_target->fbt,0); 
    glUseProgram(shader_id); 
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void delo2d_render_target_set(unsigned int frame_buffer,float r, float g, float b, float a)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    if(frame_buffer == 0)
    {
        glEnable(GL_MULTISAMPLE);
        glSampleCoverage(0.8, GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    }
    else
    {
        glEnable(GL_MULTISAMPLE);
        glSampleCoverage(0.8, GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
}
//region rendering end

//texture code begin
uint8_t delo2d_texture_load(Texture *texture, char file_path[])
{
    stbi_set_flip_vertically_on_load(0);
    texture->local_buffer = stbi_load(file_path, &texture->width, &texture->height, &texture->bytes_per_pixel, 4);

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->local_buffer);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(texture->local_buffer);

    texture->initialized = 1;
    return DELO_SUCCESS;
}

void delo2d_texture_delete(Texture *texture)
{
    if(texture->initialized == 0)return;
    glBindTexture(GL_TEXTURE_2D,0);
    glDeleteTextures(1,&texture->renderer_id);
    texture->initialized = 0;    
}
void delo2d_texture_bind(unsigned int texture, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D,texture);
}
void delo2d_texture_unbind()
{    
    glBindTexture(GL_TEXTURE_2D,0);
}
void delo2d_texture_copy(Texture *texture_src,Texture *texture_des)
{
    texture_des->bytes_per_pixel = texture_src->bytes_per_pixel;
    texture_des->width = texture_src->width;
    texture_des->height = texture_src->height;
    texture_des->renderer_id = texture_src->renderer_id;
    texture_des->local_buffer = texture_src->local_buffer;
    texture_des->initialized = texture_src->initialized;
}
//texture code end

//region quads begin
void delo2d_quad_get(Quad *quad, VertexArray *vertex_array, int element_index)
{    
    int quad_index = element_index*vertex_array->layout_float_count*4;
    int stride = vertex_array->layout_float_count;

    quad->v0.x = &(vertex_array->buffer_position[quad_index]);
    quad->v0.y = &(vertex_array->buffer_position[quad_index+1]);

    quad->v1.x = &(vertex_array->buffer_position[quad_index + (stride)]);
    quad->v1.y = &(vertex_array->buffer_position[quad_index + (stride+1)]); 

    quad->v2.x = &(vertex_array->buffer_position[quad_index + (stride*2)]);
    quad->v2.y = &(vertex_array->buffer_position[quad_index + (stride*2+1)]); 

    quad->v3.x = &(vertex_array->buffer_position[quad_index + (stride*3)]);
    quad->v3.y = &(vertex_array->buffer_position[quad_index + (stride*3+1)]); 
}
void delo2d_quad_define(VertexArray *vertex_array, int quad_index, Rectangle_f *rect_des,Rectangle_f *rect_src, int texture_index,int texture_width, int texture_height,Color color,int flip_horizontally,int flip_vertically)
{
    int n = quad_index * 4;

    delo2d_sprite_vertex_set_element_sprite(vertex_array,n,rect_des->x,rect_des->y,(rect_src->x + rect_src->width*flip_horizontally),(rect_src->y + rect_src->height*flip_vertically),texture_width,texture_height,texture_index,color);
    delo2d_sprite_vertex_set_element_sprite(vertex_array,n + 1,rect_des->x + rect_des->width,rect_des->y,rect_src->x + rect_src->width * !flip_horizontally,rect_src->y + rect_src->height * flip_vertically,texture_width,texture_height,texture_index,color);
    delo2d_sprite_vertex_set_element_sprite(vertex_array,n + 2,rect_des->x + rect_des->width,rect_des->y + rect_des->height,rect_src->x + rect_src->width * !flip_horizontally,rect_src->y + rect_src->height * !flip_vertically,texture_width,texture_height,texture_index,color);    
    delo2d_sprite_vertex_set_element_sprite(vertex_array,n + 3,rect_des->x,rect_des->y + rect_des->height,rect_src->x + rect_src->width*flip_horizontally ,rect_src->y + rect_src->height * !flip_vertically,texture_width,texture_height,texture_index,color);

}
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
void delo2d_quad_set_position(Quad *quad,int x,int y)
{
    Vector2f center;
    delo2d_quad_get_center(quad,&center);

    Vector2f delta;
    delta.x = x-center.x;
    delta.y = y-center.y;

    delo2d_quad_translate(quad,delta.x,delta.y);
}
void delo2d_quad_get_center(Quad *quad,Vector2f *center)
{
    center->x = (*quad->v0.x + *quad->v1.x + *quad->v2.x + *quad->v3.x)*0.25f;
    center->y = (*quad->v0.y + *quad->v1.y + *quad->v2.y + *quad->v3.y)*0.25f;
}
void delo2d_quad_rotate(Quad *quad, float theta)
{
    Vector2f center;
    center.x = (*(quad->v0.x) + *(quad->v1.x) + *(quad->v2.x) + *(quad->v3.x))/4;
    center.y = (*(quad->v0.y) + *(quad->v1.y) + *(quad->v2.y) + *(quad->v3.y))/4;

    Matrix44 transform = matrix44_identity();
    transform = matrix44_multiply(transform,matrix44_translation(-center.x,-center.y,0));
    transform = matrix44_multiply(transform,matrix44_rotation_z(theta));
    transform = matrix44_multiply(transform,matrix44_translation(center.x,center.y,0));

    matrix44_multilpy_vector2fp(&quad->v0, transform);
    matrix44_multilpy_vector2fp(&quad->v1, transform);
    matrix44_multilpy_vector2fp(&quad->v2, transform);
    matrix44_multilpy_vector2fp(&quad->v3, transform);
}
void delo2d_quad_rotate_around_point(Quad *quad, float theta,float point_x, float point_y)
{
    
    
    delo2d_quad_translate(quad,-point_x,-point_y);

    Matrix44 transform = matrix44_rotation_z(theta);

    matrix44_multilpy_vector2fp(&quad->v0, transform);
    matrix44_multilpy_vector2fp(&quad->v1, transform);
    matrix44_multilpy_vector2fp(&quad->v2, transform);
    matrix44_multilpy_vector2fp(&quad->v3, transform);

    delo2d_quad_translate(quad,point_x,point_y);

}
void delo2d_quad_scale(Quad *quad,float scale_x,float scale_y)
{
    float dx = *quad->v0.x - *quad->v1.x;
    float diff = (dx-(dx*scale_x))/2;
    *quad->v0.x -= diff;
    *quad->v1.x += diff;
    *quad->v3.x -= diff;
    *quad->v2.x += diff;

    float dy = *quad->v0.y - *quad->v3.y;
    diff = (dy-(dy*scale_y))/2;
    *quad->v0.y -= diff;
    *quad->v3.y += diff;
    *quad->v1.y -= diff;
    *quad->v2.y += diff;

}
void delo2d_quad_skew(Quad *quad,float skew_x,float skew_y,float pivot_point_x,float pivot_point_y)
{
    Vector2f center;
    delo2d_quad_get_center(quad,&center);

    *quad->v0.x -= skew_x * (center.y + pivot_point_y-(*quad->v0.y));
    *quad->v1.x -= skew_x * (center.y + pivot_point_y-(*quad->v1.y));

    *quad->v2.x -= skew_x * (center.y + pivot_point_y-(*quad->v2.y));
    *quad->v3.x -= skew_x * (center.y + pivot_point_y-(*quad->v3.y));

    *quad->v0.y -= skew_y * (center.x + pivot_point_x-(*quad->v0.x));
    *quad->v3.y -= skew_y * (center.x + pivot_point_x-(*quad->v3.x));

    *quad->v1.y += skew_y * (center.x + pivot_point_x-(*quad->v1.x));
    *quad->v2.y += skew_y * (center.x + pivot_point_x-(*quad->v2.x));
}
//region quads end

//vertex array code begin
void delo2d_sprite_vertex_array_create(VertexArray *vertex_array,unsigned int type, unsigned int element_count)
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

    glGenVertexArrays(1,&vertex_array->vao); 
    glBindVertexArray(vertex_array->vao); 

    glGenBuffers(1,&vertex_array->vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vbo);
    glBufferData(GL_ARRAY_BUFFER,vertex_array->count_position * sizeof(float),NULL,GL_DYNAMIC_DRAW);     
    glEnableVertexAttribArray(0);//vertex position float2
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, sizeof(float)*vertex_array->layout_float_count,0);    
    glEnableVertexAttribArray(1);//texture coordinate float2
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, sizeof(float)*vertex_array->layout_float_count,(GLvoid*)(sizeof(float)*2));    
    glEnableVertexAttribArray(2);//texture index float
    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*vertex_array->layout_float_count,(GLvoid*)(4 * sizeof(GLfloat)));    
    glEnableVertexAttribArray(3);//color float4
    glVertexAttribPointer(3,4,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*vertex_array->layout_float_count,(GLvoid*)(5 * sizeof(GLfloat)));

    glGenBuffers(1,&vertex_array->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vertex_array->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertex_array->count_index * sizeof(GLuint),vertex_array->buffer_index,GL_DYNAMIC_DRAW);  

    glBindVertexArray(0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    vertex_array->initialized = 1;
}
void delo2d_sprite_vertex_array_delete(VertexArray *vertex_array)
{    
    if(vertex_array->initialized == 0)return;
    delo2d_sprite_vertex_array_unbind(vertex_array);

    glDeleteVertexArrays(1,&vertex_array->vao); 
    glDeleteBuffers(1,&vertex_array->vbo);
    glDeleteBuffers(1,&vertex_array->ibo);

    free(vertex_array->buffer_position);
    free(vertex_array->buffer_index);
    vertex_array->initialized = 0;
}
void delo2d_sprite_vertex_set_element_sprite(VertexArray *vertex_array, int element,float x, float y, float tex_x,float tex_y,float tex_w,float tex_h,unsigned int texture_slot,Color color)
{


    int index = element * vertex_array->layout_float_count;
    vertex_array->buffer_position[index + 0] = x;
    vertex_array->buffer_position[index + 1] = y;
    vertex_array->buffer_position[index + 2] = tex_x/tex_w;
    vertex_array->buffer_position[index + 3] = tex_y/tex_h;
    vertex_array->buffer_position[index + 4] = texture_slot;
    vertex_array->buffer_position[index + 5] = color.r;
    vertex_array->buffer_position[index + 6] = color.g;
    vertex_array->buffer_position[index + 7] = color.b;
    vertex_array->buffer_position[index + 8] = color.a;
}
void delo2d_sprite_vertex_set_tex_data(VertexArray *vertex_array, int element,float tex_x,float tex_y,unsigned int texture_slot)
{
    int index = element * vertex_array->layout_float_count;
    vertex_array->buffer_position[index + 2] = tex_x;
    vertex_array->buffer_position[index + 3] = tex_y;
    vertex_array->buffer_position[index + 4] = texture_slot;
}
void delo2d_sprite_vertex_array_draw(VertexArray *vertex_array,unsigned int count_elements,unsigned int shader_id,Texture *textures,int texture_count,Matrix44 projection)
{
   
    glUseProgram(shader_id); 
    glUniformMatrix4fv(glGetUniformLocation(shader_id,"u_mvp"),1,GL_FALSE,&projection.x11);
    
    int samplers[3] = {0,1,2};
    glUniform1iv(glGetUniformLocation(shader_id,"u_textures"),3,samplers);  

    glBindVertexArray(vertex_array->vao);
     
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vbo);//Bind to update with delo2d_vertex_array_to_graphics_device
    delo2d_sprite_vertex_array_to_graphics_device(vertex_array,0);
    for (size_t i = 0; i < texture_count; i++)
    {      
        delo2d_texture_bind(textures[i].renderer_id,i); 
    }
    
    glUseProgram(shader_id); 

    int draw_index_count = vertex_array->indices_per_element * count_elements;    

    glDrawElements(GL_TRIANGLES,draw_index_count,GL_UNSIGNED_INT,NULL);

    delo2d_texture_unbind();

}
void delo2d_sprite_vertex_array_to_graphics_device(VertexArray *vertex_array, GLintptr offset)
{   
    //delo2d_vertex_array_bind(vertex_array);
    glBufferSubData(GL_ARRAY_BUFFER,offset,vertex_array->count_position * sizeof(float),vertex_array->buffer_position);
}
void delo2d_sprite_vertex_array_bind(VertexArray *vertex_array)
{
    glBindVertexArray(vertex_array->vao);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vertex_array->ibo);
}
void delo2d_sprite_vertex_array_unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0); 
}
//vertex array code end

//shader code begin
void checkShaderCompileStatus(GLuint shader)
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
        printf("Shader compiled successfully.\n");
    }
}
uint8_t delo2d_shader_compile(uint32_t type, char *shader_source_code, uint32_t *id)
{
    GLClearError();
    
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
    
    checkShaderCompileStatus(*id);

    if (compile_status == GL_FALSE) 
    {
        fprintf(stderr,"Shader failed to compile\n");
        glDeleteShader(*id);
        *id = 0;
        return DELO_ERROR;
    }

    return DELO_SUCCESS;
}

uint8_t delo2d_shader_create(char *vertex_shader_source_code, char *fragment_shader_source_code, uint32_t *program)
{
    GLClearError();

    *program = glCreateProgram();

    if (*program == 0) 
    {
        fprintf(stderr, "Error creating shader program\n");
        return DELO_ERROR;
    }

    uint32_t vs;
    uint32_t fs;

    if (delo2d_shader_compile(GL_VERTEX_SHADER, vertex_shader_source_code, &vs) == DELO_ERROR) 
    {
        printf("Could not compiler vertex shader.\n");
        glDeleteProgram(*program);
        return DELO_ERROR;
    }
    if (delo2d_shader_compile(GL_FRAGMENT_SHADER, fragment_shader_source_code, &fs) == DELO_ERROR) 
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


uint8_t delo2d_shader_load(const char *path, char **source_code)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) 
    {
        fprintf(stderr, "Error opening file %s\n", path);
        return 0;
    }
    
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    *source_code = (char*)malloc(fsize + 1); // Allocate memory for source code
    if (*source_code == NULL) 
    {
        fprintf(stderr, "Error allocating memory\n");
        fclose(f);
        return 0;
    }
    
    fread(*source_code, fsize, 1, f);
    fclose(f);
    (*source_code)[fsize] = '\0'; // Null-terminate the string
    
    return 1; // Return success
}
uint8_t delo2d_parse_shader(const char *file_content, char **output_string, const char *keyword1, const char *keyword2) 
{
    char *start_ptr, *end_ptr;
    size_t len;

    // Find the first occurrence of keyword1
    start_ptr = strstr(file_content, keyword1);
    if (start_ptr == NULL) {
        *output_string = NULL;
        return DELO_ERROR;
    }

    // Move the pointer past keyword1
    start_ptr += strlen(keyword1);

    // Find the first occurrence of keyword2 after keyword1
    end_ptr = strstr(start_ptr, keyword2);
    if (end_ptr == NULL) {
        *output_string = NULL;
        return DELO_ERROR;
    }

    // Calculate the length of the content between keyword1 and keyword2
    len = end_ptr - start_ptr;

    // Allocate memory for the output string
    *output_string = (char *)malloc(len + 1);
    if (*output_string == NULL) {
        // Memory allocation failed
        return DELO_ERROR;
    }

    // Copy the content between keyword1 and keyword2 to the output string
    strncpy(*output_string, start_ptr, len);

    // Null-terminate the output string
    (*output_string)[len] = '\0';

    return DELO_SUCCESS;
}
uint8_t delo2d_shader_from_file(char *path_shader, uint32_t *shader_id)
{
    *shader_id = 0;
    char *source_code;
    
    if(delo2d_shader_load(path_shader,&source_code) == DELO_ERROR)
    {
        printf("[delo2d] Could not load shader file %s\n",path_shader);
        return DELO_ERROR;
    }
    char *src_vert = NULL;
    char *src_frag = NULL;  
 
    uint8_t status = DELO_SUCCESS;

    status = delo2d_parse_shader(source_code,&src_vert,"#VERT_BEGIN","#VERT_END");

    if(status == DELO_SUCCESS)
    {
        status = delo2d_parse_shader(source_code,&src_frag,"#FRAG_BEGIN","#FRAG_END");
    }
    if(status == DELO_SUCCESS)
    {
        status = delo2d_shader_create(src_vert,src_frag,shader_id);
    }
    
    
    if(status == DELO_ERROR)
    {
        if(src_vert    != NULL)free(src_vert);
        if(src_frag    != NULL)free(src_frag);
        if(source_code != NULL)free(source_code);

        return DELO_ERROR;
    }

    return DELO_SUCCESS;   
}
//shader code end

//spritebatch code begin
void delo2d_sprite_batch_create(SpriteBatch *sprite_batch,int capacity)
{

    delo2d_sprite_vertex_array_create(&sprite_batch->vertex_array,DELO_QUAD_LIST,capacity);

    sprite_batch->capacity = capacity;
    sprite_batch->count = 0;
    sprite_batch->rect_des = malloc(sizeof(Rectangle_f)*capacity);
    sprite_batch->rect_src = malloc(sizeof(Rectangle_f)*capacity);
    sprite_batch->texture_index = malloc(sizeof(unsigned int)*capacity);
    sprite_batch->quad_index = malloc(sizeof(unsigned int)*capacity);
    sprite_batch->flip_horizontally = malloc(sizeof(unsigned int)*capacity);
    sprite_batch->flip_vertically = malloc(sizeof(unsigned int)*capacity);
    sprite_batch->updated = malloc(sizeof(unsigned int)*capacity);
    sprite_batch->color = malloc(sizeof(Color)*capacity);
    sprite_batch->scale = malloc(sizeof(Vector2f)*capacity);
    sprite_batch->skew = malloc(sizeof(Vector2f)*capacity);
    sprite_batch->position = malloc(sizeof(Vector2f)*capacity);
    sprite_batch->pivot_point = malloc(sizeof(Vector2f)*capacity);
    sprite_batch->orientation = malloc(sizeof(float)*capacity);
    sprite_batch->textures = malloc(sizeof(Texture)*32);
    sprite_batch->texture_count = 0;
    sprite_batch->called_begin = 0;
    sprite_batch->called_end = 1;
    sprite_batch->projection = matrix44_orthographic_projection(0.0f,1920,0.0f,1080,1,-1); 

    for (size_t i = 0; i < capacity; i++)
    {
        sprite_batch->texture_index[i] = 0;
        sprite_batch->quad_index[i] = 0;
        sprite_batch->rect_des[i].x = sprite_batch->rect_des[i].y = 0;
        sprite_batch->rect_des[i].width = sprite_batch->rect_des[i].height = 100;

        sprite_batch->rect_src[i].x = sprite_batch->rect_src[i].y = 0;
        sprite_batch->rect_src[i].width = sprite_batch->rect_src[i].height = 1;
        sprite_batch->color[i].r = sprite_batch->color[i].g = sprite_batch->color[i].b = sprite_batch->color[i].a = 0;
        sprite_batch->flip_horizontally[i] = sprite_batch->flip_vertically[i] = 0;
        sprite_batch->scale[i].x = sprite_batch->scale[i].y = 1;
        sprite_batch->skew[i].x = sprite_batch->skew[i].y = 0;
        sprite_batch->pivot_point[i].x = sprite_batch->pivot_point[i].y = 0;
        sprite_batch->position[i].x = sprite_batch->position[i].y = 0;
        sprite_batch->updated[i] = 1;
    }    
    sprite_batch->initialized = 1;
}
void delo2d_sprite_batch_add_no_texture(SpriteBatch *sprite_batch, Sprite *sprite)
{
    int texture_index = -1;

    int index = sprite_batch->count;
    sprite_batch->count ++;
    
    sprite_batch->rect_src[index].x = ((float)sprite->rect_src.x);
    sprite_batch->rect_src[index].y = ((float)sprite->rect_src.y);
    sprite_batch->rect_src[index].width = ((float)sprite->rect_src.width);
    sprite_batch->rect_src[index].height = ((float)sprite->rect_src.height);
   

    sprite_batch->rect_des[index].x = sprite->rect_des.x;
    sprite_batch->rect_des[index].y = sprite->rect_des.y;
    sprite_batch->rect_des[index].width = sprite->rect_des.width;
    sprite_batch->rect_des[index].height = sprite->rect_des.height;

    sprite->batch_index = index;
    sprite->quad_index = index;
    sprite_batch->texture_index[index] = texture_index; 
    sprite_batch->color[index].r = sprite->color.r;
    sprite_batch->color[index].g = sprite->color.g;
    sprite_batch->color[index].b = sprite->color.b;
    sprite_batch->color[index].a = sprite->color.a;
    sprite_batch->flip_horizontally[index] = sprite->flip_horizontally;
    sprite_batch->flip_vertically[index] = sprite->flip_vertically;

    sprite_batch->scale[index].x = sprite->scale.x;
    sprite_batch->scale[index].y = sprite->scale.y;

    sprite_batch->skew[index].x = sprite->skew.x;
    sprite_batch->skew[index].y = sprite->skew.y;

    sprite_batch->pivot_point[index].x = sprite->pivot_point.x;
    sprite_batch->pivot_point[index].y = sprite->pivot_point.y;

    sprite_batch->position[index].x = sprite->position.x;
    sprite_batch->position[index].y = sprite->position.y;

    sprite_batch->orientation[index] = sprite->orientation;
    sprite_batch->updated[index] = 1;
}
void delo2d_sprite_batch_delete(SpriteBatch *sprite_batch)
{
    if(sprite_batch->initialized == 0)return;

    free(sprite_batch->rect_des);
    free(sprite_batch->rect_src);
    free(sprite_batch->texture_index);
    free(sprite_batch->quad_index);
    free(sprite_batch->flip_horizontally);
    free(sprite_batch->flip_vertically);
    free(sprite_batch->updated);
    free(sprite_batch->color);
    free(sprite_batch->scale);
    free(sprite_batch->skew);
    free(sprite_batch->position);
    free(sprite_batch->pivot_point);
    free(sprite_batch->orientation);
    free(sprite_batch->textures);

    delo2d_sprite_vertex_array_delete(&sprite_batch->vertex_array);

    sprite_batch->initialized = 0;
}
void delo2d_sprite_batch_begin(SpriteBatch *sprite_batch,unsigned int shader, Matrix44 projection)
{
    if(sprite_batch->called_end == 0)return;

    sprite_batch->shader_id = shader;
    sprite_batch->projection = projection;

    sprite_batch->called_begin = 1;
}
void delo2d_sprite_batch_end(SpriteBatch *sprite_batch)
{
    if(sprite_batch->called_begin == 0)return;
    
    delo2d_sprite_batch_to_vertex_array(sprite_batch,&sprite_batch->vertex_array);     

    delo2d_sprite_vertex_array_draw(&sprite_batch->vertex_array,sprite_batch->count,sprite_batch->shader_id,sprite_batch->textures,sprite_batch->texture_count,sprite_batch->projection);

    sprite_batch->called_end = 1;
    sprite_batch->texture_count = 0;
    sprite_batch->count = 0;
    
}
void delo2d_sprite_define(Sprite *sprite, int dx, int dy,int dw, int dh,int sx, int sy,int sw, int sh,unsigned int texture_index, unsigned int texture_width, unsigned int texture_height, unsigned int stride,unsigned int frames, float duration, Color color,float scale_x,float scale_y,float skew_x,float skew_y,unsigned int flip_horizontally,unsigned int flip_vertically)
{
    sprite->frame = 0;
    sprite->time = 0;
    sprite->updated_tex_coords = 0;
    sprite->duration = duration;
    sprite->rect_des.x = dx;
    sprite->rect_des.y = dy;
    sprite->rect_des.width = dw;
    sprite->rect_des.height = dh;

    sprite->position.x = dx + dw*0.5f;
    sprite->position.y = dy + dh*0.5f;

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
    sprite->color.r = color.r;
    sprite->color.g = color.g;
    sprite->color.b = color.b;
    sprite->color.a = color.a;
    sprite->flip_horizontally = flip_horizontally;
    sprite->flip_vertically = flip_vertically;
    sprite->updated_tex_coords = 0;
    sprite->orientation = 0;
    sprite->scale.x = scale_x;
    sprite->scale.y = scale_y;
    sprite->skew.x = skew_x;
    sprite->skew.y = skew_y;
    sprite->offset.x = 0;
    sprite->offset.y = 0;
    sprite->pivot_point.x = sprite->pivot_point.y = 0;
    sprite->loop = 1;
}
void delo2d_sprite_batch_add(SpriteBatch *sprite_batch, Sprite *sprite,Texture *texture)
{
    int texture_index = -1;
    delo2d_sprite_batch_add_texture(sprite_batch,texture, &texture_index);

    int index = sprite_batch->count;
    sprite_batch->count ++;
    
    sprite_batch->rect_src[index].x = ((float)sprite->rect_src.x);
    sprite_batch->rect_src[index].y = ((float)sprite->rect_src.y);
    sprite_batch->rect_src[index].width = ((float)sprite->rect_src.width);
    sprite_batch->rect_src[index].height = ((float)sprite->rect_src.height);
   

    sprite_batch->rect_des[index].x = sprite->rect_des.x;
    sprite_batch->rect_des[index].y = sprite->rect_des.y;
    sprite_batch->rect_des[index].width = sprite->rect_des.width;
    sprite_batch->rect_des[index].height = sprite->rect_des.height;

    sprite->batch_index = index;
    sprite->quad_index = index;
    sprite_batch->texture_index[index] = texture_index; 
    sprite_batch->color[index].r = sprite->color.r;
    sprite_batch->color[index].g = sprite->color.g;
    sprite_batch->color[index].b = sprite->color.b;
    sprite_batch->color[index].a = sprite->color.a;
    sprite_batch->flip_horizontally[index] = sprite->flip_horizontally;
    sprite_batch->flip_vertically[index] = sprite->flip_vertically;

    sprite_batch->scale[index].x = sprite->scale.x;
    sprite_batch->scale[index].y = sprite->scale.y;

    sprite_batch->skew[index].x = sprite->skew.x;
    sprite_batch->skew[index].y = sprite->skew.y;

    sprite_batch->pivot_point[index].x = sprite->pivot_point.x;
    sprite_batch->pivot_point[index].y = sprite->pivot_point.y;

    sprite_batch->position[index].x = sprite->position.x;
    sprite_batch->position[index].y = sprite->position.y;

    sprite_batch->orientation[index] = sprite->orientation;
    sprite_batch->updated[index] = 1;
}
void delo2d_sprite_batch_add_texture(SpriteBatch *sprite_batch,Texture *texture, int *texture_index)
{
    
    *texture_index = -1;
    for (size_t i = 0; i < sprite_batch->texture_count; i++)
    {
        if(sprite_batch->textures[i].renderer_id == texture->renderer_id)
        {
            *texture_index = i;
            break;
        }
    }
    if(*texture_index == -1)
    { 
        delo2d_texture_copy(texture,&sprite_batch->textures[sprite_batch->texture_count]);
        *texture_index = sprite_batch->texture_count;
        sprite_batch->texture_count ++;
    }
}
void delo2d_sprite_batch_to_vertex_array(SpriteBatch *sprite_batch,VertexArray *vertex_array)
{    
    int length = sprite_batch->count;

    for(int i = 0; i < length; i++)
    {
        int texture_index = sprite_batch->texture_index[i];

        Rectangle_f rect_src = sprite_batch->rect_src[i];

        int texture_width = (texture_index != -1) ? sprite_batch->textures[texture_index].width : (int)(rect_src.width);
        int texture_height = (texture_index != -1) ? sprite_batch->textures[texture_index].height : (int)(rect_src.height);

        delo2d_quad_define(vertex_array,i,&(sprite_batch->rect_des)[i],&(sprite_batch->rect_src)[i],sprite_batch->texture_index[i],texture_width,texture_height,sprite_batch->color[i],sprite_batch->flip_horizontally[i],sprite_batch->flip_vertically[i]);
        Quad quad;
        delo2d_quad_get(&quad,vertex_array,i);

        delo2d_quad_set_position(&quad,sprite_batch->position[i].x,sprite_batch->position[i].y);
        
        sprite_batch->updated[i] = 0;
        //delo2d_quad_skew_top(&quad,sprite_batch->skew[i].x);
        
        delo2d_quad_skew(&quad,sprite_batch->skew[i].x,sprite_batch->skew[i].y,sprite_batch->pivot_point[i].x,sprite_batch->pivot_point[i].y);

        delo2d_quad_scale(&quad,sprite_batch->scale[i].x,sprite_batch->scale[i].y);

        Vector2f center;
        delo2d_quad_get_center(&quad,&center);
        delo2d_quad_rotate_around_point(&quad,sprite_batch->orientation[i], center.x + sprite_batch->pivot_point[i].x,center.y + sprite_batch->pivot_point[i].y);       
    }    
}
void delo2d_sprite_scale_dest_rect(Sprite *sprite, float scale_x, float scale_y)
{
    sprite->rect_des.width *= scale_x;
    sprite->rect_des.height *= scale_y;
    sprite->position.x = sprite->rect_des.x+sprite->rect_des.width*scale_x;
    sprite->position.y = sprite->rect_des.y+sprite->rect_des.height*scale_y;
} 
void delo2d_sprite_rotate(Sprite *sprite,float rotation,VertexArray *vertex_array)
{
    Quad quad;
    delo2d_quad_get(&quad,vertex_array,sprite->quad_index);
    delo2d_quad_rotate(&quad,rotation);
    sprite->orientation += rotation;
}
void delo2d_sprite_set_orientation(Sprite *sprite,float orientation,VertexArray *vertex_array)
{
    float delta = orientation - sprite->orientation;
    delo2d_sprite_rotate(sprite,delta,vertex_array);
}
void delo2d_sprite_translate(Sprite *sprite,float tx,float ty,VertexArray *vertex_array)
{
    Quad quad;
    delo2d_quad_get(&quad,vertex_array,sprite->quad_index);
    delo2d_quad_translate(&quad,tx,ty);

    sprite->position.x += tx;
    sprite->position.y += ty;

    //delo2d_quad_get_center(&quad,&sprite->position);    
}
void delo2d_sprite_animate_frame(Sprite *sprite,int frame)
{
    if(frame >= (int)sprite->frames)
    {
        frame = 0;
    }
    else if(frame < 0)
    {
        frame = sprite->frames-1;
    }
    sprite->frame = frame;
    sprite->time = (float)sprite->frame / sprite->duration;
    sprite->rect_src.x = sprite->offset.x + (sprite->frame % sprite->stride) * sprite->rect_src.width;
    
    sprite->rect_src.y = sprite->offset.y + (sprite->frame/sprite->stride) * sprite->rect_src.height;

    sprite->updated_tex_coords = 1;      
}
void delo2d_sprite_animate(Sprite *sprite,float dt, unsigned int reverse)
{
    if(!reverse)
    {
        sprite->time += dt;

        if(sprite->time > sprite->duration)
        {
            if(sprite->loop == 1)
            {
                sprite->time = 0;
            }
            else
            {
                return;
            }
        }
    }
    else
    {        
        sprite->time -= dt;
        if(sprite->time < 0)
        {
            if(sprite->loop == 1)
            {
                sprite->time = sprite->duration;
            }
            else
            {
                return;
            }
        }
    }

    sprite->frame = (sprite->time / sprite->duration)*(float)sprite->frames;

    sprite->frame = (sprite->frame == sprite->frames) ? sprite->frames - 1:sprite->frame;

    sprite->rect_src.x = sprite->offset.x + (sprite->frame % sprite->stride) * sprite->rect_src.width;
    
    sprite->rect_src.y = sprite->offset.y + (sprite->frame/sprite->stride) * sprite->rect_src.height;

    sprite->updated_tex_coords = 1;      
}
//spritebatch code end

//color code begin
void delo2d_color_set_f(Color *color,float r, float g, float b, float a)
{
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}
void delo2d_color_set_i(Color *color,int r, int g, int b, int a)
{
    color->r = (float)r/255.0f;
    color->g = (float)g/255.0f;
    color->b = (float)b/255.0f;
    color->a = (float)a/255.0f;
}
void delo2d_color_set(Color *color1,Color *color2)
{
    color1->r = color2->r;
    color1->g = color2->g;
    color1->b = color2->b;
    color1->a = color2->a;
}
void delo2d_color_lerp(Color *result, Color *color_a,Color *color_b, float facor)
{
    result->r = (color_a->r + color_b->r ) / 2; 
    result->g = (color_a->g + color_b->g ) / 2; 
    result->b = (color_a->b + color_b->b ) / 2; 
    result->a = (color_a->a + color_b->a ) / 2; 
}
//color code end

void delo2d_input_update(void *window, KeyboardInput *ki,KeyboardInput *ki_prev)
{
    window = (GLFWwindow**)window;

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

//vertex array primitives code begin
void delo2d_primitive_vertex_set_element(VertexArrayPrimitives *vertex_array, int element,float x, float y,float r,float g, float b, float a)
{
    int index = element * vertex_array->layout_float_count;
    vertex_array->buffer_position[index + 0] = x;
    vertex_array->buffer_position[index + 1] = y;
    vertex_array->buffer_position[index + 2] = r;
    vertex_array->buffer_position[index + 3] = g;
    vertex_array->buffer_position[index + 4] = b;
    vertex_array->buffer_position[index + 5] = a;

}
void delo2d_primitive_vertex_array_draw(VertexArrayPrimitives *vertex_array,unsigned int shader_id,Matrix44 projection)
{   
    glUseProgram(shader_id); 
    glUniformMatrix4fv(glGetUniformLocation(shader_id,"u_mvp"),1,GL_FALSE,&projection.x11);

    glBindVertexArray(vertex_array->vao);
     
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vbo);//Bind to update with delo2d_vertex_array_to_graphics_device
    delo2d_primitive_vertex_array_to_graphics_device(vertex_array,0);
 
    if(vertex_array->type == DELO_TRIANGLE_LIST)
    {
        glDrawArrays(GL_TRIANGLES,0,vertex_array->count);
    }
    else if (vertex_array->type == DELO_LINE_LIST)
    {
        glDrawArrays(GL_LINES,0,vertex_array->count);
    }  
    else if (vertex_array->type == DELO_QUAD_LIST)
    {
        glDrawArrays(GL_QUADS,0,vertex_array->count);
    }  
}
void delo2d_primitive_vertex_array_to_graphics_device(VertexArrayPrimitives *vertex_array, GLintptr offset)
{   
    delo2d_primitive_vertex_array_bind(vertex_array);
    glBufferSubData(GL_ARRAY_BUFFER,offset,vertex_array->count * vertex_array->layout_float_count * sizeof(float),vertex_array->buffer_position);
}
void delo2d_primitive_vertex_array_bind(VertexArrayPrimitives *vertex_array)
{
    glBindVertexArray(vertex_array->vao);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vbo);
}
void delo2d_primitive_vertex_array_unbind()
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void delo2d_primitive_vertex_array_create(VertexArrayPrimitives *vertex_array,unsigned int vertex_capacity, unsigned int shader)
{
    vertex_array->count = 0;
    vertex_array->capacity = vertex_capacity;
    vertex_array->layout_float_count = 6;
    vertex_array->buffer_position = malloc(vertex_capacity * sizeof(float));
    
    int length = vertex_capacity;
    for (size_t i = 0; i < length; i++)
    {
        vertex_array->buffer_position[i] = 0.0f;
    } 
   
    glGenVertexArrays(1,&vertex_array->vao); 
    glBindVertexArray(vertex_array->vao); 

    glGenBuffers(1,&vertex_array->vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vbo);
    glBufferData(GL_ARRAY_BUFFER,vertex_capacity * sizeof(float),NULL,GL_DYNAMIC_DRAW);   

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*vertex_array->layout_float_count,0);   
    glEnableVertexAttribArray(0);//vertex position float2    
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*vertex_array->layout_float_count,(GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);//color float4

    glUseProgram(shader);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0); 
    vertex_array->initialized = 1;
}
void delo2d_primitive_vertex_array_delete(VertexArrayPrimitives *vertex_array)
{
    if(vertex_array->initialized == 0)return;
    delo2d_sprite_vertex_array_unbind(vertex_array);

    glDeleteVertexArrays(1,&vertex_array->vao); 
    glDeleteBuffers(1,&vertex_array->vbo);

    free(vertex_array->buffer_position);
    vertex_array->initialized = 0;
}
//vertex array primitives code end
//primitive batch code begin
void delo2d_primitive_batch_create(PrimitiveBatch *primitive_batch,int capacity)
{
    primitive_batch->vertex_array.capacity = capacity;
    primitive_batch->vertex_array.count = 0;
    delo2d_primitive_vertex_array_create(&primitive_batch->vertex_array,capacity,primitive_batch->shader_id);
    primitive_batch->initialized = 1;
    primitive_batch->called_end = 1;
    primitive_batch->called_begin = 0;
}
void delo2d_primitive_batch_delete(PrimitiveBatch *primitive_batch)
{
    if(primitive_batch->initialized == 0)return;    
    primitive_batch->vertex_array.capacity = 0;
    primitive_batch->vertex_array.count = 0;
    delo2d_primitive_vertex_array_delete(&primitive_batch->vertex_array);
    primitive_batch->initialized = 0;    
}
void delo2d_primitive_batch_begin(PrimitiveBatch *primitive_batch,unsigned int shader,Matrix44 projection,unsigned int primitive_type)
{
    if(primitive_batch->called_end == 0)return;
    primitive_batch->vertex_array.type = primitive_type;
    primitive_batch->shader_id = shader;
    primitive_batch->projection = projection;

    primitive_batch->called_begin = 1;
    primitive_batch->called_end = 0;
}
void delo2d_primitive_batch_add(PrimitiveBatch *primitive_batch,int x, int y,float r,float g, float b, float a)
{
    int index = primitive_batch->vertex_array.count;
    primitive_batch->vertex_array.count ++;
    delo2d_primitive_vertex_set_element(&primitive_batch->vertex_array,index,x,y,r,g,b,a);    
}
void delo2d_primitive_batch_end(PrimitiveBatch *primitive_batch)
{
    if(primitive_batch->called_begin == 0)return;  
    delo2d_primitive_vertex_array_to_graphics_device(&primitive_batch->vertex_array,0);
    delo2d_primitive_vertex_array_draw(&primitive_batch->vertex_array,primitive_batch->shader_id,primitive_batch->projection);

    primitive_batch->called_end = 1;
    primitive_batch->called_begin = 0;
    primitive_batch->vertex_array.count = 0;
}
//primitive batch code end
//matrix code begin
Matrix44 matrix44_identity()
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

Matrix44 matrix44_scale(float x, float y, float z) 
{
    struct Matrix44 matrix = 
    {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1,
	};
    return matrix;
}
float* matrix44_to_gl_matrix(Matrix44 *matrix) 
{
	return &matrix->x11;
}
Matrix44 matrix44_translation(float x, float y, float z) 
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

Matrix44 matrix44_rotation_z(float theta) 
{
	struct Matrix44 matrix = 
    {
		 cos(theta), sin(theta), 0, 0,
		-sin(theta), cos(theta), 0, 0,
		 0,          0,          1, 0,
		 0,          0,          0, 1
	};
    return matrix;
}

Matrix44 matrix44_rotation_y(float theta) 
{
	struct Matrix44 matrix = 
    {
		cos(theta), 0, -sin(theta), 0,
		0,          1,  0,          0,
		sin(theta), 0,  cos(theta), 0,
		0,          0,  0,          1
	};
    return matrix;
}

Matrix44 matrix44_rotation_x(float theta) 
{
	struct Matrix44 matrix = 
    {
		1,  0,           0,          0,
		0,  cos(theta),  sin(theta), 0,
		0, -sin(theta),  cos(theta), 0,
		0,  0,           0,          1
	};
    return matrix;
}
Matrix44 matrix44_multiply(Matrix44 a, Matrix44 b) 
{
	struct Matrix44 matrix = 
    {
        a.x11 * b.x11 + a.x12 * b.x21 + a.x13 * b.x31 + a.x14 * b.x41,
		a.x21 * b.x11 + a.x22 * b.x21 + a.x23 * b.x31 + a.x24 * b.x41,
		a.x31 * b.x11 + a.x32 * b.x21 + a.x33 * b.x31 + a.x34 * b.x41,
		a.x41 * b.x11 + a.x42 * b.x21 + a.x43 * b.x31 + a.x44 * b.x41,

		a.x11 * b.x12 + a.x12 * b.x22 + a.x13 * b.x32 + a.x14 * b.x42,
		a.x21 * b.x12 + a.x22 * b.x22 + a.x23 * b.x32 + a.x24 * b.x42,
		a.x31 * b.x12 + a.x32 * b.x22 + a.x33 * b.x32 + a.x34 * b.x42,
		a.x41 * b.x12 + a.x42 * b.x22 + a.x43 * b.x32 + a.x44 * b.x42,
  
		a.x11 * b.x13 + a.x12 * b.x23 + a.x13 * b.x33 + a.x14 * b.x43,
		a.x21 * b.x13 + a.x22 * b.x23 + a.x23 * b.x33 + a.x24 * b.x43,
		a.x31 * b.x13 + a.x32 * b.x23 + a.x33 * b.x33 + a.x34 * b.x43,
		a.x41 * b.x13 + a.x42 * b.x23 + a.x43 * b.x33 + a.x44 * b.x43,

		a.x11 * b.x14 + a.x12 * b.x24 + a.x13 * b.x34 + a.x14 * b.x44,
		a.x21 * b.x14 + a.x22 * b.x24 + a.x23 * b.x34 + a.x24 * b.x44,
		a.x31 * b.x14 + a.x32 * b.x24 + a.x33 * b.x34 + a.x34 * b.x44,
		a.x41 * b.x14 + a.x42 * b.x24 + a.x43 * b.x34 + a.x44 * b.x44,
	};
    return matrix;
}
Matrix44 matrix44_add(Matrix44 a, Matrix44 b)
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
Matrix44 matrix44_perspective() 
{
    //Shit copied from the internet
	float r = 0.56; 
	float t = 0.33;
	float n = 1; 
	float f = 60;

	struct Matrix44 matrix = 
    {
		n / r, 0, 	  0, 					  0,
		0, 	   n / t, 0, 					  0,
		0, 	   0, 	  (-f - n) / (f - n)   , -1,
		0, 	   0, 	  (2 * f * n) / (n - f),  0
	};
    return matrix;
}
void matrix44_multilpy_vector2fp(Vector2fp *vector, Matrix44 transform)
{
    float x = ((*vector->x) * transform.x11) + ((*vector->y) * transform.x21) + (1 * transform.x31);
    float y = ((*vector->x) * transform.x12) + ((*vector->y) * transform.x22) + (1 * transform.x32);
    (*vector->x) = x;
    (*vector->y) = y;
}
Vector2f matrix44_multilpy_vector2f(Vector2f vector, Matrix44 transform)
{
    Vector2f result = 
    {
        (vector.x * transform.x11) + (vector.y * transform.x21) + (1 * transform.x31),
        (vector.x * transform.x12) + (vector.y * transform.x22) + (1 * transform.x32)
    };
    return result;
}
Matrix44 matrix44_orthographic_projection(float l,float r,float t,float b,float f,float n)
{
    struct Matrix44 matrix = 
    {
        2.0f/(r-l),0,0,-((r+l)/(r-l)),

        0,2.0/(t-b),0,-((t+b)/(t-b)),

        0,0,2 / (f-n),-((f+n)/(f-n)),
        0,0,0,1
    };
    return matrix;
}
float matrix44_calculate_sub_determinant(Matrix44 m, int a, int b, int c, int d) 
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
float matrix44_calculate_determinant(const Matrix44* m) 
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

Matrix44 matrix44_invert(Matrix44 input) 
{
    float det = matrix44_calculate_determinant(&input);

    if (det == 0.0f) 
    {
        printf("Matrix is not invertible (determinant is zero).\n");
        return input;
    }

    Matrix44 result = {0};

    result.x11 =  matrix44_calculate_sub_determinant(input, 1, 2, 3, 1) / det;
    result.x21 = -matrix44_calculate_sub_determinant(input, 0, 2, 3, 1) / det;
    result.x31 =  matrix44_calculate_sub_determinant(input, 0, 1, 3, 1) / det;
    result.x41 = -matrix44_calculate_sub_determinant(input, 0, 1, 2, 1) / det;

    result.x12 = -matrix44_calculate_sub_determinant(input, 1, 2, 3, 0) / det;
    result.x22 =  matrix44_calculate_sub_determinant(input, 0, 2, 3, 0) / det;
    result.x32 = -matrix44_calculate_sub_determinant(input, 0, 1, 3, 0) / det;
    result.x42 =  matrix44_calculate_sub_determinant(input, 0, 1, 2, 0) / det;

    result.x13 =  matrix44_calculate_sub_determinant(input, 1, 2, 3, 3) / det;
    result.x23 = -matrix44_calculate_sub_determinant(input, 0, 2, 3, 3) / det;
    result.x33 =  matrix44_calculate_sub_determinant(input, 0, 1, 3, 3) / det;
    result.x43 = -matrix44_calculate_sub_determinant(input, 0, 1, 2, 3) / det;

    result.x14 = -matrix44_calculate_sub_determinant(input, 1, 2, 3, 2) / det;
    result.x24 =  matrix44_calculate_sub_determinant(input, 0, 2, 3, 2) / det;
    result.x34 = -matrix44_calculate_sub_determinant(input, 0, 1, 3, 2) / det;
    result.x44 =  matrix44_calculate_sub_determinant(input, 0, 1, 2, 2) / det;

    return result;
}
//matrix code end

//text code begin
uint8_t delo2d_sprite_font_load(SpriteFont *sprite_font, char *path, int font_size)
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

    if(FT_Set_Pixel_Sizes(face, 0, font_size))
    {
        FT_Done_FreeType(ft);
        return DELO_ERROR;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &sprite_font->texture.renderer_id);
    glBindTexture(GL_TEXTURE_2D, sprite_font->texture.renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    sprite_font->texture.width  = 0;
    sprite_font->texture.height = 0;
    int8_t padding = 2;

    int32_t max_ascent  = 0;
    int32_t max_descent = 0;

    for (uint32_t c = 33; c < max; c++) 
    {
        
        if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP)) 
        {
            fprintf(stderr, "Failed to load glyph\n");
            continue;
        }
        else
        {
            sprite_font->texture.width += face->glyph->bitmap.width + padding;

            sprite_font->blank_space_offset_x = face->glyph->advance.x >> 6;
            sprite_font->line_spacing = face->size->metrics.height >> 6;

            FT_GlyphSlot g = face->glyph;

            int32_t ascent = g->bitmap_top;
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

    sprite_font->texture.height = face->height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sprite_font->texture.width, sprite_font->texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    GLubyte* buffer_blank = (GLubyte*)calloc(sprite_font->texture.width * sprite_font->texture.height * 4, sizeof(GLubyte));

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,sprite_font->texture.width, sprite_font->texture.height, GL_RGBA, GL_UNSIGNED_BYTE, buffer_blank);
    free(buffer_blank);

    float offset_x = 0;
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
            offset_y = max_ascent - face->glyph->bitmap_top;

            sprite_font->glyphs[index].x = offset_x;
            sprite_font->glyphs[index].y = offset_y;
            sprite_font->glyphs[index].w = face->glyph->bitmap.width;
            sprite_font->glyphs[index].h = (face->size->metrics.height >> 6);
            sprite_font->glyphs[index].advance = (face->glyph->linearHoriAdvance >> 16);
            sprite_font->glyphs[index].bearing_x = face->glyph->bitmap_left;

            index ++;
            int32_t length = face->glyph->bitmap.rows * face->glyph->bitmap.pitch;
            uint32_t *buffer = alloca(sizeof(uint32_t)*length);

            for (int32_t i = 0; i < length; i++)
            {
                char color = face->glyph->bitmap.buffer[i];
                uint32_t r = (((color) >> 24) & 0xFF);
                uint32_t g = (((color) >> 16) & 0xFF);
                uint32_t b = (((color) >> 8)  & 0xFF);
                uint32_t a = ((color)         & 0xFF);
                buffer[i]  = (((r) << 24) | ((g) << 16) | ((b) << 8) | (a));
            }
            
            glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

            offset_x += face->glyph->bitmap.width+padding;

        }
    }

    for (int32_t i = 0; i < max; i++)
    {
        int32_t x = sprite_font->glyphs[i].x;
        int32_t y = 0;
        int32_t w = sprite_font->glyphs[i].w;
        int32_t h = sprite_font->glyphs[i].h;
        delo2d_sprite_define(&sprite_font->sprites[i], 0,0,w,h,x,y,w,h,0,sprite_font->texture.width,sprite_font->texture.height,1,1,1,(Color){1,1,1,1},1,1,0,0,0,0);
    }
    sprite_font->font_size = font_size;
    sprite_font->texture.bytes_per_pixel = 4;
    sprite_font->texture.initialized = 1;

    return DELO_SUCCESS;
}
uint32_t* delo2d_sprite_font_convert_to_unicode(const char* string_utf8) 
{
    setlocale(LC_ALL, "en_US.UTF-8");
    size_t length_utf8 = strlen(string_utf8);

    size_t wchar_size = mbstowcs(NULL, string_utf8, 0);

    if (wchar_size == (size_t)-1) 
    {
        perror("mbstowcs");
        exit(EXIT_FAILURE);
    }

    wchar_t* unicode_string = (wchar_t*)malloc((wchar_size + 1) * sizeof(wchar_t));
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

    uint32_t* result = (uint32_t*)malloc((wchar_size + 1) * sizeof(uint32_t));
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
int32_t delo2d_sprite_font_set_caret_mouse(char *text,SpriteFont *sprite_font,float scale, Vector2 mp)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    Vector2 measure = {0,0};
    uint32_t* unicode_array = delo2d_sprite_font_convert_to_unicode(text);
    double min = FLT_MAX;
    int32_t min_index = 0;

    if(mp.x <= 0)
    {
        return 0;
    }

    for (size_t i = 0; unicode_array[i] != 0; ++i)
    {
        uint32_t c = unicode_array[i];

        if (c>=32 && c < 256)
        {
            Glyph *glyph = &sprite_font->glyphs[(int32_t)c-33];

            measure.x += glyph->advance*scale;
            measure.y = glyph->h*scale;
            double dist = floor(fabs(measure.x-mp.x));
            if(dist <= min)
            {
                min = dist;
                min_index = i+1;
            }

        } 
    }  
    free(unicode_array);

    return min_index;
}
Vector2 delo2d_sprite_font_measure_string(char *text,SpriteFont *sprite_font,float scale, int32_t max)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    Vector2 measure = {0,0};
    uint32_t* unicode_array = delo2d_sprite_font_convert_to_unicode(text);

    for (size_t i = 0; unicode_array[i] != 0 && (i < max || max == -1); ++i)
    {
        uint32_t c = unicode_array[i];

        if (c>=32 && c < 256)
        {
            Glyph *glyph = &sprite_font->glyphs[(int32_t)c-33];
            measure.x += glyph->advance;
            measure.y = glyph->h*scale;
        } 
    }  
    measure.x *= scale;
    free(unicode_array);
    return measure;
}
void delo2d_sprite_font_draw(char *text,Vector2f position,Color color, SpriteFont *sprite_font, SpriteBatch *sprite_batch, float scale, int limit_x)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    uint32_t* unicode_array = delo2d_sprite_font_convert_to_unicode(text);
    int32_t texture_index = -1;
    delo2d_sprite_batch_add_texture(sprite_batch,&sprite_font->texture, &texture_index);

    Vector2f original_position = position;

    for (size_t i = 0; unicode_array[i] != 0; ++i)
    {
        uint32_t c = unicode_array[i];

        if(position.x > original_position.x + limit_x && limit_x != 0)
        {
            position.x = original_position.x;
            position.y += sprite_font->line_spacing*scale;
        }

        if(c == 32)
        {
            position.x += sprite_font->blank_space_offset_x*scale;
        }
        else if(c == 10)
        {
            position.x = original_position.x;
            position.y += sprite_font->line_spacing*scale;
        }
        else if (c>=32 && c < 256)
        {
            Glyph *glyph = &sprite_font->glyphs[(int32_t)c-33];
            
            int32_t index = sprite_batch->count;
            Sprite *sprite = &sprite_font->sprites[(int32_t)c-33];
            sprite_batch->rect_src[index].x      = sprite->rect_src.x;
            sprite_batch->rect_src[index].y      = sprite->rect_src.y;
            sprite_batch->rect_src[index].width  = sprite->rect_src.width;
            sprite_batch->rect_src[index].height = sprite->rect_src.height;

            sprite_batch->rect_des[index].x      = position.x;
            sprite_batch->rect_des[index].y      = position.y;
            sprite_batch->rect_des[index].width  = sprite->rect_src.width *scale;
            sprite_batch->rect_des[index].height = sprite->rect_src.height*scale;

            sprite->batch_index = i;
            sprite->quad_index = i;

            sprite_batch->texture_index[i]         = texture_index; 
            sprite_batch->color[index].r           = color.r;
            sprite_batch->color[index].g           = color.g;
            sprite_batch->color[index].b           = color.b;
            sprite_batch->color[index].a           = color.a;
            sprite_batch->flip_horizontally[index] = 0;
            sprite_batch->flip_vertically[index]   = 0;

            sprite_batch->scale[index].x = 1;
            sprite_batch->scale[index].y = 1;

            sprite_batch->skew[index].x = 0;
            sprite_batch->skew[index].y = 0;

            sprite_batch->pivot_point[index].x = 0;
            sprite_batch->pivot_point[index].y = 0;

            sprite_batch->position[index].x = position.x + ((glyph->w/2) + glyph->bearing_x)*scale;
            sprite_batch->position[index].y = position.y;

            sprite_batch->orientation[index] = 0;
            sprite_batch->updated[index] = 1;

            position.x += glyph->advance*scale;
            sprite_batch->count ++;
        } 
    }  
    free(unicode_array);
}
//text code end
