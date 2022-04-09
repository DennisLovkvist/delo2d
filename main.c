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
    delo2d_vertex_array_create(&vertex_array,DELO_QUAD_LIST,1);






    delo2d_vertex_set_element(&vertex_array,0,100.0f,100.0f,0.0f,0.0f,0);
    delo2d_vertex_set_element(&vertex_array,1,200.0f,100.0f,1.0f,0.0f,0);
    delo2d_vertex_set_element(&vertex_array,2,200.0f,200.0f,1.0f,1.0f,0);
    delo2d_vertex_set_element(&vertex_array,3,100.0f,200.0f,0.0f,1.0f,0);

    
    float ortho_proj[4][4];
    delo2d_matrix_ortho_projection(&ortho_proj,0.0f,960.0f,0.0f,540.0f,1,-1);


   

/*
    SpriteBatch sprite_batch;
    Rectangle src;
    Rectangle des;

    delo2d_sprite_batch_create(&sprite_batch,2);
    delo2d_rectangle_set(&src,0,0,100,100);
    delo2d_rectangle_set(&des,0,0,100,100);
    delo2d_sprite_batch_modify_sprite(&sprite_batch,&src,&des);*/

    






    delo2d_vertex_array_set_data(&vertex_array);    
    unsigned int shader = delo2d_shader_from_file("default_shader.glsl");
    Texture texture;
    Texture texture2;
    delo2d_load_texture(&texture,"test.png");  
    delo2d_load_texture(&texture2,"test2.png");  
        
    
    


    //main loop begin
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glClearColor(1,0,0,1);

        glUniform4f(glGetUniformLocation(shader,"u_color"),0.2f,0.3f,0.8,1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader,"u_mvp"),1,GL_FALSE,&ortho_proj[0]);
        glUniform1i(glGetUniformLocation(shader,"u_texture"),0);   


        

        delo2d_render(&vertex_array,&texture,shader);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    //unload and exit
    glDeleteProgram(shader);
    glfwTerminate();


    return 0;
}