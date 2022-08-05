mkdir -p bin/debug/
cp -u shaders bin/debug/ -d -r
cp -u textures bin/debug/ -d -r
gcc -o bin/debug/delo2d_example stb_image.c delo2d.c main.c game.c -Ilibs -lglfw -lGLEW -lGL -msse2 -lm -ldl -w
./bin/debug/delo2d_example