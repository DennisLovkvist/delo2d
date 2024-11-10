mkdir -p bin/linux/debug
cp -u shaders bin/linux/debug -d -r
cp -u textures bin/linux/debug -d -r
cp -u fonts bin/linux/debug -d -r
c_files=("src/main.c")

gcc -o bin/linux/debug/main "${c_files[@]}" -Ilibs  -Iinclude/ -I/usr/include/freetype2 -lfreetype -lglfw -lGLEW -lGL -lm -ldl;
./bin/linux/debug/main