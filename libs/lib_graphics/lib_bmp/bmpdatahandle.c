#include <stdio.h>
#include "main.h"
// #include "test.h"
#include "basedraw.h"
#include "dot_font.h"
#include "bmp.h"
#include "assert.h"

#define FILE_TEST_LOG
#define LOG_BMP_FILES_NAME
#define LOG_BMP_COMMON_INFO


#define  BMP_RAW_DATA_MAX_LEN     1024

static bmp_rawdata_t *s_bmp_raw_data[BMP_RAW_DATA_MAX_LEN] = {NULL};
static int *s_bmp_diff_data[BMP_RAW_DATA_MAX_LEN] = {NULL};
static files_data_t *s_files_data = NULL;
static unsigned short *s_bmp_raw_data16[BMP_RAW_DATA_MAX_LEN] = {NULL};
static unsigned short *s_bmp_diff_data16[BMP_RAW_DATA_MAX_LEN] = {NULL};

#ifdef FILE_TEST_LOG
static FILE *f = NULL;
#endif

bmp_rawdata_t **bmp_get_bmp_raw_data(void)
{
	return s_bmp_raw_data;
}

files_data_t *bmp_get_files_data(void)
{
	return s_files_data;
}

int bmp_get_frame_num(void)
{
	assert(s_files_data);
	return s_files_data->num;
}

int bmp_get_width_each_frame(void)
{
	assert(s_bmp_raw_data[0]);
	return s_bmp_raw_data[0]->width;
}

int bmp_get_height_each_frame(void)
{
	assert(s_bmp_raw_data[0]);
	return s_bmp_raw_data[0]->height;
}

int **bmp_get_diff_data(void)
{
	return s_bmp_diff_data;
}

int **bmp_get_diff_data16(void)
{
	return s_bmp_diff_data16;
}

void bmp_data_close_test_file(void)
{
	// to do
}

void bmp_release_raw_bmp_data(void)
{
	int i;
	for(i=0; i<BMP_RAW_DATA_MAX_LEN; i++)
	{
		release_raw_bmp_data(&s_bmp_raw_data[i]);
	}
}

void bmp_release_bmp_diff_data(void)
{
	int i = 0;
	for(i=0; i<BMP_RAW_DATA_MAX_LEN; i++)
	{
		if(s_bmp_diff_data[i])
		{
			free(s_bmp_diff_data[i]);
			s_bmp_diff_data[i] = NULL;
		}
	}
}

void bmp_release_bmp_raw_data16(void)
{
	int i;
	for(i=0; i<BMP_RAW_DATA_MAX_LEN; i++)
	{
		if(s_bmp_raw_data16[i])
		{
			free(s_bmp_raw_data16[i]);
			s_bmp_raw_data16[i] = NULL;
		}
	}
}

void bmp_release_bmp_diff_data16(void)
{
	int i;
	for(i=0; i<BMP_RAW_DATA_MAX_LEN; i++)
	{
		if(s_bmp_diff_data16[i])
		{
			free(s_bmp_diff_data16[i]);
			s_bmp_diff_data16[i] = NULL;
		}
	}
}

#if 0
int bmp_build_raw_bmp_data_by_dir(const char *file_dir)
{
	int i, num = 0;
	s_files_data = file_get_all_files_in_dir(file_dir); // files_data, static data
	assert(s_files_data);

	{
		memset(s_bmp_raw_data, 0, BMP_RAW_DATA_MAX_LEN*4);
	}

#ifdef LOG_BMP_FILES_NAME
	if(!(f=fopen("log_bmp_files_name.txt", "w+")))
	{
		assert(0);
	}
	fprintf(f, "files number=%d\n", s_files_data->num);
	if(s_files_data)
	{
		num = s_files_data->num;
		for(i=0; i<num; i++)
		{
			fprintf(f, "%s\n", file_get_file_name_by_id(i));
		}
	}
	if(f)
	{
		fclose(f);
		f = NULL;
	}
#endif

	{
		int i, num = s_files_data->num;
		for(i=0; i<num; i++)
		{
			if(build_raw_bmp_data(&s_bmp_raw_data[i], s_files_data->s_file_arr[i]))
			{
				assert(0);

#ifdef LOG_BMP_COMMON_INFO
				if(i == 0)
				{
					if(!(f=fopen("log_bmp_common_info.txt", "w+")))
					{
						assert(0);
					}

					if(s_bmp_raw_data[0])
					{
						fprintf(f, "width=%d\n", s_bmp_raw_data[0]->width);
						fprintf(f, "height=%d\n", s_bmp_raw_data[0]->height);
						fprintf(f, "depth=%d\n", s_bmp_raw_data[0]->depth);		
					}
					
					if(f)
					{
						fclose(f);
						f = NULL;
					}
				}
#endif

			}
		}
	}
	return 0;
}
#endif

