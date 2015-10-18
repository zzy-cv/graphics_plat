#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <main.h>
/*********************************************************************************

	macro definition
*********************************************************************************/
#define COLOR_TO_RGB32(bgra, a, r, g, b) \
	do{\
	a = ((bgra) >> 24) & 255; \
	r = ((bgra) >> 16) & 255; \
	g = ((bgra) >> 8) & 255; \
	b = (bgra) & 255; \
	}while(0)
	
#define RGB888_TO_COLOR(a, r, g, b)  \
	((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define CLR_RGB888_TO565(clr) (((((clr)>>19)&0x1f)<<11)|\
                                    ((((clr)>>10)&0x3f)<<5)|\
                                    (((clr)>>3)&0x1f))

#define CLR_RGB565_TO888(clr)	  (((((clr)>>11)&31)<<16)|\
                                        ((((clr)>>5)&63)<<8)|\
                                        ((clr)&31))	

/*********************************************************************************

	structure type definition

*********************************************************************************/

// 复数定义
typedef struct complex_t_tag
{
	double re;
	double im;
}complex_t;

// base function

/*复数加运算*/
complex_t complex_add(complex_t c1, complex_t c2)
{
	complex_t c;
	c.re=c1.re+c2.re;
	c.im=c1.im+c2.im;
	return c;
}

/*复数减运算*/
complex_t complex_sub(complex_t c1, complex_t c2)
{
	complex_t c;
	c.re=c1.re-c2.re;
	c.im=c1.im-c2.im;
	return c;
}

/*复数乘运算*/
complex_t complex_mul(complex_t c1, complex_t c2)
{
	complex_t c;
	c.re=c1.re*c2.re-c1.im*c2.im;
	c.im=c1.re*c2.im+c2.re*c1.im;
	return c;
}


/*********************************************************************************

	垂直镜像变换，变换后图像宽高不变

*********************************************************************************/

int vert_mirror(unsigned long *dst, unsigned long *src, int src_w, int src_h)
{
	int x,y;	

	assert(dst&&src);
	for(y=0;y<src_h;y++)
	{
		for(x=0;x<src_w;x++)
		{
			dst[x+y*src_w]=src[x+(src_h-y-1)*src_w];
		}
	}
	return 0;
}

/*********************************************************************************

	水平镜像变换,变换后图像宽高不变

*********************************************************************************/

int horz_mirror(unsigned long *dst, unsigned long *src, int src_w, int src_h)
{
	int x,y;

	for(y=0;y<src_h;y++)
	{
		for(x=0;x<src_w;x++)
		{
			dst[x+y*src_w]=src[(src_w-1-x)+y*src_w];
			//dst[x*3+1+y*src_w]=src[(src_w-1-x)*3+1+y*src_w];
			//dst[x*3+2+y*src_w]=src[(src_w-1-x)*3+2+y*src_w];
		}
	}
	return 0;
}

/*********************************************************************************

	对角镜像处理，变换后图像宽高不变

*********************************************************************************/

int corner_mirror(unsigned long *dst, unsigned long *src, int src_w, int src_h)
{
	int x,y;

	for(y=0;y<src_h;y++)
	{
		for(x=0;x<src_w;x++)
		{
			dst[x+y*src_w]=src[(src_w-1-x)+(src_h-y-1)*src_w];
		}
	}

	return 0;
}

/*********************************************************************************

	二值化处理， 变换后图像宽高不变

*********************************************************************************/

/*
buff=((*(p+i)+j)->r*R+(*(p+i)+j)->g*G+(*(p+i)+j)->b*B)/100;
buff=(buff>=t?ONE:ZERO)
将各像素的R,G,B分量乘以各自的权重,R的权重是0.3,G的权重是0.59,B的权重是0.11,
将三个积相加,看结果和你事先输入的阀值t进行比较

Gray=0.3R+0.59G+0.11B,是RGB转灰度值是基本公式.
至于为什么是这个公式,就涉及到显示设备的显示原理了,这就不是我们程序员关心的问题了.
简单来说,当图像摄入设备摄入的白光能产生1个单位电压时,
如果分别滤掉蓝绿,红蓝,红绿光的话,剩下的红,绿,蓝光被设备感觉到的电压分别是0.3,0.59和0.11个单位.
这就是这个公式的由来.
*/
static unsigned long convert_by_bi_value(unsigned long color, unsigned char threshold)
{
	unsigned char gray, a, r, g, b;

	COLOR_TO_RGB32(color, a, r, g, b);
	gray = 0.3*r + 0.59*g + 0.11*b;

	return (gray>=threshold)?0xffffff:0x0;
}

int bi_value(unsigned long *buf, int w, int h, unsigned char threshold)
{
	unsigned char *byte = (unsigned char *)buf;
	int x,y;
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			buf[x+y*w] = convert_by_bi_value(buf[x+y*w], threshold);
		}
	}
	return 0;
}

