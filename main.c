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
    delo2d_render_setup(&window, 800, 600,"OpenGl Test");  
    if(delo2d_render_initialize() == -1){return -1;}

    

    //load and initialize game
    VertexArray vertex_array;
    delo2d_vertex_array_create(&vertex_array,DELO_QUAD_LIST,1);
    delo2d_vertex_set_element(&vertex_array,0,-1.0f,-1.0f,0.0f,0.0f,0);
    delo2d_vertex_set_element(&vertex_array,1, 1.0f,-1.0f,1.0f,0.0f,0);
    delo2d_vertex_set_element(&vertex_array,2, 1.0f, 1.0f,1.0f,1.0f,0);    
    delo2d_vertex_set_element(&vertex_array,3,-1.0f, 1.0f,0.0f,1.0f,0);    
    delo2d_vertex_array_set_data(&vertex_array);    
    unsigned int shader = delo2d_shader_from_file("default_shader.glsl");
    Texture texture;
    delo2d_load_texture(&texture,"test.png");  
        
    
    


    //main loop begin
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glClearColor(1,0,0,1);

        delo2d_render(&vertex_array,&texture,shader);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    //unload and exit
    glDeleteProgram(shader);
    glfwTerminate();


    return 0;
}