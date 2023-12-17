mkdir -p bin/debug/
cp -u shaders bin/debug/ -d -r
cp -u textures bin/debug/ -d -r
cp -u fonts bin/debug/ -d -r
gcc -o bin/debug/main src/*.c -Ilibs -lfreetype  -I/usr/include/freetype2 -lglfw -lGLEW -lGL -msse2 -lm -ldl -Wall -pedantic-errors
./bin/debug/main