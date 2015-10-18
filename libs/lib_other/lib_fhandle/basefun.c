#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "basefun.h"

// macro definition
#define  ESCAPE_STR         "###$"
#define  ESCAPE_STR_LEN  4

// fuction definition

// memory function
void *my_malloc(int len)
{
	void *ptr = NULL;
	if(len>0)
	{
		ptr = malloc(len);
	}

	if(ptr)
	{
		memset(ptr, 0, len);
	}
	return ptr;
}

void my_free(void *ptr)
{
	if(!ptr)
	{
		return;
	}

	free(ptr);
	ptr = NULL;
}

// open file function
FILE *rtxtfile(char *filename)
{
	FILE *fp = NULL;

	my_assert(filename);
	fp = fopen(filename, "r+");	
	if(fp)
	{
		return fp;
	}
	else
	{
		fprintf(stderr, "file open error: %s\n", filename);
		return NULL;
	}
}

FILE *wtxtfile(char *filename)
{
	FILE *fp = NULL;

	fp = fopen(filename, "w+");
	my_assert(filename);
	if(fp)
	{
		return fp;
	}
	else
	{
		fprintf(stderr, "file open error: %s\n", filename);
		return NULL;
	}
}

FILE *rbinfile(char *filename)
{
	FILE *fp = NULL;

	my_assert(filename);
	fp = fopen(filename, "rb+");
	if(fp)
	{
		return fp;
	}
	else
	{
		fprintf(stderr, "file open error: %s\n", filename);
		return NULL;
	}
}

FILE *wbinfile(char *filename)
{
	FILE *fp = fopen(filename, "wb+");

	my_assert(filename);
	fp = fopen(filename, "wb+");	
	if(fp)
	{
		return fp;
	}
	else
	{
		fprintf(stderr, "file open error: %s\n", filename);
		return NULL;
	}
}

// file handle function
int get_file_size(FILE *handle)
{
	int len = 0;

	my_assert(handle);
	fseek(handle, 0L, SEEK_END);
	len = ftell(handle);
	fseek(handle, 0L, SEEK_SET);	
	return len;
}

int file_cnt_to_buf(FILE *handle, char *buf, int len) 
{
	if(len<=0)
	{
		return -1;
	}

	my_assert(handle && buf);
	fread(buf, len, 1, handle);
	fseek(handle, 0L, SEEK_SET);
	return 0;	
}

int file_cnt_to_buf_by_name(unsigned char *name, unsigned char **buf, int *len_file)
{
	FILE *fp = rbinfile(name);
	int len = 0;
	
	if(NULL == fp)
	{
		return -1;
	}
	len = get_file_size(fp);
 	if(len<=0)
 	{
		return -1;
	}
	*buf = (unsigned char *)my_malloc(len+1);
	if(NULL == buf)
	{
		return -1;
	}
	fread(*buf, len, 1, fp);
	fseek(fp, 0L, SEEK_SET);
	*len_file = len;
	fclose(fp);
	
	return 0;
}

int buf_cnt_to_file_by_name(unsigned char *name, unsigned char *buf, int len)
{
	FILE *fp = NULL;
	
	my_assert(name&&buf);
	fp = wbinfile(name);
	if(NULL == fp || len <= 0)
	{
		return -1;
	}

	fwrite(buf, len, 1, fp);

	if(fp)
	{
		fclose(fp);
	}

	return 0;
}

// my string handle function
int line_strlen(const char* str) 
{ 
	int i=0; 

	my_assert(str);
	while((*str!='\n') && (*str++!='\0')) 
	{
		i++; 
	}
	if(*str=='\r')
	{
		i--;
	}
	if(i<0)
	{
		return 0;
	}
	return i; 
} 

char *str_get_next_line(char *buf)
{
	char *next_line_ptr = NULL;

	if(NULL == buf)
	{
		return NULL;
	}
	next_line_ptr = strchr(buf, '\n');
	if(NULL == next_line_ptr)
	{
		return NULL;
	}
	else
	{
		return (next_line_ptr+1);
	}
}

char *strstr_bin(char *buf, char *s_buf, int buf_len, int s_buf_len)
{
	int i;
	for(i=0; i<buf_len; i++)
	{
		if(buf[i] == s_buf[0])
		{
			if(!memcmp(&buf[i], &s_buf[0], s_buf_len))
			{
				break;
			}
		}
	}
	return &buf[i];
}

// string to int
//hex char to int
int hexchr2int(unsigned int c)
{
	if((c>='0') && (c<='9'))
	{
		return c - '0';
	}

	if(c>='a' && c<='z')
	{
		c &= ~0x20;
	}

	return c-'A'+10;
}

// skip blank
int str_skip_blank(unsigned char *buf)
{
	unsigned char *s = buf;
	unsigned char *e = buf;
	unsigned int cur_char = 0;

	my_assert(buf);
	cur_char = *e;
	while(cur_char)
	{
		if(cur_char == ' ')
		{
			e++;
		}
		else
		{
			*s++ = *e++;
		}
		cur_char = *e;
	}
	*s = 0;

	return 0;
}

