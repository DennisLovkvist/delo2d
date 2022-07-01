//glfw crosd-platform windows
//glew fetches function pointers from graphics card
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/delo2d.h"

#define GLEW_STATIC 1

int init(GLFWwindow **window,VertexArray *vertex_array, float *ortho_proj)
{    
    if (!glfwInit()){return -1;}    
    delo2d_render_setup(window, 960, 540,"OpenGl Test");  
    if(delo2d_render_initialize() == -1){return -1;}    
    delo2d_matrix_orthographic_projection(ortho_proj,0.0f,960.0f,0.0f,540.0f,1,-1);
    delo2d_vertex_array_create(vertex_array,DELO_QUAD_LIST,2);
    return 0;
}
int load(Texture *textures,unsigned int *shaders)
{
    shaders[0] = delo2d_shader_from_file("default_shader.glsl");    
    delo2d_load_texture(&textures[0],"test.png");  
    delo2d_load_texture(&textures[1],"test2.png"); 
    return 0;
}
int game_setup(SpriteBatch *sprite_batch,VertexArray *vertex_array,Sprite *sprites,Texture *textures)
{
    delo2d_define_sprite(&sprites[0], 0,0,128,128,64,0,228,228,0,textures[0].width,textures[0].height);      
    delo2d_define_sprite(&sprites[1], 500,300,100,100,0,0,1000,1000,1,textures[1].width,textures[1].height);
    delo2d_create_sprite_batch(sprite_batch,2);

    delo2d_sprite_batch_add(sprite_batch,&sprites[0],0);
    delo2d_sprite_batch_add(sprite_batch,&sprites[1],1); 
    delo2d_sprite_batch_to_vertex_array(sprite_batch,vertex_array); 
    return 0;
}
void game_render(GLFWwindow **window,VertexArray *vertex_array, Texture *textures,unsigned int *shaders,float *ortho_proj)
{
    glClear(GL_COLOR_BUFFER_BIT);
    delo2d_vertex_array_to_graphics_device(vertex_array,0);
    glClearColor(1,0,0,1);
    glUniform4f(glGetUniformLocation(shaders[0],"u_color"),1.0f,1.0f,1.0,1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaders[0],"u_mvp"),1,GL_FALSE,&ortho_proj[0]);
    int samplers[2] = {0,1};
    glUniform1iv(glGetUniformLocation(shaders[0],"u_textures"),2,samplers);  
    delo2d_render(vertex_array,textures,2,shaders[0]);
    glfwSwapBuffers(window);
}
void game_update(VertexArray *vertex_array,Sprite *sprites)
{
    
    delo2d_sprite_rotate(&sprites[1],0.01f,vertex_array);
    delo2d_sprite_translate(&sprites[1],1,0,vertex_array);
}
int main(void)
{  
    //variables
    GLFWwindow *window;
    VertexArray vertex_array;
    SpriteBatch sprite_batch;
    Sprite sprites[2];  
    Texture textures[2]; 
    unsigned int shaders[1];
    float ortho_proj[4][4];
    
    //init graphics
    init(&window,&vertex_array,&ortho_proj);

    //load game resources
    load(&textures,&shaders);
    //setup game
    game_setup(&sprite_batch,&vertex_array,&sprites,&textures);

    //main loop begin
    while (!glfwWindowShouldClose(window))
    {
        //game logic
        game_update(&vertex_array,&sprites);
        //rendering
        game_render(window,&vertex_array,&textures,&shaders,&ortho_proj);
        
        glfwPollEvents();
    }

    //unload and exit
    glDeleteProgram(shaders[0]);
    glfwTerminate();


    return 0;
}