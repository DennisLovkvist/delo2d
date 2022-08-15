#VERT_BEGIN
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in float tex_index;
layout(location = 3) in vec4 color;

out vec2 v_tex_coord;
out float v_tex_index;
out vec4 v_color;

uniform mat4 u_mvp;

void main()
{     
    gl_Position = position * u_mvp;
    v_tex_coord = tex_coord;
    v_tex_index = tex_index;
    v_color = color;
}

#VERT_END
#FRAG_BEGIN
#version 330 core

uniform sampler2D u_textures[2];
uniform int u_textures_width[2];
uniform int u_textures_height[2];

layout(location = 0) out vec4 color;
in vec2 v_tex_coord;
in float v_tex_index;
in vec4 v_color;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
void main()
{ 
    int index = int(v_tex_index);
    vec2 coord = v_tex_coord / vec2(u_textures_width[0],u_textures_height[0]);

    //coord.x -= sin(coord.x-0.455)*0.2;
    coord.y += pow(abs(coord.x-0.455),2.5)*4;


    vec4 tex_color = texture(u_textures[index],coord);
    
    color = tex_color*v_color;
    color.a -= abs(coord.x-0.455)*3.0;


    vec2 tex_offset = 1.0 / textureSize(u_textures[index], 0); // gets size of single texel
        vec3 result = texture(u_textures[index], coord).rgb * weight[0]; // current fragment's contribution
    
         for(int i = 1; i < 5; ++i)
        {
            result += texture(u_textures[index], coord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_textures[index], coord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }

        color = vec4(result, 1.0)*v_color;


}
#FRAG_END