#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

#define TTF_PI 3.1415926535
#define FT2_SRC_COLOR_KEY  0x1000

/*

字体属性(可以提供接口给用户设置):

1. 字的颜色        : 默认位白色字
2. 是否反走样效果  : 默认不用反走样的效果，因为反走样效果需要耗空间
3. 旋转            : 默认为不旋转，用户可以设置旋转角度，范围是0到360度
4. 是否反向显示    : 默认为不反向显示
5. 是否垂直显示    : 默认为不垂直显示

*/

void putpixel(int x, int y, unsigned long color);
int ft2_draw_pixmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y)
{
	int i, j;
	unsigned char *buf = bitmap->buffer;
	int w = bitmap->width, h = bitmap->rows;
	
	for(i=0; i<h; i++)
	{
		for(j=0; j<w; j++)
		{
			unsigned char c = buf[i*w+j];
			unsigned long rgb = 0;
             
			/* gray to rgb 快速算法          */
			/* rgb = grey * 0x00010101 (888) *
			/*      
			/*********************************/
#if 0  // 有锯齿
			rgb = c == 0? FT2_SRC_COLOR_KEY: c < 128 ? FT2_SRC_COLOR_KEY: 0xffffff;
#else  // 无锯齿
			rgb = c == 0? FT2_SRC_COLOR_KEY: c * 0x00010101;
			if(0 == c)
			{
				rgb = FT2_SRC_COLOR_KEY;
			}
			else
			{
				rgb = (c<<16) + (c << 8) + c;
			}

			rgb = 0xffffff & rgb;  // 画反走样效果的彩色字
#endif
			if(FT2_SRC_COLOR_KEY != rgb)
			{
				putpixel(j+x, i+y, rgb);
			}
		}
	}
	return 0;
}

int ft2_draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y)
{
	int i, j, k;
	unsigned char *buf = bitmap->buffer;
	int w = bitmap->pitch;  //pitch can be either positive or negative
	int h = bitmap->rows;
	
	w = w>0?w:-w;
	for(i=0; i<h; i++)
	{
		for(j=0; j<w; j++)
		{
			unsigned char c = buf[i*w+j];
			for(k=0; k<8; k++)
			{
				unsigned int bit = c&(1<<(7-k));  // 7-k
				if(bit)
				{
					putpixel(x+j*8+k, y+i, 0xff);
				}
				else
				{
					//putpixel(j*8+k, i, 0xff0000);
				}
			}
		}
	}
	return 0;
}