// hex string to int
int hexstr2int(unsigned char *buf)
{
	unsigned int cur_char = 0;
	int ret = 0;
	
	str_skip_blank(buf);
	if(!strncmp(buf, "0x", 2))
	{
		buf += 2;	
	}
	cur_char = *buf;
	while(cur_char)
	{
		int cur = 0;
		cur = hexchr2int(cur_char);
		ret = (ret<<4) + cur;
		cur_char = *(++buf);
	}

	return ret;
}

int buf_cnt_to_file_by_fadress(unsigned char *src_fname, 
											unsigned char *dst_fname,
											int s_address,
											int len,
											int flag
                                        )
{
	unsigned char *buf = NULL, *buf_out = NULL;
	unsigned char arr_name[64];
	unsigned char *tmp;
	int len_file = 0;
	int len_out = 0;
	int i = 0;

	memset(arr_name, 0, 64);
	strcpy(arr_name, dst_fname);
	tmp = strchr(arr_name, '.');
	if(tmp)
	{
		*tmp = '_';
	}
	file_cnt_to_buf_by_name(src_fname, &buf, &len_file);
	if((s_address > len_file) || (s_address+len>len_file))
	{
		return -1;
	}

	buf_out = (char *)my_malloc(len_file*5+64);
	i += s_address;
	sprintf(buf_out, "int %s_len = 0x%08x;\r\n", arr_name, len);
	len_out += (16+strlen(arr_name)+8);
	strcat(buf_out, "unsigned char ");
	len_out += 14;
	
	strcat(buf_out, "*");
	len_out += 1;
	
	strcat(buf_out, arr_name);
	len_out += strlen(arr_name);
	
	strcat(buf_out, "=\r\n");
	len_out += 3;
	
	strcat(buf_out, "\t\"");
	len_out += 2;
	while(len-->0)
	{
		unsigned char tmp = buf[i];
		unsigned char tmp_buf[8];

		memset(tmp_buf, 0, 8);
		printf("%x ", tmp);
		sprintf(tmp_buf, "\\x%02x", tmp);
		len_out += 4;
		strcat(buf_out, tmp_buf);
		if(len&&i&&((i+1)%16==0))
		{
			strcat(buf_out, "\" \\\r\n\t\"");
			len_out += 7;
		}		
		i++;
	}
	if((i-1)%16 != 0)
	{
		strcat(buf_out, "\"");
		len_out += 1;
	}
	
	strcat(buf_out, ";");
	len_out += 1;
	//printf("\nlen_out=%d, buf_out:%d\n", len_out, strlen(buf_out));

	buf_cnt_to_file_by_name(dst_fname, buf_out, len_out);
	my_free(buf);
	my_free(buf_out);		
}

int buf_cnt_to_file_ex(unsigned char *buf, 
					   int len_file,
					   unsigned char *dst_fname,
					   int s_address,
					   int len,
					   int flag)
{
	unsigned char /*buf = NULL, */*buf_out = NULL;
	unsigned char arr_name[64];
	unsigned char *tmp;
//	int len_file = 0;
	int len_out = 0;
	int i = 0;

	memset(arr_name, 0, 64);
	strcpy(arr_name, dst_fname);
	tmp = strchr(arr_name, '.');
	if(tmp)
	{
		*tmp = '_';
	}
	//file_cnt_to_buf_by_name(src_fname, &buf, &len_file);
	if((s_address > len_file) || (s_address+len>len_file))
	{
		return -1;
	}

	buf_out = (char *)my_malloc(len_file*5+64);
	i += s_address;
	sprintf(buf_out, "int %s_len = 0x%08x;\r\n", arr_name, len);
	len_out += (16+strlen(arr_name)+8);
	strcat(buf_out, "unsigned char ");
	len_out += 14;
	
	strcat(buf_out, "*");
	len_out += 1;
	
	strcat(buf_out, arr_name);
	len_out += strlen(arr_name);
	
	strcat(buf_out, "=\r\n");
	len_out += 3;
	
	strcat(buf_out, "\t\"");
	len_out += 2;
	while(len-->0)
	{
		unsigned char tmp = buf[i];
		unsigned char tmp_buf[8];

		memset(tmp_buf, 0, 8);
		printf("%x ", tmp);
		sprintf(tmp_buf, "\\x%02x", tmp);
		len_out += 4;
		strcat(buf_out, tmp_buf);
		if(len&&i&&((i+1)%16==0))
		{
			strcat(buf_out, "\" \\\r\n\t\"");
			len_out += 7;
		}		
		i++;
	}
	if((i-1)%16 != 0)
	{
		strcat(buf_out, "\"");
		len_out += 1;
	}
	
	strcat(buf_out, ";");
	len_out += 1;
	//printf("\nlen_out=%d, buf_out:%d\n", len_out, strlen(buf_out));

	buf_cnt_to_file_by_name(dst_fname, buf_out, len_out);
	//my_free(buf);
	my_free(buf_out);		
}

