#include <stdio.h>
#include <SDL.h>

#define WIDTH 1024
#define HEIGHT 768
#define BPP 4
#define DEPTH 32

#define MY_RGB(r,g,b)    ((unsigned int)(((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|\
                 (((unsigned int)(unsigned char)(r))<<16)))

static unsigned long buf[WIDTH*HEIGHT];

void putpixel(int x, int y, unsigned int color)
{
	buf[y*WIDTH+x] = color;
}

void putpixel_by_rgb(int x, int y, 
		unsigned char r, unsigned char g, unsigned char b)
{
	putpixel(x, y, MY_RGB(r, g, b));
}

void setpixel_rgb(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 color;  
 
    color = SDL_MapRGB( screen->format, r, g, b );
  
    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = color;
}

void setpixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
    Uint32 *pixmem32;
  
    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = color;
}

void draw_menu(SDL_Surface* screen)
{ 
	int x, y, ytimesw;
	static unsigned char c = 0;

	if(SDL_MUSTLOCK(screen)) 
	{
		if(SDL_LockSurface(screen) < 0) 
		{
			return;
		}
	}

	for(y = 0; y < 25; y++ ) 
	{
		ytimesw = y*screen->pitch/BPP;
		for( x = 0; x < screen->w; x++ ) 
		{
			setpixel(screen, x, ytimesw, 0xff);
		}
	}

	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	SDL_Flip(screen); 
}


void draw_screen(SDL_Surface* screen)
{ 
	int x, y, ytimesw;
	Uint32 color = 0;

	if(SDL_MUSTLOCK(screen)) 
	{
		if(SDL_LockSurface(screen) < 0) 
		{
			return;
		}
	}

	for(y = 25; y < screen->h; y++ ) 
	{
		ytimesw = y*screen->pitch/BPP;
		for( x = 0; x < screen->w; x++ ) 
		{
			color = buf[(y-25)*WIDTH+x];
			setpixel(screen, x, ytimesw, color);
		}
	}

	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	SDL_Flip(screen); 
}

void sdl_mouse_button_down(SDL_Surface *screen, SDL_Event event)
{
	printf("mouse button down: y=%d\n", event.button.y);
	switch(event.button.button)
	{
		case SDL_BUTTON_LEFT:
		if(event.button.y<20)
		{
			draw_screen(screen);
		}
		break;
	}
}

void draw_test()
{
	int i, j;
	for(j=0; j<50; j++)
	{
		for(i=0; i<50; i++)
		{
			putpixel(i, j, 0xff);
		}
	}
}

extern main_test();
int main(int argc, char* argv[])
{
	SDL_Surface *screen;
	SDL_Event event;
	int keypress = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) 
	{
		return 1;
	}
	
	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
	{
		SDL_Quit();
		return 1;
	}

	draw_menu(screen);
	main_test();

	while(!keypress) 
	{
		while(SDL_PollEvent(&event)) 
		{      
			switch (event.type) 
			{
			case SDL_QUIT:
				keypress = 1;
				break;
			case SDL_KEYDOWN:
				keypress = 1;
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEBUTTONDOWN:
				sdl_mouse_button_down(screen, event);
				break;
			}
		}
	}

	SDL_Quit();

	return 0;
}

