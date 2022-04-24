#VERT_BEGIN
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in float tex_index;

out vec2 v_tex_coord;
out float v_tex_index;

uniform mat4 u_mvp;

void main()
{ 
    
    gl_Position = position * u_mvp;
    v_tex_coord = tex_coord;
    v_tex_index = tex_index;

}

#VERT_END
#FRAG_BEGIN
#version 330 core

uniform vec4 u_color;
uniform sampler2D u_textures[2];

layout(location = 0) out vec4 color;
in vec2 v_tex_coord;
in float v_tex_index;

void main()
{ 
    int index = int(v_tex_index);
    vec4 tex_color = texture(u_textures[index],v_tex_coord);
    color = tex_color*u_color;
}
#FRAG_END