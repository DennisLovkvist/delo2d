
/*
 * Author: Dennis Lövkvist
 * Version: 1.0 (2020-05-01)
 */

#version 330 core

//uniform sampler2D texture;
layout(location = 0) in vec4 position;

void main()
{ 
    /*
    vec2 coord = gl_TexCoord[0].xy;
    pixel = texture2D(texture, coord);  
	gl_FragColor = pixel;
    */
    
    gl_Position = position;

}