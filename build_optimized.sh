mkdir -p bin/release/
cp -u shaders bin/release/ -d -r
cp -u textures bin/release/ -d -r
gcc -o bin/release/delo2d_example stb_image.c delo2d.c main.c game.c  -Ilibs -lglfw -lGLEW -lGL -msse2 -lm -ldl -w -O3
./bin/release/delo2d_example