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
#define SHADERS_COUNT 2
#define TEXTURES_COUNT 2
#define SPRITES_COUNT 2
#define SPRITEBATCH_CAPACITY 2

#define WINDOW_TITLE "OpenGl Test"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

typedef struct Graphics Graphics;
struct Graphics
{
    GLFWwindow *window;
    VertexArray vertex_array;
    VertexArray vertex_array2;
    SpriteBatch sprite_batch;
    SpriteBatch sprite_batch2;
    Sprite sprites[2];  
    Sprite sprites2[1];  
    Texture textures[TEXTURES_COUNT]; 
    unsigned int shaders[SHADERS_COUNT];
    float ortho_proj[4][4];
    RenderTarget render_target;
};

int init(Graphics *graphics)
{    
    if (!glfwInit()){return -1;}    
    delo2d_render_setup(&graphics->window, WINDOW_WIDTH, WINDOW_HEIGHT,WINDOW_TITLE);  
    if(delo2d_render_initialize() == -1){return -1;} 
    
    delo2d_matrix_orthographic_projection(graphics->ortho_proj,0.0f,(float)WINDOW_WIDTH,0.0f,(float)WINDOW_HEIGHT,1,-1);    
    
    delo2d_vertex_array_create(&graphics->vertex_array,DELO_QUAD_LIST,2);
    //delo2d_vertex_array_create(&graphics->vertex_array2,DELO_QUAD_LIST,1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    return 0;
}
int load(Texture *textures,unsigned int *shaders)
{
    shaders[0] = delo2d_shader_from_file("default_shader.glsl");  
    shaders[1] = delo2d_shader_from_file("shader_render_target.glsl");    
    delo2d_load_texture(&textures[0],"test.png");  
    delo2d_load_texture(&textures[1],"test2.png"); 
    return 0;
}
int game_setup(Graphics *graphics)
{
    delo2d_define_sprite(&graphics->sprites[0], 0,0,512,256,0,0,512,256,0,graphics->textures[0].width,graphics->textures[0].height,8,25,0.1);      
    delo2d_define_sprite(&graphics->sprites[1], 500,300,100,100,0,0,1000,1000,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0);
    delo2d_create_sprite_batch(&graphics->sprite_batch,2);
    //delo2d_create_sprite_batch(&graphics->sprite_batch2,1);

    delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[0],0);
    delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[1],1); 
    delo2d_sprite_batch_to_vertex_array(&graphics->sprite_batch,&graphics->vertex_array); 
    //delo2d_sprite_batch_to_vertex_array(&graphics->sprite_batch2,&graphics->vertex_array2); 

    return 0;
}
void game_render(Graphics *graphics)
{   
    delo2d_vertex_array_to_graphics_device(&graphics->vertex_array,0);
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT); 

    glUseProgram(graphics->shaders[0]); 
    glUniformMatrix4fv(glGetUniformLocation(graphics->shaders[0],"u_mvp"),1,GL_FALSE,&graphics->ortho_proj[0]);
    int samplers[2] = {0,1};
    glUniform1iv(glGetUniformLocation(graphics->shaders[0],"u_textures"),2,samplers);  

    glBindVertexArray(graphics->vertex_array.vao); 
    glBindBuffer(GL_ARRAY_BUFFER,graphics->vertex_array.vbo);
    delo2d_render(&graphics->vertex_array,graphics->textures,2,graphics->shaders[0]);
    

    glfwSwapBuffers(graphics->window);
}
void game_update_logic(float dt,VertexArray *vertex_array,Sprite *sprites)
{
    delo2d_sprite_rotate(&sprites[1],0.01f,vertex_array);
    //delo2d_sprite_translate(&sprites[0],1,0,vertex_array);
    delo2d_sprite_animate(&sprites[0],dt,vertex_array);
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
void game_update_render_state(Graphics *graphics)
{
    for (int i = 0; i < SPRITES_COUNT; i++)
    {
        if(graphics->sprites[i].tex_coords_updated)
        {
            delo2d_sprite_batch_update_tex_coords(&graphics->vertex_array,&graphics->sprite_batch,&graphics->sprites[i],i);
            graphics->sprites[i].tex_coords_updated = 0;
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
    Graphics graphics;
    KeyboardInput ki;
    KeyboardInput ki_prev;

    float dt = 0;
    float t = 0;
    //init graphics
    if(init(&graphics) == -1)return -1;
    //init input
    delo2d_input_init(&ki,&ki_prev);
    //load game resources
    load(&graphics.textures,&graphics.shaders);
    //setup game
    game_setup(&graphics);

    clock_t initial_time = clock();      
    
    int n = 0;

    glViewport(0, 0, 960, 540);


    

   delo2d_render_target_create(&graphics.render_target);


    while (!glfwWindowShouldClose(graphics.window))
    {
        t = (float)clock()/CLOCKS_PER_SEC;
        game_update_controls(&ki,&ki_prev,&graphics.ortho_proj);       
        game_update_logic(dt,&graphics.vertex_array,&graphics.sprites);



game_update_render_state(&graphics);

       game_render(&graphics);




        dt = (float)clock()/CLOCKS_PER_SEC - t;
        glfwPollEvents();
        delo2d_input_update(graphics.window,&ki,&ki_prev);
    }

    //unload and exit
    unload(&graphics.textures,&graphics.shaders);

    return 0;
}