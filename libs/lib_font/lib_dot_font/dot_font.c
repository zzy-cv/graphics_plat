#include <stdio.h>
#include "dot_font.h"
#include "basedraw.h"

void show_chinese(int start_x, int start_y, unsigned char *buf)
{
    unsigned char matrix[16][2];
    FILE *f_hzk;
    int i, j, k;
    if((f_hzk = fopen(".\\res\\res_font\\hzk16","rb"))==NULL)
    {
	      printf("\nOpoen file: hzk16 error!\n");
	      exit(0);
    }

    i = buf[0]-0xa0;
    j = buf[1]-0xa0;                                 /*获得区码与位码*/
    fseek(f_hzk, (94*(i-1)+(j-1))*32l, SEEK_SET);
    fread(matrix, 32, 1, f_hzk);

    for(j = 0; j < 16; j++)
        for(i = 0; i < 2; i++)
            for(k = 0; k < 8; k++)
		if(matrix[j][i]&(0x80>>k))                        /*测试为1的位则显示*/
		    putpixel(i*8+k + start_x, j + start_y, RGB(255, 0, 0));
    fclose(f_hzk);
}

static int tplat_draw_ascii_char_12x8_inner(int x, int y, unsigned char *p, unsigned long color)
{
	int i, j;
	for(j=0; j<12; j++)
	{
		unsigned char cur_byte = p[j];
		for(i=0; i<8; i++)
		{
			//unsigned char cur_byte = p[i];
			if((cur_byte>>i)&0x1)
			{
				putpixel(7-i+x, j+y, color);
			}
		}
	}

	return 0;
}

int tplat_draw_ascii_char_12x8(int x, int y, unsigned char ch, unsigned long color)
{
	int ch_index;
	FILE *file;
	static char font_buf[12];
	
	if(ch <= 32 || ch >=127)
	{
		return -1;
	}
	file = fopen(".\\res\\res_font\\ASC12", "rb");
	if(file)
	{
		;
	}
	else
	{
		;
		return -1;
	}

	ch_index = ch-32;
	fseek(file, ch_index*12, SEEK_SET);
	fread(font_buf, 12, 1, file);
	tplat_draw_ascii_char_12x8_inner(x, y, font_buf, RGB(255, 0, 0));
	
	fclose(file);
	return 0;
}

void test_ascii12(void)
{
	unsigned char c;
	int x=50, y=100;
	
	for(c='0'; c<='9'; c++)
	{
		tplat_draw_ascii_char_12x8(x, y, c, RGB(255, 255, 0));
		x+=16;
		if(x>300)
		{
			x=50;
			y+=16;
		}
	}
}

