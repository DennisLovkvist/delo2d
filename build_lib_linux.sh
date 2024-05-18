gcc -c src/delo2d.c -Ilibs -Iinclude/ -Iinclude/freetype2 -msse2 -Wall -pedantic-errors

ar rcs delo2d_lib.a *.o

