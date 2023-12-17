#VERT_BEGIN
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_mvp;

void main()
{      
    gl_Position = position * u_mvp;
    v_tex_coord = tex_coord;
}

#VERT_END
#FRAG_BEGIN
#version 330 core

uniform sampler2D u_texture;
layout(location = 0) out vec4 color;
in vec2 v_tex_coord;

void main()
{ 
    //vec4 tex_color = texture2D(u_textures[0],v_tex_coord); 
    color = tex_color*v_color;
}
#FRAG_END