digits.png: main
	./main

main: main.c
	cc -ggdb -I/usr/include/freetype2 -o main main.c -lfreetype
