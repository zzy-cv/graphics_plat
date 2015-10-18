// #include "test.h"
#include "basedraw.h"
#include "dot_font.h"
#include "bmp.h"

static void base_test_draw(void)
{
    draw_line(0, 0, 600, 600, RGB(255, 0, 0));
    draw_arraw_line_with_angle(300, 300, 200, 100);
    DrawCircle(300, 300, 100, RGB(255, 0, 0));
}

static void test_chinese(void)
{
    show_chinese(300, 300, "²â");
    show_chinese(300 + 16 + 2, 300, "ÊÔ");	
}

static void test_bmp(char *bmp_name)
{
    draw_bmp(bmp_name);
}

int test_init(void)
{
	gif_main_init();
	return 0;
}

int test_end(void)
{
	gif_main_end();
	return 0;
}

void test_draw(void)
{
	static unsigned char  c = '9';
	
	clear_rgb(0x0, 0x0, 0x0);

#if 1
	test_zzf_main();
#endif

	// test jpeg
#if 0
	jpeg_main_test();
#endif

	// test gif
#if 0
	gif_main_play();
#endif

	// test draw chinese
#if 0
	show_chinese(30, 30, (unsigned char *)"Äã");
#endif

	// test draw number
#if 0
	tplat_draw_ascii_char_12x8(30, 30, c, RGB(0, 0, 0));
	test_ascii12();
#endif

	// test base handle
#if 0
	test_alpha();
#endif

	// tset zlib
#if 0
	test_zlib_main();
#endif

	// test png1, use zlib
	// test_png1_main();
	// test_png3_main();

	// freetype test
#if 0
	ttf_main();
#endif

#if 0 // untest
	//test_vert_mirror();
	//test_horz_mirror();	
	//test_bi_value();
	//test_corner_mirror();
	//test_dct_transform();
#endif
}
