#if 0
#include <windows.h>

#include "main.h"

#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"user32.lib")

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;


#define WIDTH  1200
#define HEIGHT 1200

#define WIN_START_X  100
#define WIN_START_Y  100

#define START_X  0
#define START_Y  0

static COLORREF  datas[WIDTH*HEIGHT]    =    {0} ;

extern void fill_datas(void);
extern void test_init(void);
extern void test_end(void);

COLORREF *get_datas_ptr(void)
{
	return datas;
}

int get_width()
{
	return WIDTH;
}

int get_height()
{
	return HEIGHT;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
        
                   PSTR szCmdLine, int iCmdShow)
        
{
	static TCHAR szAppName [] = TEXT ("Bricks2") ;
	HWND                          hwnd ;
	MSG                        msg ;
	WNDCLASS                      wndclass ;
	int i=0;

	wndclass.style                               = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc                         = WndProc ;
	wndclass.cbClsExtra                          = 0 ;
	wndclass.cbWndExtra                          = 0 ;
	wndclass.hInstance                           = hInstance ;
	wndclass.hIcon                               = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor                             = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground              = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName                = NULL ;
	wndclass.lpszClassName               = szAppName ;
  
	if (!RegisterClass (&wndclass))
	{
		MessageBox (  NULL, TEXT ("This program requires Windows NT!"),
		szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("graphic test environment"),
		WS_OVERLAPPEDWINDOW,
		WIN_START_X,  WIN_START_Y,                       //CW_USEDEFAULT, CW_USEDEFAULT,
		WIDTH, HEIGHT,       //CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;
    if(!(i=SetTimer(hwnd, 0, 1000, NULL)))
	{
		DebugBreak();
		
	}
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
	}

int g_frame_id = 0;
int g_frame_num = 30;

void set_frame_num(int num)
{
	g_frame_num = num;
}

LRESULT CALLBACK WndProc ( HWND hwnd, UINT message, WPARAM wParam,LPARAM lParam)
        
{
	static int startx = START_X, starty = START_Y;
	static HBITMAP hBitmap;
	static int                    cxClient, cyClient, cxSource, cySource ;
	HDC                           hdc, hdcMem ;
	int                           x, y ;
	PAINTSTRUCT                  ps ;

 	int i;

	switch (message)
	{
		case   WM_CREATE:
			test_init();
			return 0 ;
			
		case   WM_SIZE:
			return 0 ;
		case   WM_TIMER:
			OutputDebugString("TIMER");
			//Invalidate();
			g_frame_id+=1;
			if(g_frame_id >= g_frame_num)
				g_frame_id=0;
			//SendMessage(hwnd, WM_PAINT, 0, 0);
			InvalidateRect(hwnd, NULL, FALSE);
			return 1;
			
		case   WM_PAINT:
			cxSource      = WIDTH;
			cySource      = HEIGHT;
			cxClient = WIDTH;
			cyClient = HEIGHT;
			OutputDebugString("Paint\n");
			
			hdc = GetDC (hwnd);
			hdcMem = CreateCompatibleDC (hdc);
			hBitmap = CreateCompatibleBitmap (hdc, WIDTH, HEIGHT);

			fill_datas();
			
			SetBitmapBits(hBitmap, sizeof datas, datas);
			GetBitmapBits(hBitmap, sizeof datas, datas);				

			ReleaseDC (hwnd, hdc) ;
			SelectObject (hdcMem, hBitmap) ;
			
			hdc = BeginPaint(hwnd, &ps);
			for (y = starty ; y < starty + cyClient ; y += cySource)
			{
				for (x = startx ; x < startx + cxClient ; x += cxSource)
				{
					BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, 0, SRCCOPY) ;
					//StretchBlt (hdc, x, y, cxClient, cyClient,hdcMem, 0, 0, cxSource, cySource, MERGECOPY) ;
				}
			}
			EndPaint (hwnd, &ps) ;
			DeleteDC (hdcMem) ;
			DeleteObject (hBitmap) ;
			return 0 ;
			
		case   WM_DESTROY:
			PostQuitMessage (0) ;
			test_end();
			return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
#endif

#include <stdio.h>
#include <SDL.h>

#define WIDTH  1024
#define HEIGHT 768
#define BPP 4
#define DEPTH 32

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 *pixel;
	Uint32 rgb;  

	rgb = SDL_MapRGB( screen->format, r, g, b );
	
	pixel = (Uint32*) screen->pixels  + y + x;
	*pixel = rgb;
}

void Draws_nacreen(SDL_Surface* screen, int h)
{ 
	int x, y, ytimesw;

	for(y = 0; y < screen->h; y++ ) 
	{
		ytimesw = y*screen->pitch/BPP;
		for( x = 0; x < screen->w; x++ ) 
		{
			setpixel(screen, x, ytimesw, 255, 255, 0);
		}
	}
	

}

int main(int argc, char* argv[])
{
	SDL_Surface *screen;
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
