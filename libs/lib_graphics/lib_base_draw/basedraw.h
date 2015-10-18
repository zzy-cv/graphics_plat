#ifndef  __MAIN_DRAW_H__
#define  __MAIN_DRAW_H__

#include <windows.h>

void open_draw(HWND hWnd);
void close_draw(HWND hWnd);

void putpixel(int x, int y, long color);                          /* ���� */
void clear(long color);
void clear_rgb(unsigned char r, unsigned char g, unsigned char b);
void draw_line(int x1, int y1, int x2, int y2, long color);       /* ��ֱ�� */   
void draw_arraw_line_with_angle(double start_x, double start_y, double length, double degree);/* ������ͷ��ֱ�� */

void DrawCircle(int centreX, int centreY, int R, int mycolor);  /* ��Բ */


#endif