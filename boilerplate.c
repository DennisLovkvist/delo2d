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

#define GLEW_STATIC 1
#define COUNT_SHADERS 7
#define COUNT_TEXTURES 3
#define WINDOW_TITLE "delo2d boilerplate"

const unsigned int screen_width = 1920;
const unsigned int screen_height = 1080;

int main(void)
{ 
    GLFWwindow *window;
    Texture texture;
    unsigned int shader;
    float projection[4][4];
    SpriteBatch sprite_batch;
    Sprite sprite;

    if (!glfwInit())
    {
        return -1;
    }    
    delo2d_render_setup(&window, screen_width, screen_height,WINDOW_TITLE);  

    if(delo2d_render_initialize() == -1)
    {
        return -1;
    } 
    
    delo2d_matrix_orthographic_projection(projection,0.0f,(float)screen_width,0.0f,(float)screen_height,1,-1);    
    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    shader = delo2d_shader_from_file("shaders/delo2d_sprite_default.glsl"); 
    delo2d_texture_load(&texture,"textures/logo_animation.png");     


    delo2d_sprite_batch_create(&sprite_batch,1);

    Color color_white;
    delo2d_color_set_f(&color_white,1,1,1,1);
    delo2d_sprite_define(&sprite, 10,10,256,256,0,0,256,256,0,texture.width,texture.height,16,103,0.4,color_white,1,1,0,0,0,0);
    float t = 0;
    
    while (!glfwWindowShouldClose(window))
    { 
        t += 0.0016f;
         delo2d_sprite_animate(&sprite,0.0016);

        delo2d_render_target_set(0,0,0,0,1);
        delo2d_sprite_batch_begin(&sprite_batch,shader,projection);
            delo2d_sprite_batch_add(&sprite_batch,&sprite, &texture);
        delo2d_sprite_batch_end(&sprite_batch);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    glDeleteProgram(shader);
    glfwTerminate();

    return 0;
}