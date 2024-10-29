#version 300 es
precision mediump float;

layout (location = 0) in vec3  a_vertex;         
layout (location = 1) in vec2  a_tex_coord;      
layout (location = 2) in vec4  a_color;        
layout (location = 3) in vec2  a_offset;                
layout (location = 4) in vec4  a_src_rect;  
layout (location = 5) in mat4  a_transform;  
layout (location = 9) in int a_tex_index;  
layout (location = 10) in vec2 a_limit_y;      

out vec2 v_tex_coord;
out vec4 v_color;
out vec2 v_limit_y;
flat out int v_tex_index;
uniform mat4 u_mvp;

void main()
{
    vec4 position = vec4(a_vertex.xy, 0.0, 1.0);
    
    gl_Position = (a_transform * position + vec4(a_offset, 0.0, 0.0))*u_mvp;

    v_tex_coord = a_tex_coord * a_src_rect.zw + a_src_rect.xy;
    v_tex_index = a_tex_index;
    v_color     = a_color;
    v_limit_y = a_limit_y;
}
