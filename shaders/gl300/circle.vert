#version 300 es
precision mediump float;

layout (location = 0) in vec2 a_vertex;           
layout (location = 1) in vec4 a_color;            
layout (location = 2) in vec2 a_offset;        
layout (location = 3) in float a_radius;          

uniform mat4 u_mvp;
out vec4 v_color; 
out vec2 v_offset;
out float v_radius;

void main()
{

    vec4 position = vec4(a_vertex.xy, 0.0, 1.0);
    
    mat4 a_transform = mat4(
    a_radius,    0.0, 0.0, 0.0,
    0.0,  a_radius,   0.0, 0.0,
    0.0,  0.0, 1.0, 0.0,
    0.0,  0.0, 0.0, 1.0
);

    gl_Position = (a_transform * position + vec4(a_offset, 0.0, 0.0))*u_mvp;

    v_color = a_color;
    v_offset = a_offset;
    v_radius = a_radius;
}