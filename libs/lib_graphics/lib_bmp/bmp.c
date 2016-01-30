#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "bmp.h"
#include "basedraw.h"
#include "trace.h"
#include "assert.h"

/* 宏定义 */
#define  BITMAPFILETYPE  0x4d42           /* 位图文件类型 */
#define BMP_CEIL4(x)        (((x)+3)&-4)  // for byte align
#define BMP_CEIL4_DIFF(x)   (-x&3)

/*  数据类型的定义 */
#pragma pack(1)
typedef struct tag_tplat_BITMAPFILEHEADER         /* 位图文件信息头 */
{
	unsigned short int bfType;             /* 文件的类型，该值必需是0x4D42，也就是字符'BM' */
	unsigned int bfSize;                  /* 整个文件的大小, 单位字节 */
	unsigned int bf_reserved;             /* 保留，必须设置为0 */
	unsigned int bfOffBits;               /* 从文件开始到位图数据开始之间的数据之间的偏移量 */
}tplat_BITMAPFILEHEADER;

#pragma pack(1)
typedef struct tag_tplat_BITMAPINFOHEADER        /* 图象信息头 */
{
	unsigned int biSize;                 /* 图象信息头的长度 */             
	int biWidth;                         /* 位图的宽度 */  
	int biHeight;                        /* 位图的高度 */
	unsigned short int biPanes;                 /* 位图的位面数，其值恒为1 */
	unsigned short int biBitCount;              /* 位图每个象素的位数 */
	unsigned int biCompression;          /* 	压缩说明 */
	unsigned int biSizeImage;      /* 用字节数表示的位图数据的大小。该数必须是4的倍数 */
	int biXPelsPerMeter;           /* 用象素/米表示的水平分辨率 */
	int biYPelsPerMeter;           /* 用象素/米表示的垂直分辨率 */
	unsigned int biClrUsed;        /* 位图使用的颜色数 */
	unsigned int biClrImportant;   /* 指定重要的颜色数 */
}tplat_BITMAPINFOHEADER;

#pragma pack(1)
typedef struct tag_tplat_RGBQUAD
{
	unsigned char rgbBlue;          /* 蓝色分量 */
	unsigned char rgbGreen;         /* 绿色分量 */
	unsigned char rgbRed;           /* 红色分量 */
	unsigned char rgbReserved;      /* 保留 */
}tplat_RGBQUAD;

#pragma pack(1)
typedef struct tag_tplat_BITMAPINFO
{
	tplat_BITMAPINFOHEADER bmiHeader;
	tplat_RGBQUAD bmiColors[];
}tplat_BITMAPINFO;

/* 模块变量 */
static FILE *f_bmp = NULL;

// read a bmp file, and draw it directly
static int begin_draw_bmp(unsigned char *file_name, int *colors, int *height, int *width)
{
	tplat_BITMAPFILEHEADER bmfh;
	tplat_BITMAPINFOHEADER bih;

	printf("open bmp: %s\n", file_name);
	if((f_bmp=fopen(file_name,"rb"))==NULL)
	{
		printf("bmp file open error!\n");
		return -1;
	}

	printf("sizeof unsigned long=%d\n", sizeof(unsigned long));
	printf("sizeof unsigned int=%d\n", sizeof(unsigned int));
	printf("sizeof unsigned short=%d\n", sizeof(unsigned short));
	fread(&bmfh,sizeof(tplat_BITMAPFILEHEADER),1,f_bmp);
	printf("bmp magic: %x, bmp size: %x, bf_reserved=%x, bfOffBits=%x\n", bmfh.bfType, bmfh.bfSize, bmfh.bf_reserved, bmfh.bfOffBits);
	if((bmfh.bfType != BITMAPFILETYPE) || (bmfh.bf_reserved != 0))
	{
		printf("bmp file header error!\n");
		return -1;
	}

	fread(&bih,sizeof(tplat_BITMAPINFOHEADER),1,f_bmp);
	*height = bih.biHeight;
	*width = bih.biWidth;
	*colors = bih.biBitCount;
	return 0;
}