int bmp_build_raw_bmp_data16(void)
{
	int num = 0, i;
	int width = 0, height = 0;
	int pixels = 0;
	
	memset(s_bmp_raw_data16, 0, BMP_RAW_DATA_MAX_LEN*4);
	assert(s_files_data);
	num = s_files_data->num;
	assert(s_bmp_raw_data[0]);
	width = s_bmp_raw_data[0]->width;
	height = s_bmp_raw_data[0]->height;
	pixels = width*height;
	
	for(i=0; i<num; i++)
	{
		int j = 0;
		unsigned int *src_buf = s_bmp_raw_data[i]->rawdata;
		s_bmp_raw_data16[i] = (unsigned short *)malloc(width*height*sizeof(unsigned short));
		assert(s_bmp_raw_data16[i]);
		assert(s_bmp_raw_data[i]);
		for(j=0; j<pixels; j++)
		{
			s_bmp_raw_data16[i][j] = rgb888to565(s_bmp_raw_data[i]->rawdata[j])&0xffff;
		}
	}
	
	return 0;
}

unsigned short **bmp_get_raw_bmp_data16(void)
{
	return s_bmp_raw_data16;
}

int bmp_build_diff_bmp_data(void)
{
	int frame_num = 0, i;
	int w = 0, h = 0;

	frame_num = bmp_get_frame_num();
	w = bmp_get_width_each_frame();
	h = bmp_get_height_each_frame();

	memset(s_bmp_diff_data, 0, BMP_RAW_DATA_MAX_LEN*4);
	for(i=1; i<frame_num; i++)
	{
		int w_i = 0, h_i = 0;
		int *raw_data1 = NULL;
		int *raw_data2 = NULL;
		int diff_data_pos = 0;
		int trace_h_i = -1;

		assert(s_bmp_raw_data[i-1]);
		assert(s_bmp_raw_data[i]);
		raw_data1 = s_bmp_raw_data[i-1]->rawdata;
		raw_data2 = s_bmp_raw_data[i]->rawdata;
		assert(raw_data1);
		assert(raw_data2);

		s_bmp_diff_data[i] = (int *)malloc(w*h*sizeof(int)*4);
		assert(s_bmp_diff_data[i]);
		memset(s_bmp_diff_data[i], 0, w*h*sizeof(int)*4);

		for(h_i=0; h_i<h; h_i++)
		{
			int trace_data_pos = 0;
			int trace_w_i = -1;
			for(w_i=0; w_i<w; w_i++)
			{
				if(raw_data1[w_i+h_i*w] != raw_data2[w_i+h_i*w])
				{
					if(w_i == 0)
					{
						s_bmp_diff_data[i][diff_data_pos++] = w_i;
						s_bmp_diff_data[i][diff_data_pos++] = h_i;
						trace_data_pos = diff_data_pos++;
						s_bmp_diff_data[i][diff_data_pos++] = raw_data2[w_i+h_i*w];
						s_bmp_diff_data[i][trace_data_pos] = 1;
					}
					else if(trace_h_i == h_i && trace_w_i+1 == w_i)
					{
						s_bmp_diff_data[i][diff_data_pos++] = raw_data2[w_i+h_i*w];
						s_bmp_diff_data[i][trace_data_pos]++;
					}
					else
					{
						s_bmp_diff_data[i][diff_data_pos++] = w_i;
						s_bmp_diff_data[i][diff_data_pos++] = h_i;
						trace_data_pos = diff_data_pos++;
						s_bmp_diff_data[i][diff_data_pos++] = raw_data2[w_i+h_i*w];
						s_bmp_diff_data[i][trace_data_pos] = 1;
					}
					trace_w_i = w_i;
					trace_h_i = h_i;
				}
			}
		}
		s_bmp_diff_data[i][diff_data_pos++] = -1;
		s_bmp_diff_data[i][diff_data_pos++] = -1;
		s_bmp_diff_data[i][diff_data_pos++] = -1;
	}

#ifdef FILE_TEST_LOG
	{
		int count = w*h*sizeof(int), i=0, num = 0;

		if(!(f=fopen("log_bmp_diff_data_24bit.txt", "w+")))
		{
			assert(0);
		}
	
		while(count>0)
		{
			if( s_bmp_diff_data[1][i] == -1 && s_bmp_diff_data[1][i+1] == -1 && 
			    s_bmp_diff_data[1][i+2] == -1)
			{
				fprintf(f, "\n*** the end***\n");
				break; 
			}

			fprintf(f, "x=%d\n", s_bmp_diff_data[1][i++]);
			fprintf(f, "y=%d\n", s_bmp_diff_data[1][i++]);
			num = s_bmp_diff_data[1][i++];
			fprintf(f, "num=%d\n", num);
			count = count - 3 - num;
			fprintf(f, "color_list:");
			while(num--)
			{
				fprintf(f, "%x ", s_bmp_diff_data[1][i++]);
			}
			fprintf(f, "\n\n");
		}
					
		if(f)
		{
			fclose(f);
			f = NULL;
		}
	}
#endif
}

