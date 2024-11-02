#version 300 es
precision mediump float;

layout(location = 0) out vec4 color;

in vec2 v_frag_coord;
in vec4 v_color;

void main()
{ 
    int width = 512; // Width of the checkerboard
    int height = 512; // Height of the checkerboard

    int squareSize = 32; // Size of each square in pixels (adjust as needed)

    int xIndex = int(gl_FragCoord.x) / squareSize; 
    int yIndex = int(gl_FragCoord.y) / squareSize;

    float checker = float((xIndex + yIndex) % 2); 

    if(checker == 0.0)checker+= 0.3;
    if(checker == 1.0)checker-= 0.3;

    color = vec4(checker, checker, checker, 1.0);
}