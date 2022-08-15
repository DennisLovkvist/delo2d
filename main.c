/**
 * Author:    Dennis Lövkvist
 * Created:   2022-08-05
 * Version: 1.0
 **/

#include <GL/glew.h>//glew fetches function pointers from graphics card
#include <GLFW/glfw3.h>//glfw crosd-platform windows
#include <stdio.h>
#include <stdlib.h>
#include "delo2d.h"
#include <time.h>
#include "game.h"


#define GLEW_STATIC 1
#define COUNT_SHADERS 7
#define COUNT_TEXTURES 3
#define WINDOW_TITLE "delo2d example project"

const unsigned int screen_width = 1920;
const unsigned int screen_height = 1080;

int init(GLFWwindow **window,float *projection)
{         
    if(delo2d_render_setup(window, screen_width, screen_height,WINDOW_TITLE) == -1){return -1;} 
    
    delo2d_matrix_orthographic_projection(projection,0.0f,(float)screen_width,0.0f,(float)screen_height,1,-1);    
    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    
    return 0;
}
int load(Texture *textures,unsigned int *shaders)
{
    shaders[0] = delo2d_shader_from_file("shaders/delo2d_sprite_default.glsl");  
    shaders[1] = delo2d_shader_from_file("shaders/delo2d_render_target_default.glsl");    
    shaders[2] = delo2d_shader_from_file("shaders/warp.glsl"); 
    shaders[3] = delo2d_shader_from_file("shaders/voronoi.glsl"); 
    shaders[4] = delo2d_shader_from_file("shaders/distortion.glsl");  
    shaders[5] = delo2d_shader_from_file("shaders/sky.glsl");      
    shaders[6] = delo2d_shader_from_file("shaders/silhouette.glsl");      
    delo2d_texture_load(&textures[0],"textures/sprite_sheet_fire.png");  
    delo2d_texture_load(&textures[1],"textures/sprite_sheet_scene.png"); 
    delo2d_texture_load(&textures[2],"textures/sprite_sheet_fire_distortion.png"); 
    return 0;
}
void unload(Texture *textures,unsigned int *shaders)
{ 
    for (int i = 0; i < COUNT_TEXTURES; i++)
    {
        //not implemented
        //delo2d_unload_texture(&textures[i]);  
    }
    for (int i = 0; i < COUNT_SHADERS; i++)
    {
        glDeleteProgram(shaders[i]);
    }        
    glfwTerminate();
}
int main(void)
{  
    //variables
    GLFWwindow *window;
    Texture textures[COUNT_TEXTURES];//textures from files
    unsigned int shaders[COUNT_SHADERS];//shader ids from files
    Projection projection;//projection matrix

    KeyboardInput ki;
    KeyboardInput ki_prev;

    Scene scene;

    float dt = 0;
    float t = 0;
    
    if(init(&window,&projection) == -1)return -1;//init graphics
    load(&textures,&shaders);
    game_setup(&scene,&textures,screen_width,screen_height);    
    delo2d_input_init(&ki,&ki_prev);

    clock_t initial_time = clock();      

    glViewport(0, 0, screen_width,screen_height);    

    while (!glfwWindowShouldClose(window))
    {        
        //t = (float)clock()/CLOCKS_PER_SEC;
        t+= 0.016f;

        delo2d_input_update(window,&ki,&ki_prev);

        game_update_controls(&ki,&ki_prev,projection);   

        game_update_logic(t,dt,&scene);

        game_render(t,&scene,&shaders,&textures,projection,screen_width,screen_height);

        glfwSwapBuffers(window);

        //dt = (float)clock()/CLOCKS_PER_SEC - t;   
        dt = 0.016; 

        glfwPollEvents();
    }

    //unload and exit
    unload(&textures,&shaders);

    return 0;
}