#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/delo2d.h"
#include "stb_image.h"




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
void rectangle_set(Rectangle *rectengle, int x, int y,int width, int height)
{
    rectengle->x = x;
    rectengle->y = y;
    rectengle->width = width;
    rectengle->height = height;
}
void sprite_batch_create(SpriteBatch *sprite_batch, unsigned int count)
{
    sprite_batch->count = count;
    delo2d_vertex_array_create(&sprite_batch->vertex_array,DELO_QUAD_LIST,count);
    sprite_batch->rectangle_source = malloc(sizeof(Rectangle)*count);
    sprite_batch->rectangle_destination = malloc(sizeof(Rectangle)*count);

    for(int i = 0;i < count;i++)
    {
        rectangle_set(sprite_batch->rectangle_source,0,0,0,0);
        rectangle_set(sprite_batch->rectangle_destination,0,0,0,0);
    }

}
void delo2d_render_sprite_batch(SpriteBatch *sprite_batch,Texture *texture, unsigned int shader)
{   
        delo2d_bind_texture(texture,0); 
        
        glUseProgram(shader); 
        glUniform4f(glGetUniformLocation(shader,"u_color"),0.2f,0.3f,0.8,1.0f);
        glUniform1i(glGetUniformLocation(shader,"u_texture"),0);    

        delo2d_vertex_array_bind(sprite_batch->vertex_array);

        GLClearError();
        delo2d_vertex_array_draw(sprite_batch->vertex_array);
        GLCheckError();

        delo2d_unbind_texture();
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
void delo2d_render(VertexArray *vertex_array,Texture *texture, unsigned int shader)
{   
        delo2d_bind_texture(texture,0); 
        
        glUseProgram(shader); 
        glUniform4f(glGetUniformLocation(shader,"u_color"),0.2f,0.3f,0.8,1.0f);
        glUniform1i(glGetUniformLocation(shader,"u_texture"),0);    

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
    stbi_set_flip_vertically_on_load(1);
    texture->local_buffer = stbi_load(file_path,&texture->width,&texture->height,&texture->bytes_per_pixel,4);
    glGenTextures(1,&texture->renderer_id);
    glBindTexture(GL_TEXTURE_2D,texture->renderer_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,texture->width,texture->height,1,GL_RGBA,GL_UNSIGNED_BYTE,texture->local_buffer);

    printf("%d",texture->bytes_per_pixel);


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


//vertex array code begin
void delo2d_vertex_array_draw(VertexArray *vertex_array)
{
    int draw_index_count = vertex_array->indices_per_element * vertex_array->count_elements;
    glDrawElements(GL_TRIANGLES,draw_index_count,GL_UNSIGNED_INT,NULL);
}
void delo2d_vertex_set_element(VertexArray *vertex_array, int position,float x, float y, float tex_x,float tex_y)
{
    int index = position * vertex_array->layout_float_count;
   vertex_array->buffer_position[index + 0] = x;
   vertex_array->buffer_position[index + 1] = y;



   vertex_array->buffer_position[index + 2] = tex_x;
   vertex_array->buffer_position[index + 3] = tex_y;
}
void delo2d_vertex_array_delete(VertexArray *vertex_array)
{
    delo2d_vertex_array_unbind(vertex_array);
    free(vertex_array->buffer_position);
    free(vertex_array->buffer_color);
    free(vertex_array->buffer_index);
}
void delo2d_vertex_array_create(VertexArray *vertex_array,unsigned int type, unsigned int element_count)
{

    vertex_array->layout_float_count = 4;
    vertex_array->type = type;
    vertex_array->count_elements = element_count;
    vertex_array->count_position = element_count * (type*vertex_array->layout_float_count);
    vertex_array->buffer_position = malloc(vertex_array->count_position * sizeof(float));
    vertex_array->indices_per_element = type == DELO_TRIANGLE_LIST ? 3:6;
    
    int length = vertex_array->count_position;
    for (size_t i = 0; i < length; i++)
    {
        vertex_array->buffer_position[i] = 0.0f;
    } 

    vertex_array->count_index = vertex_array->indices_per_element * element_count;
    vertex_array->buffer_index = malloc(vertex_array->count_index * sizeof(unsigned int));    

    
    if(vertex_array->type == DELO_TRIANGLE_LIST)
    {
        length = vertex_array->count_index;

        for (size_t i = 0; i < length; i++)
        {
            vertex_array->buffer_index[i] = i;
        } 
    }    
    if(vertex_array->type == DELO_QUAD_LIST)
    {        
        for (size_t i = 0; i < element_count; i++)
        {
            int index = i * 6;
            int pos_index = i*4;
            vertex_array->buffer_index[index + 0] = pos_index + 0;
            vertex_array->buffer_index[index + 1] = pos_index + 1;
            vertex_array->buffer_index[index + 2] = pos_index + 2;
            vertex_array->buffer_index[index + 3] = pos_index + 2;
            vertex_array->buffer_index[index + 4] = pos_index + 3;
            vertex_array->buffer_index[index + 5] = pos_index + 0;
        } 
    }

    glGenVertexArrays(1,&(vertex_array->vao));
    glGenBuffers(1,&(vertex_array->buffer));
    glBindBuffer(GL_ARRAY_BUFFER,vertex_array->buffer);
    glGenBuffers(1,&(vertex_array->ibo));
}
void delo2d_vertex_array_set_data(VertexArray *vertex_array)
{   
    delo2d_vertex_array_bind(vertex_array);
    
    glBufferData(GL_ARRAY_BUFFER,(vertex_array->indices_per_element * vertex_array->layout_float_count) * sizeof(float),vertex_array->buffer_position,GL_DYNAMIC_DRAW);    
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*4,0);


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, sizeof(GLfloat)*4,(GLvoid*)(2 * sizeof(GLfloat)));



    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertex_array->count_index * sizeof(unsigned int),vertex_array->buffer_index,GL_DYNAMIC_DRAW);
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
//shader code end