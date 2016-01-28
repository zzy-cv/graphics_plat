all: test_main.o sdl.o main_display.o basedraw.o dot_font.o hzk16
	gcc -o out/main out/test_main.o out/sdl.o out/main_display.o out/basedraw.o out/dot_font.o -lSDL -I/usr/include/SDL/ -lm

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

config:
	mkdir -p out

run:
	./out/main
clean:
	rm -rf ./out/*
