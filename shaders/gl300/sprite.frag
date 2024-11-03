#version 300 es
precision mediump float;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform sampler2D u_texture3;

layout(location = 0) out vec4 color;

in vec2 v_tex_coord;
in vec4 v_color;
in vec2 v_limit_y;
in float v_tex_index;

uniform bool u_flip;

void main()
{ 
    vec2 tex_coord;

    if(u_flip)
    {
        tex_coord = vec2(v_tex_coord.x, 1.0 - v_tex_coord.y);
    }
    else
    {
        tex_coord = v_tex_coord;
    }

    vec4 sampled;


    if (v_tex_index == 0.0)
    {
        sampled = texture(u_texture0, tex_coord);
    }
    else if (v_tex_index == 1.0)
    {
        sampled = texture(u_texture1, tex_coord);
    }
    else if (v_tex_index == 2.0)
    {
        sampled = texture(u_texture2, tex_coord);
    }
    else if (v_tex_index == 3.0)
    {
        sampled = texture(u_texture3, tex_coord);
    }
    else
    {
        sampled = vec4(1.0, 1.0, 0.0, 1.0); 
    }

    color = sampled*v_color;
}