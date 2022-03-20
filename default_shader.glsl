#VERT_BEGIN
#version 330 core

layout(location = 0) in vec4 position;

void main()
{ 
    
    gl_Position = position;

}
#VERT_END
#FRAG_BEGIN
#version 330 core

uniform vec4 u_color;

layout(location = 0) out vec4 color;

void main()
{ 
    color = u_color;
}
#FRAG_END