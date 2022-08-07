# delo2d

## Description
### A personal lightweight library for 2D rendering with opengl.
I needed something to replace Monogame/XNA after moving from C# to C.<br>
All my 2D projects only require a narrow list of features, therefore I had an overarching goal of making the library small and simple.
Still very much in development

## Main goals
* Loading .png files (Textures) [Done]
* Manipulate how the image data should be drawn (Sprites) [Done]
* Draw several sprites in one draw call (SpriteBatch) [Done]
* Build an exampel project [Done]

## TODO
* PrimitiveBatch
* Skew sprites
* Documentation

Language: c & glsl

## Build boilerplate project
```
$ git clone https://github.com/DennisLovkvist/delo2d.git
$ cd delo2d
$ ./build_boilerplate.sh
```
![Alt text](screenshot_project_boilerplate.png?raw=true "screenshot_project_boilerplate")

## Build example project
```
$ git clone https://github.com/DennisLovkvist/delo2d.git
$ cd delo2d
$ ./build.sh
```
![Alt text](screenshot_project_example.png?raw=true "screenshot_project_example")
