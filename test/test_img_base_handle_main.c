#include <stdio.h>
#include "windows.h"
#include <assert.h>
#include <main.h>

void test_alpha(void)
{
	int line_pix_num1 = 0, line_pix_num2 = 0;
	bmp_rawdata_t *raw_data1 = NULL;
	bmp_rawdata_t *raw_data2 = NULL;
    bmp_rawdata_t *raw_data3 = NULL;

	build_raw_bmp_data(&raw_data1, ".\\res\\res_bmp\\1.bmp");
	build_raw_bmp_data(&raw_data2, ".\\res\\res_bmp\\2.bmp");

	if(NULL == raw_data1 || NULL == raw_data2)
	{
		return;
	}

    raw_data3 = tplat_blender_two_rawdata_by_alpha256(raw_data1, raw_data2, 200);
    if(NULL == raw_data3)
    {
        return;
    }

	buf2screen_rawdata_t(raw_data3);

	release_raw_bmp_data(&raw_data1);
	release_raw_bmp_data(&raw_data2);
    release_raw_bmp_data(&raw_data3);
}

void test_vert_mirror()
{
	extern int vert_mirror(unsigned long *dst, unsigned long *src, int src_w, int src_h);

	unsigned long *src = NULL, *dst = NULL;
	int w, h;

	src = bmp_to_buf(".\\res\\2.bmp", &w, &h);
	dst = (unsigned long *)malloc(w*h*sizeof(unsigned long));
	assert(src && dst);

	vert_mirror(dst, src, w, h);
	buf_to_screen(dst, w, h);
	
	if(src)
	{
		free(src);
		src = NULL;
	}
	
	if(dst)
	{
		free(dst);
		src = NULL;
	}
}

void test_horz_mirror()
{
	int horz_mirror(unsigned long *dst, unsigned long *src, int src_w, int src_h);

	unsigned long *src = NULL, *dst = NULL;
	int w, h;

	src = bmp_to_buf(".\\res\\2.bmp", &w, &h);
	dst = (unsigned long *)malloc(w*h*sizeof(unsigned long));
	assert(src && dst);

	horz_mirror(dst, src, w, h);
	buf_to_screen(dst, w, h);
	
	if(src)
	{
		free(src);
		src = NULL;
	}
	
	if(dst)
	{
		free(dst);
		src = NULL;
	}
}

void test_bi_value()
{
#define THRESHOLD 128
	int bi_value(unsigned long *buf, int w, int h, unsigned char threshold);

	unsigned long *src = NULL;
	int w, h;

	src = bmp_to_buf(".\\res\\3.bmp", &w, &h);
	assert(src);

	bi_value(src, w, h, THRESHOLD);
	buf_to_screen(src, w, h);
	
	if(src)
	{
		free(src);
		src = NULL;
	}
#undef THRESHOLD
}

void test_corner_mirror()
{
	int corner_mirror(unsigned long *dst, unsigned long *src, int src_w, int src_h);

	unsigned long *src = NULL, *dst = NULL;
	int w, h;

	src = bmp_to_buf(".\\res\\3.bmp", &w, &h);
	dst = (unsigned long *)malloc(w*h*sizeof(unsigned long));
	
	assert(src && dst);

	corner_mirror(dst, src, w, h);
	buf_to_screen(dst, w, h);
	
	if(src)
	{
		free(src);
		src = NULL;
	}

	if(dst)
	{
		free(dst);
		dst = NULL;
	}	
}

int test_dct_transform()
{
	int dct_transform(unsigned long *buf, int width, int height);

	unsigned long *src = NULL;
	int w, h;

	src = bmp_to_buf(".\\res\\3.bmp", &w, &h);
	
	assert(src);

	dct_transform(src, src, w, h);
	buf_to_screen(src, w, h);
	
	if(src)
	{
		free(src);
		src = NULL;
	}
}
