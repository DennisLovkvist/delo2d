//glfw crosd-platform windows
//glew fetches function pointers from graphics card
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/delo2d.h"
#include <time.h>

#define GLEW_STATIC 1
#define SHADERS_COUNT 1
#define TEXTURES_COUNT 2
#define SPRITES_COUNT 1
#define SPRITEBATCH_CAPACITY 1

#define WINDOW_TITLE "OpenGl Test"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540


int init(GLFWwindow **window,VertexArray *vertex_array,VertexArray *vertex_array2,RenderTarget *render_targets,unsigned int render_target_count, float *ortho_proj)
{    
    if (!glfwInit()){return -1;}    
    delo2d_render_setup(window, WINDOW_WIDTH, WINDOW_HEIGHT,WINDOW_TITLE);  
    if(delo2d_render_initialize(render_targets,render_target_count) == -1){return -1;} 
    delo2d_matrix_orthographic_projection(ortho_proj,0.0f,(float)WINDOW_WIDTH,0.0f,(float)WINDOW_HEIGHT,1,-1);
    
    
    delo2d_vertex_array_create(vertex_array,DELO_QUAD_LIST,1);
    delo2d_vertex_array_create(vertex_array2,DELO_QUAD_LIST,1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    return 0;
}
int load(Texture *textures,unsigned int *shaders)
{
    shaders[0] = delo2d_shader_from_file("default_shader.glsl");    
    delo2d_load_texture(&textures[0],"test.png");  
    delo2d_load_texture(&textures[1],"test2.png"); 
    return 0;
}
int game_setup(SpriteBatch *sprite_batch,SpriteBatch *sprite_batch2,VertexArray *vertex_array,VertexArray *vertex_array2,Sprite *sprites,Sprite *sprites2,Texture *textures)
{
    delo2d_define_sprite(&sprites[0], 0,0,512,256,0,0,512,256,0,textures[0].width,textures[0].height,8,25,0.1);      
    delo2d_define_sprite(&sprites2[0], 500,300,100,100,0,0,1000,1000,1,textures[1].width,textures[1].height,1,1,0);
    delo2d_create_sprite_batch(sprite_batch,1);
    delo2d_create_sprite_batch(sprite_batch2,1);

    delo2d_sprite_batch_add(sprite_batch,&sprites[0],0);
    delo2d_sprite_batch_add(sprite_batch2,&sprites2[0],0); 
    delo2d_sprite_batch_to_vertex_array(sprite_batch,vertex_array); 
    delo2d_sprite_batch_to_vertex_array(sprite_batch2,vertex_array2); 
    return 0;
}
void game_render(GLFWwindow **window,VertexArray *vertex_array, Texture *textures,unsigned int *shaders,float *ortho_proj)
{    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUniformMatrix4fv(glGetUniformLocation(shaders[0],"u_mvp"),1,GL_FALSE,&ortho_proj[0]);
    int samplers[2] = {0,1};
    glUniform1iv(glGetUniformLocation(shaders[0],"u_textures"),2,samplers);  

    


    delo2d_vertex_array_to_graphics_device(vertex_array,0);
    delo2d_render(vertex_array,textures,2,shaders[0]);

    glfwSwapBuffers(window);
}
void game_update_logic(float dt,VertexArray *vertex_array,Sprite *sprites)
{
    //delo2d_sprite_rotate(&sprites[0],0.01f,vertex_array);
    //delo2d_sprite_translate(&sprites[0],1,0,vertex_array);
    //delo2d_sprite_animate(&sprites[0],dt,vertex_array);
}
void unload(Texture *textures,unsigned int *shaders)
{ 
    for (int i = 0; i < TEXTURES_COUNT; i++)
    {
        //not implemented
        //delo2d_unload_texture(&textures[i]);  
    }
    for (int i = 0; i < SHADERS_COUNT; i++)
    {
        glDeleteProgram(shaders[i]);
    }        
    glfwTerminate();
}
void game_update_render_state(VertexArray *vertex_array, SpriteBatch *sprite_batch,Sprite *sprites)
{
    for (int i = 0; i < SPRITES_COUNT; i++)
    {
        if(sprites[i].tex_coords_updated)
        {
            delo2d_sprite_batch_update_tex_coords(vertex_array,sprite_batch,&sprites[i],i);
            sprites[i].tex_coords_updated = 0;
        }
    }
}
void game_update_controls(KeyboardInput *ki,KeyboardInput *ki_prev, float *ortho_proj)
{
    if (ki->move_up == GLFW_PRESS)
    {
        delo2d_camera_move(ortho_proj,0,-1.0f);
    }
    if (ki->move_dn == GLFW_PRESS)
    {
        delo2d_camera_move(ortho_proj,0,1.0f);
    }
    if (ki->move_l == GLFW_PRESS)
    {
        delo2d_camera_move(ortho_proj,-1,0);
    }
    if (ki->move_r == GLFW_PRESS)
    {
        delo2d_camera_move(ortho_proj,1,0);
    }
} 
int main(void)
{  
    //variables
    GLFWwindow *window;
    VertexArray vertex_array;
    VertexArray vertex_array2;
    SpriteBatch sprite_batch;
    SpriteBatch sprite_batch2;
    Sprite sprites[1];  
    Sprite sprites2[1];  
    Texture textures[TEXTURES_COUNT]; 
    KeyboardInput ki;
    KeyboardInput ki_prev;
    unsigned int shaders[SHADERS_COUNT];
    float ortho_proj[4][4];

    float dt = 0;
    float t = 0;
    RenderTarget render_targets[1];
    //init graphics
    init(&window,&vertex_array,&vertex_array2,&render_targets,1,&ortho_proj);
    //init input
    delo2d_input_init(&ki,&ki_prev);
    //load game resources
    load(&textures,&shaders);
    //setup game
    game_setup(&sprite_batch,&sprite_batch2,&vertex_array,&vertex_array2,&sprites,&sprites2,&textures);

    clock_t initial_time = clock();

    int n = 0;
    //main loop begin
    while (!glfwWindowShouldClose(window))
    {
        t = (float)clock()/CLOCKS_PER_SEC;
        game_update_controls(&ki,&ki_prev,&ortho_proj);       
        game_update_logic(dt,&vertex_array,&sprites);



    

    



        game_update_render_state(&vertex_array,&sprite_batch,&sprites);
        delo2d_vertex_array_set_attrib_pointer(&vertex_array);
        game_render(window,&vertex_array,&textures,&shaders,&ortho_proj);

        dt = (float)clock()/CLOCKS_PER_SEC - t;
        glfwPollEvents();
        delo2d_input_update(window,&ki,&ki_prev);
    }

    //unload and exit
    unload(&textures,&shaders);

    return 0;
}