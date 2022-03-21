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

/*
    unsigned int vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    float positions[8] = { -0.5f,-0.5f, 
                            0.5f,-0.5f, 
                            0.5f,0.5f,
                            -0.5f,0.5f};
    unsigned int buffer;
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,4*2* sizeof(float),positions,GL_DYNAMIC_DRAW);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, sizeof(float)*2,0);
    


    unsigned int indices[] = {0,1,2,2,3,0};
    unsigned int ibo;
    glGenBuffers(1,&ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(unsigned int),indices,GL_DYNAMIC_DRAW);
*/

    VertexArray vertex_array;
    delo2d_vertex_array_create(&vertex_array,DELO_QUAD_LIST,1);


    delo2d_vertex_set_element(&vertex_array,0,-0.5f,-0.5f,1,1,1,1);
    delo2d_vertex_set_element(&vertex_array,1, 0.5f,-0.5f,1,1,1,1);
    delo2d_vertex_set_element(&vertex_array,2, 0.5f, 0.5f,1,1,1,1);    
    delo2d_vertex_set_element(&vertex_array,3,-0.5f, 0.5f,1,1,1,1);
    
    delo2d_vertex_array_set_data(&vertex_array);    

    unsigned int shader = delo2d_shader_from_file("default_shader.glsl");
    
    int location = glGetUniformLocation(shader,"u_color");

    
    glBindVertexArray(0);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);



    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(shader);    
        glUniform4f(location,0.2f,0.3f,0.8,1.0f);

        //glBindVertexArray(vao);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
        //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,NULL);

        delo2d_vertex_array_bind(&vertex_array);
        delo2d_vertex_array_draw(&vertex_array);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}