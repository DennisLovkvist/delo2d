#version 300 es
precision mediump float;

layout (location = 0) in vec2 a_vertex;           
layout (location = 1) in vec4 a_color;            
layout (location = 2) in vec2 a_center;          

uniform mat4 u_mvp;
out vec2 v_frag_coord;
out vec4 v_color; // Output color to the fragment shader
out vec2 v_center; // Output color to the fragment shader

void main()
{
    vec4 position = vec4(a_vertex.xy, 0.0, 1.0);
    
    gl_Position = position*u_mvp;

    v_color = a_color;
    vec3 ndc = gl_Position.xyz / gl_Position.w;
    v_frag_coord = ndc.xy * 0.5 + 0.5;
    v_center = a_center;
}