static void draw_less_8bits_bmp(int bits, int height, int width)
{
	int i,j;
	unsigned char rgb;
	int index;
	tplat_RGBQUAD *temp;
	tplat_RGBQUAD rgb_data;

	temp = (tplat_RGBQUAD *)malloc(sizeof(tplat_RGBQUAD) * (1 << bits));
	fread(temp, sizeof(tplat_RGBQUAD), (1 << bits), f_bmp);

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			switch(bits)
			{
				case 1:
					if(i%8 == 0)
					{
						fread(&rgb, 1, 1, f_bmp);
						index = rgb>>7;
						rgb_data = temp[index];
						putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					}
					else
					{
						index = (rgb >> (7 - j%8))%2;
						rgb_data = temp[index];
						putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));	
					}
					break;

				case 4:
					if(i%2 == 0)
					{
						fread(&rgb, 1, 1, f_bmp);
						index = rgb/16;
						rgb_data = temp[index];
						putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					}
					else
					{
						index = rgb%16;
						rgb_data = temp[index];
						putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));	
					}
					break;

				case 8:
					fread(&rgb, 1, 1, f_bmp);
					index = rgb;
					rgb_data = temp[index];
					putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					break;

				default:
					break;
			}
		}
	}
	free(temp);
}

static void draw24bits_bmp(int bits, int height, int width)
{
	int i,j;
	unsigned char r, g, b;
	int index;

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			fread(&b, 1, 1, f_bmp);
			fread(&g, 1, 1, f_bmp);
			fread(&r, 1, 1, f_bmp);
			putpixel(i, height - 1 - j, MY_RGB_FOR_BMP(r, g, b));
			if((width*3 % 4) && (i == width-1))  // bmp图片行数据 4 字节对齐, 未优化
			{
				int dumy, nbyte = 0;
				nbyte = 4 - width*3%4;
				fread(&dumy, 1, nbyte, f_bmp);
			}
		}
	}
}

static void end_draw_bmp()
{
	fclose(f_bmp);
}

int draw_bmp(char *bmp_name)
{
	int bits;
	int height, width;

	if(begin_draw_bmp(bmp_name, &bits, &height, &width))
	{
		// error ...
		return -1;
	}

	switch(bits)
	{
		case 1:
		case 4:
		case 8:
			draw_less_8bits_bmp(bits, height, width);
			break;
		case 24:
			draw24bits_bmp(bits, height, width);
			break;
		default:
			break;
	}

	end_draw_bmp();
	return 0;
}

// get bmp rgb value to a buffer
static int get_bmp_major_info(unsigned char *file_name, int *width, int *height, int *colors)
{
	tplat_BITMAPFILEHEADER bmfh;
	tplat_BITMAPINFOHEADER bih;
	//FILE *f_bmp = NULL;

	if((f_bmp=fopen(file_name,"rb"))==NULL)
	{
		return -1;
	}

	fread(&bmfh,sizeof(tplat_BITMAPFILEHEADER),1,f_bmp);
	if((bmfh.bfType != BITMAPFILETYPE) || (bmfh.bf_reserved != 0))
	{
		return -1;
	}

	fread(&bih,sizeof(tplat_BITMAPINFOHEADER),1,f_bmp);
	*height = bih.biHeight;
	*width = bih.biWidth;
	*colors = bih.biBitCount;
	//fclose(f_bmp);
	return 0;
}

