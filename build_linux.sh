mkdir -p bin/debug/linux
gcc -o bin/debug/linux/main src/*.c -Ilibs -lfreetype  -Iinclude/ -Iinclude/freetype2 -lglfw -lGLEW -lGL -msse2 -lm -ldl -Wall -pedantic-errors
./bin/debug/linux/main