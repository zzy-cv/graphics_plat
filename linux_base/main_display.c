#include <stdio.h>

#include "main.h"
#include "basedraw.h"
// #include "test.h"
#include <assert.h>
#define MY_RGB(r,g,b)    ((unsigned int)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|\
                 (((unsigned int)(unsigned char)(b))<<16)))
#define MY_RGB_FOR_BMP(r,g,b)  ((unsigned int)(((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|\
                 (((unsigned int)(unsigned char)(r))<<16)))     
#define MERGE_RGB565(r, g, b) (((b) & 31) + (((g) & 63) << 5) + (((r) & 31) << 11))
#define RGB888TO565(r, g, b)  MERGE_RGB565(((r) >> 3), ((g) >> 2), ((b) >> 3))
#define EXTRACT_RGB888(clr, r, g, b)     do{ \
r = ((clr) >> 16) & 255; g = ((clr) >> 8) & 255; b = (clr) & 255; \
}while(0)
	
#define EXTRACT_RGB565(clr, r, g, b)	    {\
r = ((clr) >> 11) & 31; g = ((clr) >> 5) & 63; b = (clr) & 31;\
}while(0)
	
#define RGB565TO888(r, g, b) (((r)<<19)|((g)<<10)|(b<<3))

static int width = 0;
static int height = 0;
static unsigned long *datas_ptr = NULL;

static void init(void);
static void test_main(void);
static void test1(void);
static void test_draw_line(void);

int rgb888to565(int clr)
{
	unsigned char r, g, b;
	EXTRACT_RGB888(clr, r, g, b);	
	return RGB888TO565(r,g,b);
	
}

int rgb565to888(int clr)
{
	unsigned char r, g, b;
	EXTRACT_RGB565(clr,r,g,b);
	return RGB565TO888(r, g, b);
}

void fill_datas(void)
{
	init();
	test_draw();
}

void putpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
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

void putpixel_big(int x, int y, unsigned long color, int factor)
{
	int i, j;
	for(i=0; i<factor; i++)
	{
		for(j=0; j<factor; j++)
		{
			datas_ptr[factor*x+i+(factor*y+j)*width] = MY_RGB_FOR_BMP((unsigned char)color, (unsigned char)(color>>8), (unsigned char)(color>>16)); 
		}
	}
}

void putpixel_bmpbuf(bmp_rawdata_t *rawdata, int x, int y, unsigned long color)
{
	rawdata->rawdata[x+y*rawdata->width] = MY_RGB_FOR_BMP((unsigned char)color, (unsigned char)(color>>8), (unsigned char)(color>>16)); 
}

void buf_to_screen(unsigned long *buf, int w, int h)
{
	int i, j;
	
    for(j = 0; j < h; j++)
    {
        for(i = 0; i < w; i++)
        {
            datas_ptr[i+width*j] = buf[i+w*j];
        }
    }
}

void buf2screen_rawdata_t(bmp_rawdata_t *rawdata)
{
    int i, j;
    int w, h;
    
    if(NULL == rawdata || NULL == rawdata->rawdata)
    {
        return;
    }

    w = rawdata->width;
    h = rawdata->height;
    for(j=0; j<h; j++)
    {
        for(i=0; i<w; i++)
        {
            datas_ptr[i+width*j] = rawdata->rawdata[i+w*j];
        }
    }
}

void show_raw_buf(bmp_rawdata_t *rawdata, int x, int y)
{
	int i,j;
	int width_bmp, height_bmp;

	width_bmp = rawdata->width;
	height_bmp = rawdata->height;

    for(j = 0; j < height_bmp; j++)
    {
        for(i = 0; i < width_bmp; i++)
        {
        	int left = i+x, top = j+y;
			
			datas_ptr[left+top*width] = rawdata->rawdata[i+j*width_bmp]; 
        }
    }
}

void show_raw_buf16(unsigned short *buf, int x, int y, int width_bmp, int height_bmp)
{
	int i, j;

	for(j=0; j<height_bmp; j++)
	{
		for(i=0; i<width_bmp; i++)
		{
			int left = i+x, top = j+y;
			datas_ptr[left+top*width] = rgb565to888(buf[i+j*width_bmp]);
		}
	}
}

#define RAW_BUF_DIFF_LOG

#ifdef RAW_BUF_DIFF_LOG
static FILE *f = NULL;
#endif
void show_raw_buf_diff(bmp_rawdata_t *rawdata1, bmp_rawdata_t *rawdata2, int x, int y)
{
	int i,j;
	int width_bmp, height_bmp;

	#ifdef RAW_BUF_DIFF_LOG
	if(!(f = fopen("raw_buf_diff.txt", "w+")))
	{
		assert(0);
	}
	#endif

	assert(rawdata1->width == rawdata2->width && rawdata1->height == rawdata2->height);
	width_bmp = rawdata1->width;
	height_bmp = rawdata1->height;

    for(j = 0; j < height_bmp; j++)
    {
        for(i = 0; i < width_bmp; i++)
        {
        	int left = i+x, top = j+y;
			if(rawdata1->rawdata[i+j*width_bmp] == rawdata2->rawdata[i+j*width_bmp])
			{
				datas_ptr[left+top*width] = 0x0;
			}
			else
			{
				#ifdef RAW_BUF_DIFF_LOG
				fprintf(f, "%x[%d,%d] ", rawdata2->rawdata[i+j*width_bmp], i, j);
				#endif
				datas_ptr[left+top*width] = 0xffffff;
				//datas_ptr[left+top*width] = rawdata2->rawdata[i+j*width_bmp]; 
			}
			#ifdef RAW_BUF_DIFF_LOG
			fprintf(f, "\n");
			#endif
        }
    }
	#ifdef RAW_BUF_DIFF_LOG
	if(f)
	{
		fclose(f);
		f = NULL;
	}
	#endif
}

int show_diff_buf(const unsigned int *buf, int width_bmp)
{
	int x = buf[0], y = buf[1], num = buf[2];
	int i = 0;

	x = buf[0];
	y = buf[1];
	num = buf[2];
	i = 3;
	
	while(x != -1)
	{
		while(num--)
		{
			datas_ptr[x+y*width] = buf[i++];
			x++;
		}
		x = buf[i++];
		y = buf[i++];
		num = buf[i++];
		//i = i + num;
	}
}

int show_diff_buf16(const unsigned short *buf, int width_bmp)
{
	unsigned short x = buf[0], y = buf[1], num = buf[2];
	int i = 0;

	x = buf[0];
	y = buf[1];
	num = buf[2];
	i = 3;
	
	while(x != (unsigned short)-1)
	{
		while(num--)
		{
			datas_ptr[x+y*width] = rgb565to888(buf[i++]);
			x++;
		}
		x = buf[i++];
		y = buf[i++];
		num = buf[i++];
		//i = i + num;
	}
}

static void init(void)
{
	width = get_width();
	height = get_height();
	datas_ptr = get_datas_ptr();
}

