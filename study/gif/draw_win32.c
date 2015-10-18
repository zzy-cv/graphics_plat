#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "stdlib.h"

static HDC hdc;
static HWND hwnd;

#define MY_RGB(r,g,b)    ((unsigned int)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|\
                 (((unsigned int)(unsigned char)(b))<<16)))
 
// 编译命令: cl test.c /link libc.lib kernel32.lib user32.lib gdi32.lib
void dc_init(void)
{
	char arg[200]={0};
	char module_name[100];
	int module_name_size = 100;
	int file_name_start = 0;
	int n = 0;

	memset(module_name, 0, module_name_size);
	GetModuleFileName(NULL,module_name,module_name_size);
	while(module_name[n])
	{
		if(module_name[n] == '\\')
		{
			file_name_start = n+1;
		}
		n++;
	}

	arg[0]='\"';
	strcpy(arg+1, &module_name[file_name_start]);
	arg[strlen(arg)]='\"';

	//找到程序运行窗口的句柄
	hwnd=FindWindow(NULL, arg);
	
	//通过窗口句柄得到该窗口的设备场境句柄
	hdc=GetDC(hwnd);
	//printf("%d", hdc);

}

void dc_release(void)
{
	ReleaseDC(hwnd,hdc);
}

void putpixel(int i, int j, unsigned int color)
{
	static int init = 0;

	SetPixel(hdc, i, j, color);//用画点的办法画一根线,最后一个参数是颜色（32位）
}

void putpixel_by_rgb(int i, int j, int r, int g, int b)
{
	putpixel(i, j, MY_RGB(r, g, b));
}

void putpixel_test()
{
	int i;
	unsigned int color = 0x0;
	for(i=0; i<500; i++)
	{
		putpixel_by_rgb(i, 50, 0xff, 0, 0);
	}
}

int main(int argc, char **argv)
{
		dc_init();
		main_test();
		dc_release();

		return 0;
}

