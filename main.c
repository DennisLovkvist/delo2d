//glfw crosd-platform windows
//glew fetches function pointers from graphics card
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/delo2d.h"

#define GLEW_STATIC 1


int main(void)
{  
    //opengl setup begin
    if (!glfwInit()){return -1;}
    GLFWwindow *window;
    delo2d_render_setup(&window, 960, 540,"OpenGl Test");  
    if(delo2d_render_initialize() == -1){return -1;}

    

    //load and initialize game
    
    VertexArray vertex_array;
    delo2d_vertex_array_create(&vertex_array,DELO_QUAD_LIST,2);


    Rectangle_f rect_src;
    Rectangle_f rect_des;

    //delo2d_set_quad(&vertex_array,0,0,rect_src);

    rect_des.x = 0;
    rect_des.y = 0;
    rect_des.width = 400;
    rect_des.height = 400;

    rect_src.x = 0;
    rect_src.y = 0;
    rect_src.width = 0.1f;
    rect_src.height = 0.1f;


    SpriteBatch sprite_batch;
    delo2d_create_sprite_batch(&sprite_batch,2);

    delo2d_set_rect_src(&sprite_batch,0,0,0,300,300,4096,2048);
    delo2d_set_rect_des(&sprite_batch,0,200,0,300,300);  
    delo2d_define_quad(&vertex_array,0,&sprite_batch.rect_des[0],&sprite_batch.rect_src[0],0);

    delo2d_set_rect_src(&sprite_batch,1,0,0,1136,1420,1136,1420);
    delo2d_set_rect_des(&sprite_batch,1,500,300,100,100);
    delo2d_define_quad(&vertex_array,1,&sprite_batch.rect_des[1],&sprite_batch.rect_src[1],1);
    



    Quad quad;

    delo2d_get_quad(&quad,&vertex_array,0);


    delo2d_quad_translate(&quad,10,10);

    delo2d_get_quad(&quad,&vertex_array,1);


    //delo2d_quad_translate(&quad,10,10);
    
    delo2d_quad_rotate(&quad,-0.9f);



    
    float ortho_proj[4][4];
    delo2d_matrix_orthographic_projection(&ortho_proj,0.0f,960.0f,0.0f,540.0f,1,-1);


   

/*
    SpriteBatch sprite_batch;
    Rectangle src;
    Rectangle des;

    delo2d_sprite_batch_create(&sprite_batch,2);
    delo2d_rectangle_set(&src,0,0,100,100);
    delo2d_rectangle_set(&des,0,0,100,100);
    delo2d_sprite_batch_modify_sprite(&sprite_batch,&src,&des);*/

    






    //delo2d_vertex_array_set_data(&vertex_array);    
    unsigned int shader = delo2d_shader_from_file("default_shader.glsl");
    Texture texture;
    Texture texture2;
    delo2d_load_texture(&texture,"test.png");  
    delo2d_load_texture(&texture2,"test2.png");  
        
    
    float rotation = 0;


    //main loop begin
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        delo2d_vertex_array_to_graphics_device(&vertex_array,0);
        rotation = 0.01f;
        delo2d_quad_rotate(&quad,rotation);
        glClearColor(1,0,0,1);

        glUniform4f(glGetUniformLocation(shader,"u_color"),1.0f,1.0f,1.0,1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader,"u_mvp"),1,GL_FALSE,&ortho_proj[0]);
        int samplers[2] = {0,1};
        glUniform1iv(glGetUniformLocation(shader,"u_textures"),2,samplers);  

        delo2d_render(&vertex_array,&texture,&texture2,shader);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    //unload and exit
    glDeleteProgram(shader);
    glfwTerminate();


    return 0;
}