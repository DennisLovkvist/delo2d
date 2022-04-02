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
    GLFWwindow* window;
   
    if (!glfwInit())
    {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK)
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);


    VertexArray vertex_array;
    delo2d_vertex_array_create(&vertex_array,DELO_QUAD_LIST,1);

    delo2d_vertex_set_element(&vertex_array,0,-0.5f,-0.5f,0.0f,0.0f);
    delo2d_vertex_set_element(&vertex_array,1, 0.5f,-0.5f,1.0f,0.0f);
    delo2d_vertex_set_element(&vertex_array,2, 0.5f, 0.5f,1.0f,1.0f);    
    delo2d_vertex_set_element(&vertex_array,3,-0.5f, 0.5f,0.0f,1.0f);
    
    delo2d_vertex_array_set_data(&vertex_array);    

    unsigned int shader = delo2d_shader_from_file("default_shader.glsl");


    Texture texture;
    delo2d_load_texture(&texture,"test.png");

    
        
    
    glBindVertexArray(0);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
glEnable(GL_TEXTURE_2D);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glClearColor(1,0,0,1);
          
        
        delo2d_bind_texture(&texture,0); 
        
        glUseProgram(shader); 
        glUniform4f(glGetUniformLocation(shader,"u_color"),0.2f,0.3f,0.8,1.0f);
        glUniform1i(glGetUniformLocation(shader,"u_texture"),0);    

        delo2d_vertex_array_bind(&vertex_array);


        GLClearError();
        delo2d_vertex_array_draw(&vertex_array);
        GLCheckError();

        delo2d_unbind_texture();

        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}