#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "basedraw.h"
// #include "test.h"
#include <assert.h>

static int width = 0;
static int height = 0;
static unsigned int *datas_ptr = NULL;

static void init(void);
static void test_main(void);
static void test1(void);
static void test_draw_line(void);

extern void test_draw(void);

void set_width(int w) {
	width = w;
}

void set_height(int h) {
	height = h;
}

void set_display_buf(void) {
	datas_ptr = malloc(width*height*sizeof(unsigned int));
}

int get_width() {
	return width;
}

int get_height() {
	return height;
}

unsigned int *get_display_buf() {
	return datas_ptr;
}

void draw_display_buffer(void)
{
	test_draw();
	// init();
	// test_draw();
}

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

void putpixel_for_rgb(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	datas_ptr[x+y*width] = MY_RGB_FOR_BMP(r, g, b); 
}

void putpixel(int x, int y, unsigned int color)
{
	datas_ptr[x+y*width] = color; //MY_RGB_FOR_BMP((unsigned char)color, (unsigned char)(color>>8), (unsigned char)(color>>16)); 
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