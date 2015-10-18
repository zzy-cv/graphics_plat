#include <stdio.h>
#include "windows.h"
#include <gif_lib.h>
#include <assert.h>

/* image formats */
enum RImageFormat {
    RRGBFormat,
    RRGBAFormat
};

typedef struct RColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} RColor;

typedef struct RImage {
    unsigned char *data;       /* image data RGBA or RGB */
    int width, height;	   /* size of the image */
    enum RImageFormat format;
    RColor background;	   /* background color */
    int refCount;
} RImage;

#define mymalloc malloc

static int
    ImageNum = 0,
    BackGround = 0,
    OneFileFlag = FALSE,
    HelpFlag = FALSE,
    ColorMapSize = 0,
    InterlacedOffset[] = { 0, 4, 2, 1 }, /* The way Interlaced image should. */
    InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */
static ColorMapObject
    *ColorMap;

FILE *fp_gif;
GifRowType GifRow;
	GifFileType *gif_file = NULL;

int gif_main_init() 
{
	int colors_num;
	unsigned char *colors;
	int Size;
	int i;

	/*  unsigned char *Buffer, *BufferP; */
	fp_gif = fopen(".\\res\\res_gif\\porsche.gif", "rb");  // "porsche.gif"  "jump.gif"  "gif_gril.gif" "460.gif"
	if(NULL == fp_gif)
	{
		assert(0);
	}
	if ((gif_file = DGifOpenFileHandle(fp_gif)) == NULL) 
	{
		assert(0);
	}

	colors_num = (1<<gif_file->SColorMap->BitsPerPixel);
	colors = gif_file->SColorMap->Colors;

	BackGround = gif_file->SBackGroundColor;
	ColorMap = (gif_file->Image.ColorMap ? gif_file->Image.ColorMap : gif_file->SColorMap);
	ColorMapSize = ColorMap->ColorCount;

	Size = gif_file->SWidth * sizeof(GifPixelType); 
  
	if ((GifRow = (GifRowType) mymalloc(Size)) == NULL)
	{
		assert(0); /* First row. */
	}
  
	for (i = 0; i < gif_file->SWidth; i++) 
	{
		GifRow[i] = gif_file->SBackGroundColor;
	}
	open_trace_file();
#if 0
	{
		int i=0;
		for(i=0; i<256;i++)
		{
			write_trace("", i);
			write_trace("   ", ColorMap->Colors[i].Red);
			write_trace("   ", ColorMap->Colors[i].Green);
			write_trace("   ", ColorMap->Colors[i].Blue);
		}
	}
#endif
}

int gif_main_play_a_frame(int *rec_type, int *alpha)
{
	int	i, j;
	int Width, Height, ExtCode, Count, x;
	int Row, Col;
	GifByteType *Extension;
	//int alpha = -1;

	/*  GifRowType *ScreenBuffer; */
	static GifColorType *ColorMapEntry;

	/* Scan the content of the GIF file and load the image(s) in: */
	if (DGifGetRecordType(gif_file, rec_type) == GIF_ERROR) 
	{
		PrintGifError();
		return (-1);
	}

	switch (*rec_type)
	{
		case IMAGE_DESC_RECORD_TYPE:
		if (DGifGetImageDesc(gif_file) == GIF_ERROR) 
		{
			PrintGifError();
			return (-1);
		}
		Row = gif_file->Image.Top; /* Image Position relative to Screen. */
		Col = gif_file->Image.Left;
		Width = gif_file->Image.Width;
		Height = gif_file->Image.Height;
		if (gif_file->Image.Left + gif_file->Image.Width > gif_file->SWidth ||
		gif_file->Image.Top + gif_file->Image.Height > gif_file->SHeight) 
		{
			return(0);
		}
		if (gif_file->Image.Interlace) 
		{
			for (Count = i = 0; i < 4; i++) 
			{
				for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i]) 
				{
					Count++;
					if (DGifGetLine(gif_file, &GifRow[Col], Width) == GIF_ERROR) 
					{
						return (-1);
					}
				}
				for (x = 0; x <  Width; x++)  // for (x = 0; x < gif_file->SWidth; x++) 
				{
					if(*alpha != -1 && *alpha == GifRow[x])
					{
						continue;
					}
					ColorMapEntry = &ColorMap->Colors[GifRow[x]];
					putpixel_for_rgb(x+Col, Row, ColorMapEntry->Red, 
					              ColorMapEntry->Green, ColorMapEntry->Blue);
				}
			}
		}
		else 
		{
			for (i = 0; i < Height; i++) 
			{
				Row++;
				if (DGifGetLine(gif_file, &GifRow[Col], Width) == GIF_ERROR) 
				{
				return (-1);
				}

				for (x = 0; x < Width; x++)  // for (x = 0; x < gif_file->SWidth; x++) 
				{
					if(*alpha != -1 && *alpha == GifRow[x+Col]) 
					{
						continue;
					}
					ColorMapEntry = &ColorMap->Colors[GifRow[x+Col]];
					putpixel_for_rgb(x+Col, Row, ColorMapEntry->Red, 
					ColorMapEntry->Green, ColorMapEntry->Blue);
				}
			}
		}
		break;
		case EXTENSION_RECORD_TYPE:
		/* Skip any extension blocks in file: */
		*alpha = -1;
		if (DGifGetExtension(gif_file, &ExtCode, &Extension) == GIF_ERROR) 
		{
			return (-1);
		}
		//write_trace("ExtCode:", ExtCode);
		//write_trace("0:", Extension[0]);
		//write_trace("2:", Extension[2]);
		while (Extension != NULL) 
		{
			if((0xf9 == ExtCode) && (Extension[1]&1) && (*alpha < 0))
			{
				char a[8];
				memset(a, 0, 8);
				*alpha = (int)Extension[4];
				//sprintf(a, "%d", alpha);
				write_trace("alpha:", *alpha);
				//break;
			}
			write_trace("ExtCode:", ExtCode);
			write_trace("Extension1:", Extension[1]&1);
			//write_trace("2", Extension[2]);
			Extension = NULL;
			if (DGifGetExtensionNext(gif_file, &Extension) == GIF_ERROR) 
			{
				return (-1);
			}
			if(Extension != NULL)
				write_trace("ExtensionNext:", Extension[1]);
		}

		break;
		case TERMINATE_RECORD_TYPE:
		break;
		default:		    /* Should be traps by DGifGetRecordType. */
		break;
	}
	
	return 0;
}

int gif_main_play()
{
	int rec_type = 0;
	static int i = 0;
	static int alpha = -1;

	write_trace("frame:", i);
	do
	{
		//clear_rgb(0x0, 0x0, 0x0);
		gif_main_play_a_frame(&rec_type, &alpha);
		if(IMAGE_DESC_RECORD_TYPE == rec_type)
		{
			break;
		}

		if(rec_type == TERMINATE_RECORD_TYPE)
		{
			gif_main_end();
			gif_main_init();
			i = 0;
		}
	}while(1);
	if(i<10)
	{
		tplat_draw_ascii_char_12x8(30, 100, i +'0', RGB(0, 0, 0));
		i++;
	}
	else
	{
		tplat_draw_ascii_char_12x8(30, 100, i/10 +'0', RGB(0, 0, 0));
		tplat_draw_ascii_char_12x8(38, 100, i%10 +'0', RGB(0, 0, 0));
		i++;
	}
}

int gif_main_end()
{
	fclose(fp_gif);
	close_trace_file();
}

int gif_main()
{
	gif_main_init();
	gif_main_play();
	gif_main_end();
}
