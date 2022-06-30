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

     
    unsigned int shader = delo2d_shader_from_file("default_shader.glsl");
    Texture textures[2];
    delo2d_load_texture(&textures[0],"test.png");  
    delo2d_load_texture(&textures[1],"test2.png"); 

    //load and initialize game
    
    VertexArray vertex_array;
    delo2d_vertex_array_create(&vertex_array,DELO_QUAD_LIST,2);

    

    Sprite sprites[2];     
    delo2d_define_sprite(&sprites[0], 0,0,128,128,64,0,228,228,0,textures[0].width,textures[0].height);      
    delo2d_define_sprite(&sprites[1], 500,300,100,100,0,0,1000,1000,1,textures[1].width,textures[1].height);
    

    SpriteBatch sprite_batch;
    delo2d_create_sprite_batch(&sprite_batch,2);

    

    delo2d_sprite_batch_add(&sprite_batch,&sprites[0],0);
    delo2d_sprite_batch_add(&sprite_batch,&sprites[1],1);
   

    delo2d_sprite_batch_to_vertex_array(&sprite_batch,&vertex_array);

    
    float ortho_proj[4][4];
    delo2d_matrix_orthographic_projection(&ortho_proj,0.0f,960.0f,0.0f,540.0f,1,-1);



     
        
    Quad quad;
    
    delo2d_get_quad(&quad,&vertex_array,1);
    float rotation = 0;


    //main loop begin
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        delo2d_vertex_array_to_graphics_device(&vertex_array,0);
        rotation = 0.01f;
        delo2d_quad_rotate(&quad,rotation);
        delo2d_quad_translate(&quad,1,0);
        glClearColor(1,0,0,1);

        glUniform4f(glGetUniformLocation(shader,"u_color"),1.0f,1.0f,1.0,1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader,"u_mvp"),1,GL_FALSE,&ortho_proj[0]);
        int samplers[2] = {0,1};
        glUniform1iv(glGetUniformLocation(shader,"u_textures"),2,samplers);  

        delo2d_render(&vertex_array,&textures,2,shader);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    //unload and exit
    glDeleteProgram(shader);
    glfwTerminate();


    return 0;
}