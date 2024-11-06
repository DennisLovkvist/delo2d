#version 300 es
precision mediump float;

layout(location = 0) out vec4 color;

in vec2 v_frag_coord;
in vec4 v_color;
in vec2 v_center;

uniform float u_thickness;   

void main()
{ 
    vec2 fragCoord = gl_FragCoord.xy;

    float dist = distance(fragCoord, v_center);

    if (dist < u_thickness*0.5) 
    {
        color = v_color; 

    } 
    else 
    {
        discard;
    }
}