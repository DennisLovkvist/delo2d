#pragma once 
#define DELO2D_FUNCTION_SIGNATURES
#include <delo2d.h>
#include <stdint.h>
#include <math.h>

#define COLOR_TRANSPARENT (Color){0,0,0,0}

#define BRUSH_SQUARE 0
#define BRUSH_ROUND  1

static void pen_draw(RendererPrimitive* renderer,RendererCircle* renderer_circle, RenderTarget* layer, Vector2f point_a, Vector2f point_b, Color color, uint8_t thickness, uint32_t shader_brush)
{
    int x1 = (int)round(point_a.x);
    int y1 = (int)round(point_a.y); 
    int x2 = (int)round(point_b.x); 
    int y2 = (int)round(point_b.y);

    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);


    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2;
    int e2 = err;


    glBindFramebuffer(GL_FRAMEBUFFER, layer->fbo);

    glViewport(0, 0, layer->texture.width, layer->texture.height);

    //renderer_primitive_begin(renderer,&layer->projection,(GLuint*)&shader_brush,DELO_TRIANGLE_LIST);


    d2d_renderer_circle_begin(renderer_circle,&layer->projection,(GLuint*)&shader_brush);
        

    glUseProgram(shader_brush);
    //glUniform1f(glGetUniformLocation(shader_brush, "u_thickness"), thickness);

    //glUniform1f(glGetUniformLocation(shader_brush, "u_back_buffer_height"),(float)viewportHeight);
  
    while (1) 
    {
        //renderer_primitive_add_rectangle(renderer,(Rectangle_f){x1,y1,thickness,thickness},color);

        d2d_renderer_circle_add(renderer_circle,(Vector2f){x1,y1},color,(float)thickness);

        if (x1 == x2 && y1 == y2) break;
        e2 = err;

        if (e2 < dy) 
        { 
            err += dx; 
            y1 += sy; 
        }
        if (e2 > -dx) 
        { 
            err -= dy; 
            x1 += sx; 
        }
    }
    //renderer_primitive_end(renderer);
    d2d_renderer_circle_end(renderer_circle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
}