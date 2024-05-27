
/*System Headers*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
/*Third-Party Headers*/
#include <GL/glew.h>//glew fetches function pointers from graphics card
#include <GLFW/glfw3.h>//glfw cross-platform windows
/*Custom Headers*/
#include "delo2d.h"

#define COUNT_SHADERS 7
#define COUNT_TEXTURES 3
#define WINDOW_TITLE "delo2d boilerplate"

const unsigned int screen_width = 800;
const unsigned int screen_height = 600;

int main(void)
{ 
    GLFWwindow *window;
    Texture texture;
    unsigned int shader_sprite_default;
    unsigned int shader_sprite_anti_aliasing;
    unsigned int shader_primitive;
    unsigned int shader_render_target;
    Matrix44 projection;
    SpriteBatch sprite_batch;
    PrimitiveBatch primitive_batch;
    Sprite sprite;
    SpriteFont sprite_font;
    RenderTarget render_target;

    if(delo2d_render_setup((void**)&window, screen_width, screen_height,WINDOW_TITLE) == -1){return -1;}//setup and initialization for opengl
    
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(debugCallback, 0);

    delo2d_render_target_create(&render_target,screen_width,screen_height, 256,86,256,64);

    projection = matrix44_orthographic_projection(0.0f,(float)screen_width,0.0f,(float)screen_height,1,-1);//creates an orthographic_projection to be used as our camera

    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    glfwSwapInterval(0);


    if(delo2d_shader_from_file("shaders/delo2d_primitive_default.glsl",&shader_primitive) == DELO_ERROR)
    {
        glfwTerminate();
        return 1;
    }
    if(delo2d_shader_from_file("shaders/delo2d_sprite_default.glsl",&shader_sprite_default) == DELO_ERROR)
    {
        glfwTerminate();
        return 1;
    }
    if(delo2d_shader_from_file("shaders/delo2d_sprite_aa.glsl",&shader_sprite_anti_aliasing) == DELO_ERROR)
    {
        glfwTerminate();
        return 1;
    }

    if(delo2d_shader_from_file("shaders/delo2d_render_target_default.glsl",&shader_render_target) == DELO_ERROR)
    {
        glfwTerminate();
        return 1;
    }

    if(delo2d_texture_load(&texture,"textures/logo_animation.png") == DELO_ERROR)
    {
        glfwTerminate();
        return 1;
    }


    delo2d_sprite_font_load(&sprite_font,"fonts/UbuntuMono-R.ttf",32);//loads font (only .ttf)

    delo2d_sprite_batch_create(&sprite_batch,64);//creates a spritebatch with capacity for 64 sprites   
    delo2d_primitive_batch_create(&primitive_batch,1000);//creates a spritebatch with capacity for 1 sprite
    
    //defines the sprites
    Color color_white;
    delo2d_color_set_f(&color_white,1,1,1,1);
    int x = screen_width*0.5 - 256/2;
    int y = screen_height*0.5 - 256/2;
    delo2d_sprite_define(&sprite, x,y,256,256,0,0,256,256,0,texture.width,texture.height,16,103,2,color_white,1,1,0,0,0,0);
    sprite.loop = 0;

    float dt = 0;
    struct timeval t1, t2;
    double elapsedTime;

    Vector2f render_target_text_position = (Vector2f){8,32};

    while (!glfwWindowShouldClose(window))
    { 
        gettimeofday(&t1, NULL);

        delo2d_render_target_set(render_target.fbo,0.3,0.3,0.3,1);

        delo2d_sprite_batch_begin(&sprite_batch,shader_sprite_anti_aliasing,render_target.projection);
            delo2d_sprite_font_draw("Render Target",render_target_text_position,(Color){0.8,0.8,0.8,1},&sprite_font, &sprite_batch,1,0);
        delo2d_sprite_batch_end(&sprite_batch);

        render_target_text_position.x += 100*dt;
        render_target_text_position.x = (render_target_text_position.x > 256) ? -256:render_target_text_position.x;

        delo2d_render_target_set(0,0,0,0,1);//sets framebuffer to 0 (the screen) and clear the buffer with r=0,g=0,b=0,a=1
        
        delo2d_sprite_animate(&sprite,dt,0);

        delo2d_sprite_batch_begin(&sprite_batch,shader_sprite_default,projection);//sets up the spritebatch for drawing with a shader and projection
            delo2d_sprite_font_draw("delo2d boilerplate 2024-05-27",(Vector2f){150,50},(Color){0.4,0.4,0.4,1},&sprite_font, &sprite_batch,1,0);
        delo2d_sprite_batch_end(&sprite_batch);//sets the content of the spritebatch to a vertex array and draws it
        
        delo2d_sprite_batch_begin(&sprite_batch,shader_sprite_default,projection);//sets up the spritebatch for drawing with a shader and projection
           delo2d_sprite_batch_add(&sprite_batch,&sprite, &texture);//adds a sprite to the spritebatch and the texture used by the sprite
        delo2d_sprite_batch_end(&sprite_batch);//sets the content of the spritebatch to a vertex array and draws it


        //using primitivebatch to draw triangles
        delo2d_primitive_batch_begin(&primitive_batch,shader_primitive,projection,DELO_TRIANGLE_LIST);
            //triangle with a different color for each vertex
            delo2d_primitive_batch_add(&primitive_batch,100,300,1,0,0,1);
            delo2d_primitive_batch_add(&primitive_batch,200,300,0,1,0,1);
            delo2d_primitive_batch_add(&primitive_batch,150,400,0,0,1,1);
            //white triangle
            delo2d_primitive_batch_add(&primitive_batch,600,300,1,1,1,1);
            delo2d_primitive_batch_add(&primitive_batch,700,300,1,1,1,1);
            delo2d_primitive_batch_add(&primitive_batch,650,400,1,1,1,1);
        delo2d_primitive_batch_end(&primitive_batch);

        //using primitivebatch to draw lines
        delo2d_primitive_batch_begin(&primitive_batch,shader_primitive,projection,DELO_LINE_LIST);
            //draws 3 lines making up the outline of a traingle
            delo2d_primitive_batch_add(&primitive_batch,100,300,1,0,0,1);//line 1
            delo2d_primitive_batch_add(&primitive_batch,200,300,0,1,0,1);//line 1
            delo2d_primitive_batch_add(&primitive_batch,200,300,0,1,0,1);//line 2
            delo2d_primitive_batch_add(&primitive_batch,150,200,0,1,0,1);//line 2
            delo2d_primitive_batch_add(&primitive_batch,150,200,0,1,0,1);//line 3
            delo2d_primitive_batch_add(&primitive_batch,100,300,1,0,0,1);//line 3            
            //draws 3 lines making up the outline of a traingle
            delo2d_primitive_batch_add(&primitive_batch,600,300,1,1,1,1);//line 1
            delo2d_primitive_batch_add(&primitive_batch,700,300,1,1,1,1);//line 1
            delo2d_primitive_batch_add(&primitive_batch,700,300,1,1,1,1);//line 2
            delo2d_primitive_batch_add(&primitive_batch,650,200,1,1,1,1);//line 2
            delo2d_primitive_batch_add(&primitive_batch,650,200,1,1,1,1);//line 3
            delo2d_primitive_batch_add(&primitive_batch,600,300,1,1,1,1);//line 3
        delo2d_primitive_batch_end(&primitive_batch);

        delo2d_render_target_draw(&render_target, shader_render_target);


        glfwSwapBuffers(window);

        gettimeofday(&t2, NULL);
        
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;// sec to ms
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;// us to ms        

        dt =  elapsedTime *= 0.001; 

        glfwPollEvents();
    }

    delo2d_texture_delete(&texture);
    delo2d_sprite_batch_delete(&sprite_batch);
    delo2d_primitive_batch_delete(&primitive_batch);

    glDeleteProgram(shader_primitive);
    glDeleteProgram(shader_sprite_default);
    glfwTerminate();

    return 0;
}