int bmp_build_diff_bmp_data16(void)
{
	int frame_num = 0, i;
	int w = 0, h = 0;

	frame_num = bmp_get_frame_num();
	w = bmp_get_width_each_frame();
	h = bmp_get_height_each_frame();

	memset(s_bmp_diff_data16, 0, BMP_RAW_DATA_MAX_LEN*4);
	for(i=1; i<frame_num; i++)
	{
		int w_i = 0, h_i = 0;
		unsigned short *raw_data1 = NULL; // previous frame
		unsigned short *raw_data2 = NULL;   // current frame
		int diff_data_pos = 0;
		int trace_h_i = -1;

		assert(s_bmp_raw_data16[i-1]);
		assert(s_bmp_raw_data16[i]);
		raw_data1 = s_bmp_raw_data16[i-1];
		raw_data2 = s_bmp_raw_data16[i];
		
		s_bmp_diff_data16[i] = (int *)malloc(w*h*sizeof(short)*4);
		assert(s_bmp_diff_data16[i]);
		memset(s_bmp_diff_data16[i], 0, w*h*sizeof(short)*4);
		
		for(h_i=0; h_i<h; h_i++)
		{
			int trace_data_pos = 0;
			int trace_w_i = -1;
			for(w_i=0; w_i<w; w_i++)
			{
				if(raw_data1[w_i+h_i*w] != raw_data2[w_i+h_i*w])
				{
					if(w_i == 0)
					{
						s_bmp_diff_data16[i][diff_data_pos++] = w_i;
						s_bmp_diff_data16[i][diff_data_pos++] = h_i;
						trace_data_pos = diff_data_pos++;
						s_bmp_diff_data16[i][diff_data_pos++] = raw_data2[w_i+h_i*w];
						s_bmp_diff_data16[i][trace_data_pos] = 1;
					}
					else if(trace_h_i == h_i && trace_w_i+1 == w_i)
					{
						s_bmp_diff_data16[i][diff_data_pos++] = raw_data2[w_i+h_i*w];
						s_bmp_diff_data16[i][trace_data_pos]++;
					}
					else
					{
						s_bmp_diff_data16[i][diff_data_pos++] = w_i;
						s_bmp_diff_data16[i][diff_data_pos++] = h_i;
						trace_data_pos = diff_data_pos++;
						s_bmp_diff_data16[i][diff_data_pos++] = raw_data2[w_i+h_i*w];
						s_bmp_diff_data16[i][trace_data_pos] = 1;
					}
					trace_w_i = w_i;
					trace_h_i = h_i;
				}
			}
		}
		s_bmp_diff_data16[i][diff_data_pos++] = -1;
		s_bmp_diff_data16[i][diff_data_pos++] = -1;
		s_bmp_diff_data16[i][diff_data_pos++] = -1;
	}

#ifdef FILE_TEST_LOG
	{
		int count = w*h*sizeof(int), i=0, num = 0;

		if(!(f=fopen("log_bmp_diff_data_16bit.txt", "w+")))
		{
			assert(0);
		}
	
		while(count>0)
		{
			if( s_bmp_diff_data16[1][i] == (unsigned short)-1 && 
				s_bmp_diff_data16[1][i+1] == (unsigned short)-1 && 
			    s_bmp_diff_data16[1][i+2] == (unsigned short)-1)
			{
				fprintf(f, "\n*** the end***\n");
				break; 
			}

			fprintf(f, "x=%d\n", s_bmp_diff_data16[1][i++]);
			fprintf(f, "y=%d\n", s_bmp_diff_data16[1][i++]);
			num = s_bmp_diff_data16[1][i++];
			fprintf(f, "num=%d\n", num);
			count = count - 3 - num;
			fprintf(f, "color_list:");
			while(num--)
			{
				fprintf(f, "%x ", s_bmp_diff_data16[1][i++]);
			}
			fprintf(f, "\n\n");
		}
					
		if(f)
		{
			fclose(f);
			f = NULL;
		}
	}
#endif
}

