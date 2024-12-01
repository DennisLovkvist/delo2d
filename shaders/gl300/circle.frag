#version 300 es
precision mediump float;

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec2 v_offset;
in float v_radius;

uniform float u_back_buffer_height;

void main()
{ 

    float dist = distance(gl_FragCoord.xy, vec2(v_offset.x,v_offset.y));

    if (dist < v_radius) 
    {
        color = v_color; 

    } 
    else 
    {
        discard;
    }
}