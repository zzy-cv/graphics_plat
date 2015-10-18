/* png 测试数据
   8bit 的 8X8 png 数据

   // 解压后的数据: filter type + 数据
    0     64      2      3     61     60      6      7     57
    1      9     46    255    214      1     38    255    222
    4      8    248    255      8      1    248    255      8
    1     40    242      1     10    255    250      1      2
    4    248      8      1    248    255     10      1    242
    4      9    238    255     22      1    230    255     30
    4      8    248    255      8      1    248    255      8
    1      8     50      1    202    255     58      1    194
    
   // filter前的数据: 
   64    2    3   61   60    6    7   57
    9   55   54   12   13   51   50   16
   17   47   46   20   21   43   42   24
   40   26   27   37   36   30   31   33
   32   34   35   29   28   38   39   25
   41   23   22   44   45   19   18   48
   49   15   14   52   53   11   10   56
    8   58   59    5    4   62   63    1
*/

// png测试数据长度
int c_data_col = 5;
int c_data_row = 16;
int c_data_len = 0x00000050;

// 解压后的数据: filter type + 数据
unsigned char *c_data=
	"\x01\xf5\x3f\x72\x15" \
	"\x03\x01\xd1\x54\xe9" \
	"\x00\xce\xa1\x57\xed" \
	"\x00\xa2\x00\xdf\xd2" \
	"\x01\x4d\x07\x03\x1f" \
	"\x02\x5a\xf1\xb8\x29" \
	"\x00\x5a\x0b\xb5\x0e" \
	"\x00\xed\x17\xf2\x89" \
	"\x03\x05\x1e\x7b\x1b" \
	"\x00\x17\xce\x69\xda" \
	"\x00\xaa\xdc\x17\xd8" \
	"\x02\xd8\x29\x5a\x9f" \
	"\x04\xb6\xb2\x24\xec" \
	"\x03\x9b\x1f\x8b\x10" \
	"\x00\x11\xb8\x3f\x00" \
	"\x04\xe2\xfe\x1a\x21";

// filter前的数据:
/*
   245    52   166   187
   123    40   187   164
   206   161    87   237
   162     0   223   210
    77    84    87   118
   167    69    15   159
    90    11   181    14
   237    23   242   137
   123   103    39   115
    23   206   105   218
   170   220    23   216
   130     5   113   119
    56   183   219   199
   183   214    99   165
    17   184    63     0
   243   241    89    33
*/

unsigned char row_test[] = {1, 9, 46, 255, 214, 1, 38, 255, 222};
unsigned char row_test1[] = {1, 40, 242, 1, 10, 255, 250, 1, 2};
unsigned char row_test2[] = {1, 8, 50, 1, 202, 255, 58, 1, 194};

void png_invert_filter_sub(unsigned char *row, unsigned int rowbytes, unsigned int pixel_depth)
{
	unsigned int i;
	unsigned int istop = rowbytes;
	unsigned int bpp = (pixel_depth + 7) >> 3;
	unsigned char * rp = row + bpp;
	unsigned char * lp = row;

	for (i = bpp; i < istop; i++)
	{
		*rp = (unsigned char)(((int)(*rp) + (int)(*lp++)) & 0xff);
		rp++;
	}
}
void png_invert_filter_up(unsigned char *pre_row, unsigned char *row, unsigned int rowbytes)
{
     unsigned int i;
     unsigned int istop = rowbytes;
     unsigned char *rp = row;
     unsigned char *pp = pre_row;

     for (i = 0; i < istop; i++)
     {
        *rp = (unsigned char)(((int)(*rp) + (int)(*pp++)) & 0xff);
        rp++;
     }
}

void png_invert_filter_avg(unsigned char *pre_row, unsigned char *row, unsigned int rowbytes, unsigned int pixel_depth)
{
     unsigned int i;
     unsigned char *rp = row;
     unsigned char *pp = pre_row;
     unsigned char *lp = row;
     unsigned int bpp = (pixel_depth + 7) >> 3;
     unsigned int istop = rowbytes - bpp;

     for (i = 0; i < bpp; i++)
     {
        *rp = (unsigned char)(((int)(*rp) + ((int)(*pp++) / 2 )) & 0xff);
        rp++;
     }

     for (i = 0; i < istop; i++)
     {
        *rp = (unsigned char)(((int)(*rp) + (int)(*pp++ + *lp++) / 2 ) & 0xff);
        rp++;
     }
}

void png_invert_filter_paeth(unsigned char *pre_row, unsigned char *row, unsigned int rowbytes, unsigned int pixel_depth)
{
     unsigned int i;
     unsigned char *rp = row;
     unsigned char *pp = pre_row;
     unsigned char *lp = row;
     unsigned char *cp = pre_row;
     unsigned int bpp = (pixel_depth + 7) >> 3;
     unsigned int istop=rowbytes - bpp;

     for (i = 0; i < bpp; i++)
     {
        *rp = (unsigned char)(((int)(*rp) + (int)(*pp++)) & 0xff);
        rp++;
     }

     for (i = 0; i < istop; i++)   /* use leftover rp,pp */
     {
        int a, b, c, pa, pb, pc, p;

        a = *lp++;
        b = *pp++;
        c = *cp++;

        p = b - c;
        pc = a - c;

#ifdef PNG_USE_ABS
        pa = abs(p);
        pb = abs(pc);
        pc = abs(p + pc);
#else
        pa = p < 0 ? -p : p;
        pb = pc < 0 ? -pc : pc;
        pc = (p + pc) < 0 ? -(p + pc) : p + pc;
#endif

        /*
           if (pa <= pb && pa <= pc)
              p = a;
           else if (pb <= pc)
              p = b;
           else
              p = c;
         */

        p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

        *rp = (unsigned char)(((int)(*rp) + p) & 0xff);
        rp++;
     }
}

void png_invert_filter_sub(unsigned char *row, unsigned int rowbytes, unsigned int pixel_depth);
void png_invert_filter_up(unsigned char *pre_row, unsigned char *row, unsigned int rowbytes);
void png_invert_filter_avg(unsigned char *pre_row, unsigned char *row, unsigned int rowbytes, unsigned int pixel_depth);
void png_invert_filter_paeth(unsigned char *pre_row, unsigned char *row, unsigned int rowbytes, unsigned int pixel_depth);

int png_filter_main()
{
	int i,j;
	unsigned char buf[256];

	for(i=0; i<c_data_len; i++)
	{
		buf[i] = c_data[i];
	}

	for(i=0; i<c_data_row; i++)
	{
		switch(c_data[i*c_data_col])
		{
			case 0:
				break;
				
			case 1:
				png_invert_filter_sub(&buf[i*c_data_col+1], 4, 8);
				break;
				
			case 2:
				png_invert_filter_up(&buf[(i-1)*c_data_col+1], &buf[i*c_data_col+1], 4);
				break;
				
			case 3:
				png_invert_filter_avg(&buf[(i-1)*c_data_col+1], &buf[i*c_data_col+1], 4, 8);
				break;
				
			case 4:
				png_invert_filter_paeth(&buf[(i-1)*c_data_col+1], &buf[i*c_data_col+1], 4, 8);
				break;
			default:
				break;
		}
	}

	for(i=0; i<c_data_row; i++)
	{
		for(j=0; j< c_data_col; j++)
		{
			printf("%d ", buf[i*c_data_col+j]);
		}
		printf("\n");
	}
	return 0;
}