/*********************************************************************************

	离散余弦变换

*********************************************************************************/

#define DCT_PI 3.1415926535
void FFT(complex_t * TD, complex_t * FD, int power)
{
	int count;
	int i,j,k,bfsize,p;
	double angle;
	complex_t *W,*X1,*X2,*X;

	count=1<<power;
	W=(complex_t *)malloc(sizeof(complex_t)*count/2);
	X1=(complex_t *)malloc(sizeof(complex_t)*count);
	X2=(complex_t *)malloc(sizeof(complex_t)*count);
	for(i=0;i<count/2;i++)
	{
		angle=-i*DCT_PI*2/count;
		W[i].re=cos(angle);
		W[i].im=sin(angle);
	}
	memcpy(X1,TD,sizeof(complex_t)*count);
	for(k=0;k<power;k++)
	{
		for(j=0;j<1<<k;j++)
		{
			bfsize=1<<(power-k);
			for(i=0;i<bfsize/2;i++)
			{
				p=j*bfsize;
				X2[i+p]=complex_add(X1[i+p],X1[i+p+bfsize/2]);
				X2[i+p+bfsize/2]=complex_mul(complex_sub(X1[i+p],X1[i+p+bfsize/2]),W[i*(1<<k)]);
			}
		}
		X=X1;
		X1=X2;
		X2=X;
	}
	for(j=0;j<count;j++)
	{
		p=0;
		for(i=0;i<power;i++)
		{
			if (j&(1<<i)) p+=1<<(power-i-1);
		}
		FD[j]=X1[p];
	}
	free(W);
	free(X1);
	free(X2);
}

void DCT_Pre(double *f, double *F, int power)
{
	int i,count;
	complex_t *X;
	double s;

	count=1<<power;
	X=(complex_t *)malloc(sizeof(complex_t)*count*2);
	memset(X,0,sizeof(complex_t)*count*2);
	for(i=0;i<count;i++)
	{
		X[i].re=f[i];
	}
	FFT(X,X,power+1);
	s=1/sqrt(count);
	F[0]=X[0].re*s;
	s*=sqrt(2);
	for(i=1;i<count;i++)
	{
		F[i]=(X[i].re*cos(i*DCT_PI/(count*2))+X[i].im*sin(i*DCT_PI/(count*2)))*s;
	}
	free(X);
}

int dct_transform(unsigned long *buf, int width, int height)
{
	int w=1,h=1,wp=0,hp=0;
	double a;
	int x,y;
	double *f = NULL, *W = NULL;

	while(w*2<=width)
	{
		w*=2;
		wp++;
	}
	while(h*2<=height)
	{
		h*=2;
		hp++;
	}

	f = (double *)malloc(w*h*sizeof(double));
	W = (double *)malloc(w*h*sizeof(double));

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			f[x+y*w]=buf[(x)+(y)*width];
		}
	}

	for(y=0;y<h;y++)
	{
		DCT_Pre(&f[w*y],&W[w*y],wp);
	}
	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			f[x*h+y]=W[x+w*y];
		}
	}
	for(x=0;x<w;x++)
	{
		DCT_Pre(&f[x*h],&W[x*h],hp);
	}
	memset(buf,0,width*height);

	for(y=0;y<h;y++)
	{
		for(x=0;x<w;x++)
		{
			a=fabs(W[x*h+y]);
			if (a>255) a=255;
			{
				buf[(x)+(height-y-1)*width]=(unsigned char)(a);
			}
		}
	}

	free(f);
	free(W);
	return 0;
}

