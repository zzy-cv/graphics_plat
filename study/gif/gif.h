#ifndef  __GIF_H__
#define  __GIF_H__

#include <stdio.h>
#include <stdlib.h>

#define STARTX 2
#define STARTY 2

#define LZ_MAX_CODE         4095    /* Biggest code possible in 12 bits. */
#define LZ_BITS             12

#define FLUSH_OUTPUT        4096    /* Impossible code, to signal flush. */
#define FIRST_CODE          4097    /* Impossible code, to signal first. */
#define NO_SUCH_CODE        4098    /* Impossible code, to signal empty. */

#define HT_SIZE			8192	   /* 12bits = 4096 or twice as big! */
#define HT_KEY_MASK		0x1FFF			      /* 13bits keys */
#define HT_KEY_NUM_BITS		13			      /* 13bits keys */
#define HT_MAX_KEY		8191	/* 13bits - 1, maximal code possible */
#define HT_MAX_CODE		4095	/* Biggest code possible in 12 bits. */

typedef enum {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE, /* Begin with ',' */
    EXTENSION_RECORD_TYPE,  /* Begin with '!' */
    TERMINATE_RECORD_TYPE   /* Begin with ';' */
} gif_record_enum_t;

typedef unsigned char gif_pixel_t;

typedef struct rgb_type
{
	unsigned char r, g, b;
}rgb_t;

//typedef struct gif_hash_type{
//    unsigned int hash[HT_SIZE];
//}gif_hash_t;

typedef struct gif_lzw_type
{
	short bits_per_pixel;    /* Bits per pixel (Codes uses at least this + 1). */
	short clear_code;   /* The CLEAR LZ code. */
	short eof_code;     /* The EOF LZ code. */
	short running_code; /* The next code algorithm can generate. */
	short running_bits; /* The number of bits required to represent RunningCode. */
	short max_code1;    /* 1 bigger than max. possible code, in RunningBits bits. */
	short last_code;    /* The code before the current code. */
	short cur_code;    /* Current algorithm code. */
	short stack_ptr;    /* For character stack (see below). */
	short cur_shift_state;    /* Number of bits in CrntShiftDWord. */
	unsigned long cur_shift_dword;   /* For bytes decomposition into codes. */
	unsigned long pixel_count;   /* Number of pixels in image. */
	unsigned char buf[256];   /* Compressed input is buffered here. */
	unsigned char stack[LZ_MAX_CODE]; /* Decoded pixels are stacked here. */
	unsigned char suffix[LZ_MAX_CODE + 1];    /* So we can trace the codes. */
	unsigned short prefix[LZ_MAX_CODE + 1];
	//gif_hash_t *hash_tab;
}gif_lzw_t;

typedef struct cm_obj_type // color map object
{
	int color_count;
	int bits_per_pixel;
	rgb_t *colors;
}cm_obj_t;

typedef struct gif_image_desc_type
{
	unsigned short left;                         // current image left
	unsigned short top;                         // current image top
	unsigned short width;                      // current image width
	unsigned short height;                     // current image height
	int interlace;                 // sequential or interlaced lines
	cm_obj_t *local_cm;       // the local color map
}gif_image_desc_t;

typedef struct gif_context_type
{
	FILE *gif_fp;               // file handler
	unsigned short width;   // screen width
	unsigned short height;  // screen heighe
	int color_res;              // color resolution
	int back_color;            // back ground color
	cm_obj_t *g_cm;          // global color map
	gif_pixel_t *gif_row;     // current line pixel data

	int image_count;           // number of current image
	gif_image_desc_t image; // current image
	gif_lzw_t *lzw_data;      // lzw context
}gif_context_t;

#endif

