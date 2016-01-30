all: test_main.o sdl.o main_display.o basedraw.o dot_font.o hzk16 bmp.o trace.o
	gcc -o out/main out/test_main.o out/sdl.o out/main_display.o out/basedraw.o out/dot_font.o out/bmp.o out/trace.o -lSDL -I/usr/include/SDL/ -lm

test_main.o: test/test_main.c
	gcc -c -o out/test_main.o test/test_main.c -I./libs/lib_graphics/lib_base_draw -I./linux_base -I./libs/lib_font/lib_dot_font
sdl.o: linux_base/sdl.c
	gcc -c -o out/sdl.o linux_base/sdl.c -I/usr/include/SDL/
main_display.o: linux_base/main_display.c
	gcc -c -o out/main_display.o linux_base/main_display.c -I./libs/lib_graphics/lib_base_draw
basedraw.o: libs/lib_graphics/lib_base_draw/basedraw.c
	gcc -c -o out/basedraw.o libs/lib_graphics/lib_base_draw/basedraw.c -I./libs/lib_graphics/lib_base_draw -I./linux_base -lm
dot_font.o: libs/lib_font/lib_dot_font/dot_font.c
	gcc -c -o out/dot_font.o libs/lib_font/lib_dot_font/dot_font.c -I./libs/lib_graphics/lib_base_draw -I./linux_base
hzk16: res/res_font/HZK16
	cp res/res_font/HZK16 out/
trace.o: debug_code/trace.c
	gcc -c -o out/trace.o debug_code/trace.c
bmp.o: libs/lib_graphics/lib_bmp/bmp.c libs/lib_graphics/lib_bmp/bmp.h
	gcc -c -o out/bmp.o libs/lib_graphics/lib_bmp/bmp.c -I./libs/lib_graphics/lib_bmp -I./linux_base -I./libs/lib_graphics/lib_base_draw -I./debug_code

config:
	mkdir -p out

run:
	./out/main
clean:
	rm -rf ./out/*
