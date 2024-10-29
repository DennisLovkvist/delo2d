#version 330 core

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform sampler2D u_texture3;

uniform float u_back_buffer_height;

layout(location = 0) out vec4 color;

in vec2 v_tex_coord;
in vec4 v_color;
in vec2 v_limit_y;
flat in int v_tex_index;

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

    if (v_tex_index == 0)
    {
        sampled = texture(u_texture0, tex_coord);
    }
    else if (v_tex_index == 1)
    {
        sampled = texture(u_texture1, tex_coord);
    }
    else if (v_tex_index == 2)
    {
        sampled = texture(u_texture2, tex_coord);
    }
    else if (v_tex_index == 3)
    {
        sampled = texture(u_texture3, tex_coord);
    }
    else
    {
        sampled = vec4(1.0, 0.0, 0.0, 1.0); 
    }

    color = vec4(v_color.r, v_color.g, v_color.b, sampled.r*v_color.a);

    if (gl_FragCoord.y < u_back_buffer_height-v_limit_y.y && v_limit_y.y != 0)
    {
        color.a = 0;
    }
    else if (gl_FragCoord.y > u_back_buffer_height-v_limit_y.x && v_limit_y.x != 0)
    {
        color.a = 0;
    }
}
