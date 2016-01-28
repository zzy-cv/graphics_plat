#ifndef  __MAIN_DRAW_H__
#define  __MAIN_DRAW_H__

#ifdef WIN32
#include <windows.h>
#else
#endif

#ifdef WIN32
void open_draw(HWND hWnd);
void close_draw(HWND hWnd);
#else
#endif

extern void putpixel(int x, int y, unsigned int color);                          /* 画点 */
void clear(unsigned int color);
void clear_rgb(unsigned char r, unsigned char g, unsigned char b);
void draw_line(int x1, int y1, int x2, int y2, long color);       /* 画直线 */   
void draw_arraw_line_with_angle(double start_x, double start_y, double length, double degree);/* 画带箭头的直线 */

void DrawCircle(int centreX, int centreY, int R, int mycolor);  /* 画圆 */


#endif