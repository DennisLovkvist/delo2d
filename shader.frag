
/*
 * Author: Dennis Lövkvist
 * Version: 1.0 (2020-05-01)
 */

#version 330 core

//uniform sampler2D texture;
layout(location = 0) out vec4 color;

void main()
{ 
    /*
    vec2 coord = gl_TexCoord[0].xy;
    pixel = texture2D(texture, coord);  
	gl_FragColor = pixel;
    */
    
    color = vec4(1,0,0,1);

}