/*********************************************************************************

	透明效果

*********************************************************************************/

#define ALPHA1_ROUND(clr) (((clr) & 0x8610)>>3)
#define ALPHA1(clr)    (((clr) & 0x8610)>>4)
#define ALPHA2(clr)    (((clr) & 0xC718)>>3)
#define ALPHA4(clr)    (((clr) & 0xE79C)>>2)
#define ALPHA8(clr)    (((clr) & 0xF7DE)>>1)

#define RANK16_ALPHATONE00(dst, src) (dst)

#define RANK16_ALPHATONE01(dst, src) (ALPHA1_ROUND(src)+\
                                      (dst)-\
                                      ALPHA1_ROUND(dst))

#define RANK16_ALPHATONE02(dst, src) (ALPHA2(src)+\
                                      ALPHA1(src)+\
                                      (dst)-\
									  ALPHA2(dst)-\
									  ALPHA1(dst))

#define RANK16_ALPHATONE03(dst, src) (ALPHA1_ROUND(src)+\
                                      ALPHA2(src)+\
									  (dst)-\
									  ALPHA1_ROUND(dst)-\
									  ALPHA2(dst))

#define RANK16_ALPHATONE04(dst, src) (ALPHA4(src)+\
                                      ALPHA1(src)+\
									  dst-\
									  ALPHA4(dst)-\
									  ALPHA1(dst))

#define RANK16_ALPHATONE05(dst, src) (ALPHA1_ROUND(src)+\
                                      ALPHA4(src)+\
									  dst-\
									  ALPHA1_ROUND(dst)-\
									  ALPHA4(dst))

#define RANK16_ALPHATONE06(dst, src) (ALPHA2(src)+\
                                      ALPHA4(src)+\
									  ALPHA1(src)+\
									  (dst)-\
									  ALPHA2(dst)-\
									  ALPHA4(dst)-\
									  ALPHA1(dst))

#define RANK16_ALPHATONE07(dst, src) (ALPHA1_ROUND(src)+\
                                      ALPHA2(src)+\
									  ALPHA4(src)+\
									  (dst)-\
									  ALPHA1_ROUND(dst)-\
									  ALPHA2(dst)-\
									  ALPHA4(dst))

#define RANK16_ALPHATONE08(dst, src) (ALPHA8(src)+\
                                      ALPHA8(dst))

#define RANK16_ALPHATONE09(dst, src) ((src)-\
									  ALPHA1_ROUND(src)-\
									  ALPHA2(src)-\
									  ALPHA4(src)+\
									  ALPHA1_ROUND(dst)+\
									  ALPHA2(dst)+\
									  ALPHA4(dst))

#define RANK16_ALPHATONE10(dst, src) ((src)-\
									  ALPHA2(src)-\
									  ALPHA4(src)-\
									  ALPHA1(src)+\
									  ALPHA2(dst)+\
									  ALPHA4(dst)+\
									  ALPHA1(dst))

#define RANK16_ALPHATONE11(dst, src) ((src)-\
									  ALPHA1_ROUND(src)-\
									  ALPHA4(src)+\
									  ALPHA1_ROUND(dst)+\
									  ALPHA4(dst))

#define RANK16_ALPHATONE12(dst, src) ((src)-\
									  ALPHA4(src)-\
									  ALPHA1(src)+\
									  ALPHA4(dst)+\
									  ALPHA1(dst))

