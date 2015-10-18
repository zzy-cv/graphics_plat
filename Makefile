all:
	gcc -o out/main linux_base/sdl.c -lSDL -I/usr/include/SDL/
run:
	./out/main
clean:
	rm -rf ./out/*