#ifndef __MAIN_H__
#define __MAIN_H__

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
extern void putpixel(int x, int y, unsigned long color);

extern unsigned long *get_datas_ptr(void);
extern int get_width();
extern int get_height();

#endif	