#define RANK16_ALPHATONE13(dst, src) ((src)-\
									  ALPHA1_ROUND(src) -\
									  ALPHA2(src)+\
									  ALPHA1_ROUND(dst)+\
									  ALPHA2(dst))

#define RANK16_ALPHATONE14(dst, src) ((src)-\
									  ALPHA2(src)-\
									  ALPHA1(src)+\
									  ALPHA2(dst)+\
									  ALPHA1(dst))

#define RANK16_ALPHATONE15(dst, src) ((src)-\
									  ALPHA1_ROUND(src)+\
									  ALPHA1_ROUND(dst))

#define RANK16_ALPHATONE16(dst, src) (src)

unsigned short rank16_make_alpha_blender(unsigned short dst, unsigned short src, int alpha)
{
	switch(alpha)
	{
	case 0:
		return RANK16_ALPHATONE00(dst, src);
		
	case 1:
		return RANK16_ALPHATONE01(dst, src);

	case 2:
		return RANK16_ALPHATONE02(dst, src);

	case 3:
		return RANK16_ALPHATONE03(dst, src);

	case 4:
		return RANK16_ALPHATONE04(dst, src);

	case 5:
		return RANK16_ALPHATONE05(dst, src);

	case 6:
		return RANK16_ALPHATONE06(dst, src);

	case 7:
		return RANK16_ALPHATONE07(dst, src);

	case 8:
		return RANK16_ALPHATONE08(dst, src);

	case 9:
		return RANK16_ALPHATONE09(dst, src);

	case 10:
		return RANK16_ALPHATONE10(dst, src);

	case 11:
		return RANK16_ALPHATONE11(dst, src);

	case 12:
		return RANK16_ALPHATONE12(dst, src);

	case 13:
		return RANK16_ALPHATONE13(dst, src);

	case 14:
		return RANK16_ALPHATONE14(dst, src);

	case 15:
		return RANK16_ALPHATONE15(dst, src);

	case 16:
		return RANK16_ALPHATONE16(dst, src);
	default:
		return dst;
	}
}

/* 
	convert bmp rawdata's depth to 24bits
*/
int tplat_format_rawdata(bmp_rawdata_t *rawdata)
{
    int i=0, j=0;
    
	switch(rawdata->depth)
	{
	    case 1:
            // todo
            return -1;
            break;
        case 4:
            // todo
            return -1;
            break;
        case 8:
            // todo
            return -1;
            break;
        case 24:
            return 0;
            break;
		default: // depth error
            return -1;
            break;
	}
	return 0;
}

