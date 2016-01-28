#include "basedraw.h"
#include "main.h"
#include "math.h"

void clear(unsigned int color)
{
	int x, y;
	
	int w = get_width();
	int h = get_height();

	for(y = 0; y < h; y++)
		for(x = 0; x < w; x++)
			putpixel(x, y, color);
}

void clear_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	int i, j;
	int w = get_width();
	int h = get_height();
	for(i = 0; i < h; i++)
		for(j = 0; j < w; j++)
			putpixel_for_rgb(i, j, r, g, b);	
}


static void CirclePoints(int x, int y, int t, int s, int mycolor)
{
    putpixel(t+x, s+y, mycolor);
    putpixel(t-x, s+y, mycolor);
    putpixel(t+x, s-y, mycolor);
    putpixel(t-x, s-y, mycolor);
}

/*----------------------------Bresenham算法生成圆-----------------------------*/
void DrawCircle(int centreX, int centreY, int R, int mycolor)
{
    int x, y, delt, dHD, deltV, next;
    x = 0;
    y = R;
    delt = 2 * (1-R);
    while(y >= 0)
    {
        CirclePoints(x, y, centreX, centreY, mycolor);   /* 八分法 */
        if(delt < 0)
        {
            dHD = 2 * (delt + y) - 1;
            if(dHD <= 0)
            {
                next = 0;
            }
            else
            {
                next = 1;
            }
        }
        else if(delt > 0)
        {
            deltV = 2 * (delt - x) - 1;
            if(deltV <= 0)
            {
                next = 1;
            }
            else
            {
                next = 2;
            }
        }
        else
        {
            next = 1;
        }
        switch(next)
        {
            case 0: x++;
                    delt += 2 * x + 1;
                    break;
            case 1: x++;
                    y--;
                    delt += 2 * (x - y + 1);
                    break;
            case 2:y--;
                    delt += -2 * y + 1;
                    break;
        } /*switch*/
      }  /*while*/
}

/* Bresenham算法   */
void draw_line(int x1, int y1, int x2, int y2, long color)
{
    int i;
    int x, y;

    if (y1 == y2)
    {
        if (x2 > x1)
        {
            for (i = x1; i <= x2; i++)
            {
                putpixel(i, y1, color);
            }
        }
        else
        {
            for (i = x1; i >= x2; i--)
            {
                putpixel(i, y1, color);
            }
        }
    }
    else if (x1 == x2)
    {
        if (y2 > y1)
        {
            for (i = y1; i <= y2; i++)
            {
                putpixel(x1, i, color);
            }
        }
        else
        {
            for (i = y1; i >= y2; i--)
            {
                putpixel(x1, i, color);
            }
        }
    }
    else
    {
        int dx = x2-x1;
        int dy = y2-y1;
        int delt = dy/dx;
        int xinc, yinc;
        int dxa, dya;

        x = x1;
        y = y1;

        if (dx>0)
        {
            dxa = (unsigned long)dx;
            xinc = 1;
        }
        else
        {
            dxa = (unsigned long)(-dx);
            xinc = -1;
        }
        if (dy>0)
        {
            dya = (unsigned long)dy;
            yinc = 1;
        }
        else
        {
            dya = (unsigned long)(-dy);
            yinc = -1;
        }
        if (delt)
        {
            int count=dya;
            do
            {
                putpixel(x, y, color);
                count -= dxa;
                if(count<0)
                {
                    count += dya;
                    x += xinc;
                }
                y += yinc;
            }while(y != y2);
        }
        else
        {
            int count=dxa;
            do
            {
                putpixel(x, y, color);
                count -= dya;
                if(count < 0)
                {
                    count += dxa;
                    y += yinc;
                }
                x += xinc;
            }while(x != x2);
        }
    }
}

/* 画带箭头的直线 */
static double   get_distance(double   start_x, double   start_y, double   end_x, double   end_y)
{
    double   result, sqrt_f;
    result = (start_x-end_x)*(start_x-end_x)+(start_y-end_y)*(start_y-end_y);
    sqrt_f = sqrt(result);
    return   sqrt_f;
}

static void draw_arraw_line_imp(double  s_x, double  s_y, double  e_x, double  e_y, double length, double angle)
{
    double   tmp_x;
    double   tmp_y;
    double   x1;
    double   y1;
    double   x2;
    double   y2;

    double distance = get_distance(s_x,s_y,e_x,e_y);

    tmp_x = e_x + (s_x-e_x)*length/distance;
    tmp_y = e_y + (s_y-e_y)*length/distance;
    x1 = (tmp_x-e_x)*cos(-angle/2) - (tmp_y-e_y)*sin(-angle/2) + e_x;
    y1 = (tmp_y-e_y)*cos(-angle/2) + (tmp_x-e_x)*sin(-angle/2) + e_y;
    x2 = (tmp_x-e_x)*cos(angle/2) - (tmp_y-e_y)*sin(angle/2) + e_x;
    y2 = (tmp_y-e_y)*cos(angle/2) + (tmp_x-e_x)*sin(angle/2) + e_y;

    draw_line((int)x1, (int)y1, (int)e_x, (int)e_y, RGB(0, 0, 0xff));
    draw_line((int)x2, (int)y2, (int)e_x, (int)e_y, RGB(0, 0, 0xff));
}

#define PI 3.14159265
static double degree_to_radian(double degree)
{
    double radian = degree * PI / 180.0;
    return radian;
}

static void draw_arraw_line(double  s_x, double  s_y, double  e_x, double  e_y)
{
    draw_line((int)s_x, (int)s_y, (int)e_x, (int)e_y, RGB(0, 0, 0xff));
    draw_arraw_line_imp((int)s_x, (int)s_y, (int)e_x, (int)e_y, 15, degree_to_radian(60));
}

void draw_arraw_line_with_angle(double start_x, double start_y, double length, double degree)
{
    double delt_x, delt_y, end_x, end_y, radian;

    radian = degree_to_radian(degree);
    delt_x = length * cos(radian);
    delt_y = length * sin(radian);

    end_x = start_x + delt_x;
    end_y = start_y - delt_y;

    draw_arraw_line(start_x, start_y, end_x, end_y);
}