void draw_string(FT_Face face, unsigned short *string, int num_chars)
{
	FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
	FT_Matrix matrix;
	int n;
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Error error;
	int target_height = 500;

	pen.x = 200*64;
	pen.y = (target_height-200)*64;

	matrix.xx = (FT_Fixed)( cos( 0 ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( 0 ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( 0 ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( 0 ) * 0x10000L );

		
	for ( n = 0; n < num_chars; n++ )
	{
		/* set transformation */
		FT_Set_Transform( face, &matrix, &pen );

		/* load glyph image into the slot (erase previous one) */

		error = FT_Load_Char( face, string[n], 
		        FT_LOAD_RENDER ); //| FT_LOAD_FORCE_AUTOHINT | FT_LOAD_MONOCHROME | FT_LOAD_VERTICAL_LAYOUT);
		ft2_draw_pixmap( &slot->bitmap, slot->bitmap_left, target_height - slot->bitmap_top );
#if 0
		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
#else
		/* decrement pen position */
		pen.x -= slot->advance.x;
		pen.y -= slot->advance.y;
#endif
	}
}

void kern(FT_Face face, unsigned short *text, int num_chars)
{
	FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
	FT_UInt 	  glyph_index;
	FT_Bool 	  use_kerning;
	FT_UInt 	  previous;
	int 		  pen_x, pen_y, n;

	FT_Error error;

	pen_x = 100;
	pen_y = 100;

	use_kerning = FT_HAS_KERNING( face );
	previous	= 0;

	for ( n = 0; n < num_chars; n++ )
	{
		/* convert character code to glyph index */
		glyph_index = FT_Get_Char_Index( face, text[n] );

		/* retrieve kerning distance and move pen position */
		if ( use_kerning && previous && glyph_index )
		{
			FT_Vector  delta;

			FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
			pen_x += delta.x >> 6;
		}

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );
		if ( error )
			continue;  /* ignore errors */

		/* now draw to our target surface */
		ft2_draw_pixmap( &slot->bitmap,pen_x + slot->bitmap_left,pen_y - slot->bitmap_top );

		/* increment pen position */
		pen_x += slot->advance.x >> 6;

		/* record current glyph index */
		previous = glyph_index;
	}
}

// center
static void  compute_string_bbox( FT_BBox	*abbox, FT_Glyph *glyphs, FT_Vector *pos, int num_glyphs)
{
	FT_BBox  bbox;
	int n;

	/* initialize string bbox to "empty" values */
	bbox.xMin = bbox.yMin =  320000;
	bbox.xMax = bbox.yMax = -320000;

	/* for each glyph image, compute its bounding box, */
	/* translate it, and grow the string bbox 		 */
	for ( n = 0; n < num_glyphs; n++ )
	{
		FT_BBox  glyph_bbox;

		FT_Glyph_Get_CBox( glyphs[n], ft_glyph_bbox_pixels,&glyph_bbox );

		glyph_bbox.xMin += pos[n].x;
		glyph_bbox.xMax += pos[n].x;
		glyph_bbox.yMin += pos[n].y;
		glyph_bbox.yMax += pos[n].y;

		if ( glyph_bbox.xMin < bbox.xMin )
			bbox.xMin = glyph_bbox.xMin;

		if ( glyph_bbox.yMin < bbox.yMin )
			bbox.yMin = glyph_bbox.yMin;

		if ( glyph_bbox.xMax > bbox.xMax )
			bbox.xMax = glyph_bbox.xMax;

		if ( glyph_bbox.yMax > bbox.yMax )
			bbox.yMax = glyph_bbox.yMax;
	}

	/* check that we really grew the string bbox */
	if ( bbox.xMin > bbox.xMax )
	{
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	}

	/* return string bbox */
	*abbox = bbox;
}

static void center_draw( FT_BBox string_bbox, FT_Glyph *glyphs, FT_Vector *pos, int num_glyphs)
{
	/* compute string dimensions in integer pixels */
	int string_width, string_height, start_x, start_y;
	int my_target_width = 500, my_target_height = 500;
	int n;
	FT_Error error;

	string_width  = string_bbox.xMax - string_bbox.xMin;
	string_height = string_bbox.yMax - string_bbox.yMin;
	
	/* compute start pen position in 26.6 cartesian pixels */
	start_x = ( ( my_target_width  - string_width  ) / 2 ) * 64;
	start_y = ( ( my_target_height - string_height ) / 2 ) * 64;
	
	for ( n = 0; n < num_glyphs; n++ )
	{
	  FT_Glyph	 image;
	  FT_Vector  pen;
	
	
	  image = glyphs[n];
	
	  pen.x = start_x + (pos[n].x<<6);
	  pen.y = start_y + (pos[n].y<<6);
	
	  error = FT_Glyph_To_Bitmap( &image, FT_RENDER_MODE_MONO,&pen, 0 ); // FT_RENDER_MODE_NORMAL FT_RENDER_MODE_MONO
	  if ( !error )
	  {
		FT_BitmapGlyph	bit = (FT_BitmapGlyph)image;

		ft2_draw_bitmap( &bit->bitmap,bit->left,my_target_height - bit->top );
		FT_Done_Glyph( image );
	  }
	}

}

void center(FT_Face face, unsigned short *text, int num_chars)
{
#define MAX_GLYPHS 20

	FT_GlyphSlot  slot = face->glyph;	/* a small shortcut */
	FT_UInt 	  glyph_index;
	FT_Bool 	  use_kerning;
	FT_UInt 	  previous;
	int 		  pen_x, pen_y, n;

	FT_Glyph	  glyphs[MAX_GLYPHS];	/* glyph image	  */
	FT_Vector	  pos	[MAX_GLYPHS];	/* glyph position */
	FT_UInt 	  num_glyphs;
	FT_Error error;
	FT_BBox bbox;

	pen_x = 0;	 /* start at (0,0) */
	pen_y = 0;

	num_glyphs	= 0;
	use_kerning = FT_HAS_KERNING( face );
	previous	= 0;

	for ( n = 0; n < num_chars; n++ )
	{
		/* convert character code to glyph index */
		glyph_index = FT_Get_Char_Index( face, text[n] );

		/* retrieve kerning distance and move pen position */
		if ( use_kerning && previous && glyph_index )
		{
			FT_Vector  delta;

			FT_Get_Kerning( face, previous, glyph_index,FT_KERNING_DEFAULT, &delta );
			pen_x += delta.x >> 6;
		}

		/* store current pen position */
		pos[num_glyphs].x = pen_x;
		pos[num_glyphs].y = pen_y;

		/* load glyph image into the slot without rendering */
		error = FT_Load_Glyph( face, glyph_index, FT_LOAD_MONOCHROME ); // FT_LOAD_MONOCHROME  FT_LOAD_DEFAULT
		if ( error )
			continue;  /* ignore errors, jump to next glyph */

		/* extract glyph image and store it in our table */
		error = FT_Get_Glyph( face->glyph, &glyphs[num_glyphs] );
		if ( error )
			continue;  /* ignore errors, jump to next glyph */

		/* increment pen position */
		pen_x += (slot->advance.x >> 6);

		/* record current glyph index */
		previous = glyph_index;

		/* increment number of glyphs */
		num_glyphs++;
	}
	compute_string_bbox(&bbox, glyphs, pos, num_glyphs);
	center_draw(bbox, glyphs, pos, num_glyphs);
#undef MAX_GLYPHS	
}

// advance render
typedef struct  TGlyph_
{
	FT_UInt    index;  /* glyph index                  */
	FT_Vector  pos;    /* glyph origin on the baseline */
	FT_Glyph   image;  /* glyph image                  */
} TGlyph, *PGlyph;

static void  advance_compute_string_bbox( FT_BBox  *abbox, TGlyph *glyphs, int num_glyphs )
{
	FT_BBox  bbox;
	int n;
	
	bbox.xMin = bbox.yMin =  320000;
	bbox.xMax = bbox.yMax = -320000;

	for ( n = 0; n < num_glyphs; n++ )
	{
		FT_BBox  glyph_bbox;

		FT_Glyph_Get_CBox( glyphs[n].image, ft_glyph_bbox_pixels, &glyph_bbox );

		if (glyph_bbox.xMin < bbox.xMin)
			bbox.xMin = glyph_bbox.xMin;

		if (glyph_bbox.yMin < bbox.yMin)
			bbox.yMin = glyph_bbox.yMin;

		if (glyph_bbox.xMax > bbox.xMax)
			bbox.xMax = glyph_bbox.xMax;

		if (glyph_bbox.yMax > bbox.yMax)
			bbox.yMax = glyph_bbox.yMax;
	}

	if ( bbox.xMin > bbox.xMax )
	{
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	}

	*abbox = bbox;
}

static advance_render_draw(FT_BBox string_bbox, TGlyph *glyphs, 
	             int num_glyphs, double angle)
{
	FT_Vector start, pen;
	FT_Matrix matrix;
	int string_width, string_height;
	int my_target_width = 500;
	int my_target_height = 500;
	int n;
	FT_Error error;
	/* get bbox of original glyph sequence */
	advance_compute_string_bbox(&string_bbox, glyphs, num_glyphs);

	/* compute string dimensions in integer pixels */
	string_width  = (string_bbox.xMax - string_bbox.xMin) / 64;
	string_height = (string_bbox.yMax - string_bbox.yMin) / 64;

	/* set up start position in 26.6 cartesian space */
	start.x = ( ( my_target_width  - string_width  ) / 2 ) * 64;
	start.y = ( ( my_target_height - string_height ) / 2 ) * 64;

	/* set up transform (a rotation here) */
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	for ( n = 0; n < num_glyphs; n++ )
	{
		FT_Glyph  image;
		FT_Vector pen;
		FT_BBox   bbox;

		/* create a copy of the original glyph */
		error = FT_Glyph_Copy( glyphs[n].image, &image );
		if ( error ) 
		{
			continue;
		}
		
		/* transform copy (this will also translate it to the */
		/* correct position                                   */
		pen.x = start.x + (glyphs[n].pos.x<<6); // calculate start position
		pen.y = start.y + (glyphs[n].pos.y<<6);
		FT_Glyph_Transform( image, &matrix, &pen );

		/* check bounding box; if the transformed glyph image      */
		/* is not in our target surface, we can avoid rendering it */
		FT_Glyph_Get_CBox( image, ft_glyph_bbox_pixels, &bbox );
		if ( bbox.xMax <= 0 || bbox.xMin >= my_target_width  || bbox.yMax <= 0 || bbox.yMin >= my_target_height )
		{
			continue; // skip error, continue draw next
		}

		// convert glyph image to bitmap
		error = FT_Glyph_To_Bitmap(&image,
		                           FT_RENDER_MODE_NORMAL,  
		                           NULL, // not additional move pen
		                           1 );  // destroy the glyph copy! that's image buffer
		if ( !error )
		{
			FT_BitmapGlyph bit = (FT_BitmapGlyph)image;
			ft2_draw_pixmap( &bit->bitmap,bit->left,my_target_height - bit->top );
			FT_Done_Glyph( image );
		}
	}
}

void advance_render(FT_Face face, unsigned short *text, int num_chars, double angle)
{
#define MAX_GLYPHS  20
	FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
	FT_Bool       use_kerning;
	FT_UInt       previous;
	int           pen_x, pen_y, n;
	FT_BBox bbox;

	TGlyph        glyphs[MAX_GLYPHS];  /* glyphs table */
	PGlyph        glyph;               /* current glyph in table */
	FT_UInt       num_glyphs;
	FT_Error error;
	pen_x = 0;   /* start at (0,0) */
	pen_y = 0;

	num_glyphs  = 0;
	use_kerning = FT_HAS_KERNING( face );
	previous    = 0;

	glyph = glyphs;
	for ( n = 0; n < num_chars; n++ )
	{
		glyph->index = FT_Get_Char_Index( face, text[n] );

		if ( use_kerning && previous && glyph->index )
		{
			FT_Vector  delta;

			FT_Get_Kerning( face, previous, glyph->index, FT_KERNING_DEFAULT, &delta );
			pen_x += delta.x >> 6;
		}

		/* store current pen position */
		glyph->pos.x = pen_x;
		glyph->pos.y = pen_y;

		error = FT_Load_Glyph( face, glyph->index, FT_LOAD_DEFAULT );
		if ( error ) 
			continue;

		error = FT_Get_Glyph( face->glyph, &glyph->image );
		if ( error ) 
			continue;

		/* translate the glyph image now */
		FT_Glyph_Transform( glyph->image, 0, &glyph->pos );

		pen_x   += (slot->advance.x >> 6);
		previous = glyph->index;

		/* increment number of glyphs */
		glyph++;
	}

	/* count number of glyphs loaded */
	num_glyphs = glyph - glyphs;
	advance_render_draw(bbox, glyphs, num_glyphs, angle);	
#undef MAX_GLYPHS	
}

int ttf_main(void)
{
	FT_Library    library;
	FT_Face       face;

	FT_GlyphSlot  slot;
	FT_Vector    delt = {100, 100};
	FT_Error      error;

	char*         filename;
	unsigned char*         text;
	unsigned char *text_cn;

	double        angle;
	double degree = 0.0;
	int           target_height;
	int           num_chars;

	filename      = ".\\res\\res_freetype\\fz.ttf";                          

	text          = "hello";
	text_cn =  "\x60\x4f\x7d\x59\x2d\x4e\xfd\x56";
	
	degree = 30.0;
#if 0	
	num_chars     = strlen( text );
#else
	num_chars = 4;
#endif
	angle         = (degree/180.0)*TTF_PI;      /* use 25 degrees     */
	target_height = 500;

	error = FT_Init_FreeType(&library);              /* initialize library */
	/* error handling omitted */

	error = FT_New_Face(library, filename, 0, &face); /* create face object */
	/* error handling omitted */

	/* use 50pt at 100dpi */
	error = FT_Set_Char_Size(face, 100*32, 0, 100, 0);                /* set character size */
	/* error handling omitted */

	slot = face->glyph;

	draw_string(face, text_cn, num_chars);
	//kern(face,  (unsigned short *)text_cn, num_chars);
	//center(face, text_cn, num_chars);
	//advance_render(face, (unsigned short *)text_cn, num_chars, angle); // generate a bitmap
	
	//ft2_show_image();  // show bitmap buffer

	FT_Done_Face( face );
	FT_Done_FreeType( library );

	return 0;
}

/* EOF */