bmp_rawdata_t *tplat_blender_two_rawdata_by_alpha256(
	bmp_rawdata_t *rawdata1, bmp_rawdata_t *rawdata2, int alpha)
{
	int ret = 0;
	bmp_rawdata_t *tmp = NULL;
    int res_w = 0, res_h = 0;
    bmp_rawdata_t *res_rawdata = NULL;
    int i = 0, j = 0;
    unsigned int rgb1 = 0, rgb2 = 0, rgb_res = 0;
    unsigned char rgb1_a = 0, rgb1_r = 0, rgb1_g = 0, rgb1_b = 0;
    unsigned char rgb2_a = 0, rgb2_r = 0, rgb2_g = 0, rgb2_b = 0;
    unsigned char rgb_res_a = 0, rgb_res_r = 0, rgb_res_g = 0, rgb_res_b =0;
	
	if(NULL == rawdata1 || NULL == rawdata2
		|| NULL == rawdata1->rawdata || NULL == rawdata2->rawdata)
	{
		return NULL;
	}

	ret = tplat_format_rawdata(rawdata1);
	if(0 != ret)
	{
		return NULL;
	}

	ret = tplat_format_rawdata(rawdata2);
	if(0 != ret)
	{
		return NULL;
	}

    res_w = rawdata1->width;
    if(res_w < rawdata2->width)
    {
        res_w = rawdata2->width;
    }
	res_h = rawdata1->height;
    if(res_h < rawdata2->height)
    {
        res_h = rawdata2->height;
    }

    if(0 == res_w || 0 == res_h)
    {
        return NULL;
    }

    res_rawdata = (bmp_rawdata_t *)malloc(sizeof(bmp_rawdata_t));
    if(NULL == res_rawdata)
    {
        return NULL;
    }

    res_rawdata->width = res_w;
    res_rawdata->height = res_h;
    res_rawdata->depth = 24;
    res_rawdata->rawdata = (unsigned int *)malloc(sizeof(unsigned int)*res_w*res_h);

    for(j = 0; j < res_h; j++)
    {
        for(i = 0; i < res_w; i++)
        {
            // check rawdata1
            if(rawdata1->width <= i || rawdata1->height <= j) // get rawdata2's data
            {
                res_rawdata->rawdata[i+j*res_w] = rawdata2->rawdata[i+j*rawdata2->width];
            }
            else if(rawdata2->width <= i || rawdata2->height <= j) // get rawdata1's data
            {
                res_rawdata->rawdata[i+j*res_w] = rawdata1->rawdata[i+j*rawdata1->width];
            }
            else // alpha rawdata1 & rawdata2
            {
                rgb1 = rawdata1->rawdata[i+j*rawdata1->width]; // get rawdat1's rgb data
                rgb2 = rawdata2->rawdata[i+j*rawdata2->width]; // get rawdata2's rgb data
                COLOR_TO_RGB32(rgb1, rgb1_a, rgb1_r, rgb1_g, rgb1_b);
                COLOR_TO_RGB32(rgb2, rgb2_a, rgb2_r, rgb2_g, rgb2_b);
                rgb_res_a = (rgb1_a*alpha+rgb2_a*(256-alpha))/256;
                rgb_res_r = (rgb1_r*alpha+rgb2_r*(256-alpha))/256;
                rgb_res_g = (rgb1_g*alpha+rgb2_g*(256-alpha))/256;
                rgb_res_b = (rgb1_b*alpha+rgb2_b*(256-alpha))/256;
                rgb_res = RGB888_TO_COLOR(rgb_res_a, rgb_res_r, rgb_res_g, rgb_res_b);
                res_rawdata->rawdata[i+j*res_w] = rgb_res;
            }
        }
    }

    return res_rawdata;
}

void blender_two_buf_by_alpha(unsigned long *dst, unsigned long *src,
	int dst_w, int src_w, int src_h, int alph)
{
	unsigned int rgb1, rgb2, rgb3;
	unsigned short srgb1, srgb2, srgb3;
	int line_pix_num1 = 0, line_pix_num2 = 0;
	int i, j;
	
	for(j = 0; j < src_h; j++)
	{
		for(i = 0; i < src_w; i++)
		{
			rgb1 = dst[i+line_pix_num1];
			rgb2 = src[i+line_pix_num2];
			srgb1 = CLR_RGB888_TO565(rgb1);
			srgb2 = CLR_RGB888_TO565(rgb2);
			srgb3 = rank16_make_alpha_blender(srgb1, srgb2, alph);

			rgb3 = CLR_RGB565_TO888(srgb3);
			dst[i+line_pix_num1] = rgb3;
		}
		line_pix_num1 += dst_w;
		line_pix_num2 += src_w;
	}
}

/*********************************************************************************
	测试函数
*********************************************************************************/

void rgb_macro_test(void)
{
	unsigned long bgra = 0x12345678;
	unsigned char b, g, r, a;
	unsigned char str[100];
	
	COLOR_TO_RGB32(bgra, a, r, g, b);

	open_trace_file();
	sprintf(str, "bgra:0x%x\n", bgra);		
	write_trace(str, -1);
	sprintf(str, "a:0x%x r:0x%x g:0x%x b:0x%x\n", a, r, g, b);
	write_trace(str, -1);
	close_trace_file();
}
