/**
 * Author:    Dennis Lövkvist
 * Created:   2022-08-05
 * Version: 1.0
 **/

#include <GL/glew.h>//glew fetches function pointers from graphics card
#include <GLFW/glfw3.h>//glfw cross-platform windows
#include <stdio.h>
#include <stdlib.h>
#include "delo2d.h"
#include <sys/time.h>

#define GLEW_STATIC 1
#define COUNT_SHADERS 7
#define COUNT_TEXTURES 3
#define WINDOW_TITLE "delo2d boilerplate"

const unsigned int screen_width = 800;
const unsigned int screen_height = 600;

int main(void)
{ 
    GLFWwindow *window;
    Texture texture;
    unsigned int shader;
    unsigned int shader_primitive;
    Matrix44 projection;
    SpriteBatch sprite_batch;
    PrimitiveBatch primitive_batch;
    Sprite sprite;

    if(delo2d_render_setup(&window, screen_width, screen_height,WINDOW_TITLE) == -1){return -1;}//setup and initialization for opengl
    
    projection = matrix_orthographic_projection(0.0f,(float)screen_width,0.0f,(float)screen_height,1,-1);//creates an orthographic_projection to be used as our camera

    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    glfwSwapInterval(0);

    shader_primitive = delo2d_shader_from_file("shaders/delo2d_primitive_default.glsl");
    shader = delo2d_shader_from_file("shaders/delo2d_sprite_default.glsl");//loads and parses the default sprite shader
    //delo2d_texture_load(&texture,"textures/logo_animation.png");//loads texture (only tested .png files)
    Glyph glyphs[128];
    delo2d_font_load(&texture,&glyphs[0],128,"fonts/white-rabbit.regular.ttf");

    delo2d_sprite_batch_create(&sprite_batch,1);//creates a spritebatch with capacity for 1 sprite    
    delo2d_primitive_batch_create(&primitive_batch,1000);//creates a spritebatch with capacity for 1 sprite
    //defines the sprites
    Color color_white;
    delo2d_color_set_f(&color_white,1,1,1,1);
    int x = 0;
    int y = 0;
    delo2d_sprite_define(&sprite, x,y,texture.width,texture.height,0,0,texture.width,texture.height,0,texture.width,texture.height,1,1,1,color_white,1,1,0,0,0,0);
  

    while (!glfwWindowShouldClose(window))
    { 

        delo2d_render_target_set(0,0,0,0,1);//sets framebuffer to 0 (the screen) and clear the buffer with r=0,g=0,b=0,a=1
        
        delo2d_sprite_batch_begin(&sprite_batch,shader,projection);
            delo2d_sprite_batch_add(&sprite_batch,&sprite, &texture);
        delo2d_sprite_batch_end(&sprite_batch);



        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    delo2d_texture_delete(&texture);
    delo2d_sprite_batch_delete(&sprite_batch);
    delo2d_primitive_batch_delete(&primitive_batch);

    glDeleteProgram(shader_primitive);
    glDeleteProgram(shader);
    glfwTerminate();

    return 0;
}