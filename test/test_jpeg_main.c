#include <stdio.h>

char *a[] = 
{
	"test",
	".\\res\\res_jpeg\\testprog.jpg",
	".\\res\\res_jpeg\\testzzy.bmp",
	NULL
};

int jpeg_main_test()
{
	djpeg_main(3, a);
	draw_bmp(a[2]);
	return 0;
}