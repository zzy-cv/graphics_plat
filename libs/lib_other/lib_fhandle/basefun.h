#ifndef __BASEFUN_H__
#define __BASEFUN_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// macro definition
#define  my_assert assert

// memory function
void *my_malloc(int len);
void my_free(void *ptr);

// open file function
FILE *rtxtfile(char *filename);
FILE *wtxtfile(char *filename);
FILE *rbinfile(char *filename);
FILE *wbinfile(char *filename);

// file handle function
int get_file_size(FILE *handle);
int file_cnt_to_buf(FILE *handle, char *buf, int file_len); 
int file_cnt_to_buf_by_name(unsigned char *name, unsigned char **buf, int *len_file);

// my string handle function
int line_strlen(const char* str);
char *str_get_next_line(char *buf);
char *strstr_bin(char *buf, char *s_buf, int buf_len, int s_buf_len);

#endif

