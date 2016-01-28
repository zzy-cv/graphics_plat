#ifndef __MAIN_H__
#define __MAIN_H__

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

#define RGB MY_RGB

// structure for bmp
typedef struct bmp_rawdata_t_tag
{
	int width;
	int height;
	int depth;
	unsigned int *rawdata;
}bmp_rawdata_t;

// structure for file
typedef struct file_data_type
{
#define MAX_FILE_NUM_IN_DIR  1024
#define MAX_FILE_NAME_LEN 256
	int num;
	char s_file_arr[MAX_FILE_NUM_IN_DIR][MAX_FILE_NAME_LEN];
}files_data_t;

extern void fill_datas(void);
extern void putpixel_for_rgb(int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern void putpixel(int x, int y, unsigned int color);

extern unsigned long *get_datas_ptr(void);
extern int get_width();
extern int get_height();

#endif	
