#include <stdio.h>
#include <SDL.h>

#define WIDTH  1024
#define HEIGHT 768
#define BPP 4
#define DEPTH 32
SDL_Surface *screen;

extern unsigned int *get_display_buf();
extern void draw_display_buffer(void);

static int plat_init(void) {
	set_width(WIDTH);
	set_height(HEIGHT);
	set_display_buf();
}

/*
static void putpixel_screen(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 *pixel;
	Uint32 rgb;  

	rgb = SDL_MapRGB( screen->format, r, g, b );
	
	pixel = (Uint32*) screen->pixels  + y + x;
	*pixel = rgb;
}

void putpixel_for_rgb(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	datas_ptr[x+y*width] = MY_RGB_FOR_BMP(r, g, b); 
}

void putpixel(int x, int y, unsigned long color)
{
	datas_ptr[x+y*width] = MY_RGB_FOR_BMP((unsigned char)color, (unsigned char)(color>>8), (unsigned char)(color>>16)); 
}
*/
void update_screen(void)
{ 
	int x, y, ytimesw;
	unsigned int color = 0;
	unsigned int *buf = NULL;
	unsigned int *pixel;

	buf = get_display_buf();

	printf("screen->pitch/BPP=%d\n", screen->pitch/BPP);
	printf("w=%d, h=%d\n", screen->w, screen->h);
	for(y = 0; y < screen->h; y++ ) 
	{
		ytimesw = y*screen->pitch/BPP;
		// printf("w=%d, h=%d\n", screen->w, screen->h);
		for( x = 0; x < screen->w; x++ )
		{
			// printf("x=%d, y=%d, w=%d\n", x, y, screen->w);
			color = buf[x+y*WIDTH];
			// printf("color=%d\n", color);
			// putpixel_screen(screen, x, y, (unsigned char)color, (unsigned char)(color>>8), (unsigned char)(color>>16));
			pixel = (Uint32*) screen->pixels  + y*screen->pitch/BPP + x;
			*pixel = color;
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

	plat_init();
	
	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE)))
	{
		SDL_Quit();
		return 1;
	}
	
	if(SDL_MUSTLOCK(screen)) 
	{
		if(SDL_LockSurface(screen) < 0) {
			return;
		}
	}
	
	printf("width=%d, height=%d\n", screen->w, screen->h);
	draw_display_buffer();

	while(!keypress) 
	{
		printf("update screen ...\n");
		update_screen();
		printf("update screen end\n");
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
