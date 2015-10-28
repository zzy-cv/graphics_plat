#include <stdio.h>
#include <SDL.h>

#define WIDTH  1024
#define HEIGHT 768
#define BPP 4
#define DEPTH 32
SDL_Surface *screen;

void Draws_nacreen(SDL_Surface* screen, int h)
{ 
	int x, y, ytimesw;

	for(y = 0; y < screen->h; y++ ) 
	{
		ytimesw = y*screen->pitch/BPP;
		for( x = 0; x < screen->w; x++ ) 
		{
			putpixel(screen, x, ytimesw, 255, 255, 0);
		}
	}
	

}

int main(int argc, char* argv[])
{
	SDL_Event event;
	
	int keypress = 0;
	int h=0; 
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) 
		return 1;
	
	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
	{
		SDL_Quit();
		return 1;
	}
	
	if(SDL_MUSTLOCK(screen)) 
	{
		if(SDL_LockSurface(screen) < 0) return;
	}
	
	printf("width=%d, height=%d\n", screen->w, screen->h);
	while(!keypress) 
	{
		Draws_nacreen(screen,h++);
		if(SDL_MUSTLOCK(screen)) 
		SDL_UnlockSurface(screen);
		SDL_Flip(screen); 
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
			}
		}
	}
	
	SDL_Quit();

	return 0;
}