static void draw_less_8bits_bmp_buf(bmp_rawdata_t *bmp_buf)
{
	int i,j;
	unsigned char rgb;
	int index;
	tplat_RGBQUAD *temp;
	tplat_RGBQUAD rgb_data;
	int bits, height, width;

	bits = bmp_buf->depth;
	height = bmp_buf->height;
	width = bmp_buf->width;

	temp = (tplat_RGBQUAD *)malloc(sizeof(tplat_RGBQUAD) * (1 << bits));
	fread(temp, sizeof(tplat_RGBQUAD), (1 << bits), f_bmp);

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			switch(bits)
			{
				case 1:
					if(i%8 == 0)
					{
						fread(&rgb, 1, 1, f_bmp);
						index = rgb>>7;
						rgb_data = temp[index];
						//putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
						putpixel_bmpbuf(bmp_buf, i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					}
					else
					{
						index = (rgb >> (7 - j%8))%2;
						rgb_data = temp[index];
						//putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
						putpixel_bmpbuf(bmp_buf, i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					}
					break;

				case 4:
					if(i%2 == 0)
					{
						fread(&rgb, 1, 1, f_bmp);
						index = rgb/16;
						rgb_data = temp[index];
						//putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
						putpixel_bmpbuf(bmp_buf, i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					}
					else
					{
						index = rgb%16;
						rgb_data = temp[index];
						//putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
						putpixel_bmpbuf(bmp_buf, i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					}
					break;

				case 8:
					fread(&rgb, 1, 1, f_bmp);
					index = rgb;
					rgb_data = temp[index];
					//putpixel(i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					putpixel_bmpbuf(bmp_buf, i, height - 1 - j, RGB(rgb_data.rgbRed, rgb_data.rgbGreen, rgb_data.rgbBlue));
					break;

				default:
					break;
			}
		}
	}
	free(temp);
}

static void draw24bits_bmp_buf(bmp_rawdata_t *bmp_buf)
{
	int i,j;
	unsigned char r, g, b;
	int index;
	int height, width;

	height = bmp_buf->height;
	width = bmp_buf->width;

	for(j = 0; j < height; j++)
	{
		for(i = 0; i < width; i++)
		{
			fread(&b, 1, 1, f_bmp);
			fread(&g, 1, 1, f_bmp);
			fread(&r, 1, 1, f_bmp);
			putpixel_bmpbuf(bmp_buf, i, height - 1 - j, RGB(r, g, b));
			if((width*3 % 4) && (i == width-1))  // bmp图片行数据 4 字节对齐, 未优化
			{
				int dumy, nbyte = 0;
				nbyte = 4 - width*3%4;
				fread(&dumy, 1, nbyte, f_bmp);
			}            
		}
	}
}

int draw_bmp_buf(bmp_rawdata_t *bmp_buf)
{
	assert(bmp_buf);

	switch(bmp_buf->depth)
	{
		case 1:
		case 4:
		case 8:
			//draw_less_8bits_bmp(bmp_buf->depth, bmp_buf->height, bmp_buf->width);
			draw_less_8bits_bmp_buf(bmp_buf);
			break;
		case 24:
			//draw24bits_bmp(bmp_buf->depth, bmp_buf->height, bmp_buf->width);
			draw24bits_bmp_buf(bmp_buf);
			break;
		default:
			break;
	}

	end_draw_bmp();
	return 0;
}

unsigned int *bmp_to_buf(unsigned char *filename, int *w, int *h)
{
	int bits;
	int height, width;
	unsigned int *buf = NULL;

	assert(h&&w);

	if(begin_draw_bmp(filename, &bits, &height, &width))
	{
		return NULL;
	}

	*h = height;
	*w = width;
	buf = (unsigned int *)malloc(width*height*sizeof(unsigned int));
	assert(buf);
	switch(bits)
	{
		case 1:
		case 4:
		case 8:
			// todo
			// draw_less_8bits_bmp(buf, bits, height, width);
			break;
		case 24:
			// todo
			//draw24bits_bmp(buf, bits, height, width);
			break;
		default:
			break;
	}

	end_draw_bmp();
	return buf;
}

int build_raw_bmp_data(bmp_rawdata_t **raw_data, unsigned char *file_name)
{
	bmp_rawdata_t *tmp_data = NULL;
	int *tmp = NULL;

	assert(raw_data);
	*raw_data = malloc(sizeof(bmp_rawdata_t));
	assert(*raw_data);
	tmp_data = *raw_data;
	memset(tmp_data, 0, sizeof(bmp_rawdata_t));
	if(get_bmp_major_info(file_name, &tmp_data->width, &tmp_data->height, &tmp_data->depth))
	{
		assert(0);
	}

	tmp = (int *)malloc(tmp_data->width*tmp_data->height*4);
	assert(tmp);
	tmp_data->rawdata = (unsigned int *)tmp;
	draw_bmp_buf(tmp_data);
	return 0;
}

int release_raw_bmp_data(bmp_rawdata_t **raw_data)
{
	bmp_rawdata_t *tmp = *raw_data;

	if(tmp)
	{
		if(tmp->rawdata)
		{
			free(tmp->rawdata);
		}
		free(tmp);
	}
	*raw_data = NULL;
}

