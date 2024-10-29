#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_frag_coord;
in vec4 v_color;

void main()
{ 
    color = v_color;
}