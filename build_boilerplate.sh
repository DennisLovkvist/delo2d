mkdir -p bin/debug/
cp -u shaders bin/debug/ -d -r
cp -u textures bin/debug/ -d -r
gcc -o bin/debug/delo2d_boilerplate stb_image.c delo2d.c boilerplate.c -Ilibs -lglfw -lGLEW -lGL -msse2 -lm -ldl -Wall -pedantic-errors
./bin/debug/delo2d_boilerplate