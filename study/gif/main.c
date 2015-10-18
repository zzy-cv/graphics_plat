#include <stdio.h>
#include <stdlib.h>
#include "gif.h"

void *gif_malloc(int size)
{
	void *p = NULL;

	p = malloc(size);
	if(p)
	{
		memset(p, 0, size);
	}

	return p;
}

int read(gif_context_t *gif, unsigned char *buf, int len)
{
	return fread(buf, 1, len, gif->gif_fp);
}

int get_word(gif_context_t *gif, unsigned short *word)
{
	unsigned char c[2];

	if(read(gif, c, 2) != 2)
	{
		return -1;
	}

	*word = (((unsigned int)c[1]) << 8) + c[0];
	return 0;
}

static int gif_setup_uncompress(gif_context_t *gif)
{
	int i, bits_per_pixel;
	unsigned char code_size = 0;
	unsigned short *prefix;
	gif_lzw_t *lzw = gif->lzw_data;
	
	read(gif, &code_size, 1);
	bits_per_pixel = code_size;

	lzw->buf[0] = 0; // input buffer empty
	lzw->bits_per_pixel = bits_per_pixel;
	lzw->clear_code = (1<<bits_per_pixel);
	lzw->eof_code = lzw->clear_code+1;
	lzw->running_code = lzw->eof_code+1;
	lzw->running_bits = bits_per_pixel+1;   // number of bits per code
	lzw->max_code1 = 1<<lzw->running_bits;  // max code+1
	lzw->stack_ptr = 0; // no pixels on the pixel stack
	lzw->last_code = NO_SUCH_CODE;
	lzw->cur_shift_state = 0; // no information in cur_shift_dwod
	lzw->cur_shift_dword = 0;

	prefix = lzw->prefix;
	for(i=0; i<=LZ_MAX_CODE; i++)
	{
		prefix[i] = NO_SUCH_CODE;
	}
	
	return 0;
}

cm_obj_t *gif_make_cm_obj(int bits_per_pixel)
{
	cm_obj_t *obj = NULL;
	int color_count = 0;
	
	color_count = 1 << bits_per_pixel;
	obj = (cm_obj_t *)gif_malloc(sizeof(cm_obj_t));
	if(NULL == obj)
	{
		return NULL;
	}

	obj->colors = (rgb_t *)gif_malloc(color_count*sizeof(rgb_t));
	if(NULL == obj->colors)
	{
		free(obj);
		return NULL;
	}

	obj->color_count = color_count;
	obj->bits_per_pixel = bits_per_pixel;

	return obj;
}

void gif_free_cm_obj(cm_obj_t *obj)
{
	if(NULL != obj)
	{
		free(obj->colors);
		free(obj);
	}
}

// todo error handle
int gif_get_screen_desc(gif_context_t *gif)
{
	unsigned char buf[3];
	int bits_per_pixel, is_g_cmap;
	int i;

	// Logical Screen Descriptor
	get_word(gif, &gif->width);
	get_word(gif, &gif->height);
	printf("width=%hd[%x], height=%hd[%x]\n", 
		gif->width, gif->width, gif->height, gif->height);

	read(gif, buf, 3);

	gif->color_res = (((buf[0] & 0x70) + 1) >> 4) + 1;
	bits_per_pixel = (buf[0] & 0x07) + 1;
	gif->back_color = buf[1];
	is_g_cmap = buf[0] & 0x80;
	printf("color res=%d\nbit per pixel=%d\nback color=%d\nis g cmap=%d\n", 
		gif->color_res, bits_per_pixel, gif->back_color, is_g_cmap);

	if(is_g_cmap)  // have global color map
	{
		gif->g_cm = gif_make_cm_obj(bits_per_pixel);
		if(NULL == gif->g_cm)
		{
			printf("create color map object error\n");
			return -1;
		}

		// get the global color map
		for(i=0; i<gif->g_cm->color_count; i++)
		{
			if(read(gif, buf, 3)!=3)
			{
				printf("read file error\n");
				return -1;
			}
			gif->g_cm->colors[i].r = buf[0];
			gif->g_cm->colors[i].g = buf[1];
			gif->g_cm->colors[i].b = buf[2];
		}
	}
	else // not have global color map
	{
		gif->g_cm = NULL;
	}

	return 0;
}

int gif_get_record_type(gif_context_t *gif, gif_record_enum_t *type)
{
	unsigned char ch = 0;

	if(read(gif, &ch, 1)!=1)
	{
		printf("file read error\n");
		return -1;
	}

	switch(ch)
	{
		case ',':
			*type = IMAGE_DESC_RECORD_TYPE;
			break;
		case '!':
			*type = EXTENSION_RECORD_TYPE;
			break;
		case ';':
			*type = TERMINATE_RECORD_TYPE;
			break;
		default:
			*type = UNDEFINED_RECORD_TYPE;
			printf("undefined record type\n");
			return -1;
	}

	return 0;
}

gif_context_t* gif_open(unsigned char *file_name)
{
#define GIF_STAMP "GIF8*a"   
#define GIF_STAMP_LEN sizeof(GIF_STAMP) - 1
#define GIF_VERSION_POS 3

	FILE *gif_fp = NULL;
	unsigned char buf[GIF_STAMP_LEN+1];
	gif_context_t *gif = NULL;
	int i;

	// open gif file
	gif_fp = fopen(file_name, "rb");
	if(NULL == gif_fp)
	{
		printf("file open error!\n");
		return NULL;
	}

	// create gif handle
	gif = gif_malloc(sizeof(gif_context_t));
	if(NULL == gif)
	{
		printf("alloc memory error\n");
		return NULL;
	}
	memset(gif, 0, sizeof(gif_context_t));
	gif->gif_fp = gif_fp;

	// create lzw context
	gif->lzw_data = (gif_lzw_t *)gif_malloc(sizeof(gif_lzw_t));
	
	// read file stamp
	read(gif, buf, GIF_STAMP_LEN);
	printf("gif stamp: %s\n", buf);
	
	// check file if valid
	buf[GIF_STAMP_LEN] = 0;
	if (strncmp(GIF_STAMP, buf, GIF_VERSION_POS) != 0)
	{
		printf("file format error\n");
		return NULL;
	}

	// todo error handle
	if(gif_get_screen_desc(gif)<0)
	{
		return NULL;
	}

	gif->gif_row = (gif_pixel_t *)gif_malloc(gif->width*sizeof(gif_pixel_t));
	if(NULL == gif->gif_row)
	{
		printf("alloc memory error\n");
		return NULL;
	}

	for(i=0; i<gif->width; i++)
	{
		gif->gif_row[i] = gif->back_color;
	}

	return gif;
}

int gif_get_image_desc(gif_context_t *gif)
{
	int i, bits_per_pixel;
	unsigned char buf[3];
	int is_local_cmap = 0;
	gif_lzw_t *lzw = gif->lzw_data;

	if(get_word(gif, &gif->image.left)<0 ||
		get_word(gif, &gif->image.top)<0 ||
		get_word(gif, &gif->image.width)<0 ||
		get_word(gif, &gif->image.height)<0)
	{
		return -1;
	}
	printf("left=%d, top=%d, width=%d, height=%d\n", 
		gif->image.left, gif->image.top, 
		gif->image.width, gif->image.height);
	if(read(gif, buf, 1) != 1)
	{
		printf("file read error\n");
		return -1;
	}
	
	bits_per_pixel = (buf[0]&0x07)+1;
	gif->image.interlace = (buf[0]&0x40);
	is_local_cmap = buf[0]&0x80;
	printf("interlace=%d\n", gif->image.interlace);
	printf("is_local_cmap=%d\n", is_local_cmap);
	if(is_local_cmap)
	{
		printf("have local cmap\n");
	}
	else
	{
		printf("not have a local cmap\n");
	}

	lzw->pixel_count = (long)gif->image.width*(long)gif->image.height;
	gif_setup_uncompress(gif);

	return 0;
}

static int gif_get_prefix_char(unsigned short *prefix, 
	int code, int clear_code)
{
	int i = 0;
	while(code>clear_code && i++<=LZ_MAX_CODE)
	{
		code = prefix[code];
	}

	return code;
}

static int gif_buffered_input(gif_context_t *gif, 
	unsigned char *buf, unsigned char *next_byte)
{
	if(buf[0] == 0)
	{
		// needs to read the next buffer. this one is empty
		if(read(gif, buf, 1) != 1)
		{
			printf("error: %s, %d\n", __FILE__, __LINE__);
			return -1;
		}
		// there shouldn't be any empty data blocks here as the LZW spec
		// says the LZW termination code should come first. Therefore we
		// shouldn't be inside this routine at that point
		if(buf[0] == 0)
		{
			printf("error: %s, %d\n", __FILE__, __LINE__);
			return -1;
		}
		if(read(gif, &buf[1], buf[0]) != buf[0])
		{
			printf("error: %s, %d\n", __FILE__, __LINE__);
			return -1;
		}
		*next_byte = buf[1];
		buf[1] = 2; // we use now the second place as last char read
		buf[0]--;
	}
	else
	{
		*next_byte = buf[buf[1]++];
		buf[0]--;
	}
	
	return 0;
}

static int gif_uncompress_input(gif_context_t *gif, int *code)
{
	unsigned char next_byte;
	gif_lzw_t *lzw = gif->lzw_data;
	static unsigned short code_masks[] = {
		0x0000, 0x0001, 0x0003, 0x0007,
		0x000f, 0x001f, 0x003f, 0x007f,
		0x00ff, 0x01ff, 0x03ff, 0x07ff,
		0x0fff
	};

	/* The image can't contain more than LZ_BITS per code. */
	if (lzw->running_bits > LZ_BITS) 
	{
		printf("error: %s, %d\n", __FILE__, __LINE__);
		return -1;
	}

	while (lzw->cur_shift_state < lzw->running_bits) 
	{
		/* Needs to get more bytes from input stream for next code: */
		if (gif_buffered_input(gif, lzw->buf, &next_byte) < 0) 
		{
			printf("error: %s, %d\n", __FILE__, __LINE__);
			return -1;
		}
		lzw->cur_shift_dword |=
			((unsigned long)next_byte) << lzw->cur_shift_state;
		lzw->cur_shift_state += 8;
	}
	*code = lzw->cur_shift_dword & code_masks[lzw->running_bits];

	lzw->cur_shift_dword >>= lzw->running_bits;
	lzw->cur_shift_state -= lzw->running_bits;

	/* If code cannot fit into RunningBits bits, must raise its size. Note
	* however that codes above 4095 are used for special signaling.
	* If we're using LZ_BITS bits already and we're at the max code, just
	* keep using the table as it is, don't increment Private->RunningCode.
	*/
	if (lzw->running_code < LZ_MAX_CODE + 2 &&
	++lzw->running_code > lzw->max_code1 &&
	lzw->running_bits < LZ_BITS) 
	{
		lzw->max_code1 <<= 1;
		lzw->running_bits++;
	}

	return 0;
}

static int gif_uncompress_line(gif_context_t *gif, 
	gif_pixel_t *line, int line_len)
{
	int i=0, j=0;
	int cur_code, eof_code, clear_code, cur_prefix, last_code, stack_ptr;
	unsigned char *stack, *suffix;
	unsigned short *prefix;
	gif_lzw_t *lzw = gif->lzw_data;

	stack_ptr = lzw->stack_ptr;
	prefix = lzw->prefix;
	suffix = lzw->suffix;
	stack = lzw->stack;
	eof_code = lzw->eof_code;
	clear_code = lzw->clear_code;
	last_code = lzw->last_code;

	if(stack_ptr != 0)
	{
		// let pop the stack off before continueing to read the gif file
		while(stack_ptr!=0 && i<line_len)
		{
			line[i++] = stack[--stack_ptr];
		}
	}

	while(i<line_len)
	{
		if(gif_uncompress_input(gif, &cur_code) < 0)
		{
			return -1;
		}

		if(cur_code == eof_code)
		{
			if(i!=line_len-1 || lzw->pixel_count != 0)
			{
				printf("eof too soon: %s, %d\n", __FILE__, __LINE__);
				return -1;
			}
			i++;
		}
		else if(cur_code == clear_code)
		{
			// we need to start over again
			for(j=0; j<=LZ_MAX_CODE; j++)
			{
				prefix[j] = NO_SUCH_CODE;
			}
			lzw->running_code = lzw->eof_code+1;
			lzw->running_bits = lzw->bits_per_pixel+1;
			lzw->max_code1 = 1<<lzw->running_bits;
			last_code = lzw->last_code = NO_SUCH_CODE;
		}
		else
		{
			// it's regular code. if in pixel range simply add it to putput
			// stream, otherwise trace to codes linked list until the prefix
			// is in pixel range
			if(cur_code < clear_code)
			{
				// this is simple. it's pixel scalar, so add it to output
				line[i++] = cur_code;
			}
			else
			{
				// it's a code to needed to be traced: trace the linked list
				// until the prefix is a pixel, while pushing the suffix
				// pixels on our stack. if we done, pop the stack in reverse
				// (that's what stack is good for!) order to output
				if(prefix[cur_code] == NO_SUCH_CODE)
				{
					if(cur_code == lzw->running_code-2)
					{
						cur_prefix = last_code;
						suffix[lzw->running_code-2] = 
							stack[stack_ptr++] = gif_get_prefix_char(
								prefix, last_code, clear_code); 
					}
					else
					{
						printf("error: %s, %d\n", __FILE__, __LINE__);
						return -1;
					}
				}
				else
				{
					cur_prefix = cur_code;
				}
				// now (if image is OK) we should not get an NO_SUCH_CODE
				// during the trace. As we might loop forver, in case of 
				// defective image, we count the number of loops we trace
				// and stop if we got LZ_MAX_CODE. obviously we can not
				// loop more than that
				j = 0;
				while(j++<=LZ_MAX_CODE &&
					cur_prefix > clear_code && cur_prefix <= LZ_MAX_CODE)
				{
					stack[stack_ptr++] = suffix[cur_prefix];
					cur_prefix = prefix[cur_prefix];
				}
				if(j>=LZ_MAX_CODE || cur_prefix>LZ_MAX_CODE)
				{
					printf("error: %s, %d", __FILE__, __LINE__);
					return -1;
				}
				// push the last character on stack
				stack[stack_ptr++] = cur_prefix;

				// now lets pop all the stack into output
				while(stack_ptr!=0 && i<line_len)
				{
					line[i++] = stack[--stack_ptr];
				}				
			}
			if(last_code != NO_SUCH_CODE)
			{
				prefix[lzw->running_code-2] = last_code;
				if(cur_code == lzw->running_code-2)
				{
					// only allowed if cur_code is exactly the running code
					// in that case cur_code = XXXCode, cur_code or the
					// prefix code is last code and the suffix char is
					// exactly the prefix of last code
					suffix[lzw->running_code-2] = gif_get_prefix_char(prefix, 
						last_code, clear_code);
				}
				else
				{
					suffix[lzw->running_code-2] = gif_get_prefix_char(prefix, 
						cur_code, clear_code);
				}
			}
			last_code = cur_code;
		}
	}

	lzw->last_code = last_code;
	lzw->stack_ptr = stack_ptr;

	return 0;
}

int gif_get_code_next(gif_context_t *gif, 
	unsigned char **code_block)
{
	unsigned char ch;
	gif_lzw_t *lzw = gif->lzw_data;

	if(read(gif, &ch, 1) != 1)
	{
		printf("read file error, %s:%d\n", __FILE__, __LINE__);
		return -1;
	}
	printf("ch=%d\n", ch);
	if(ch> 0)
	{
		*code_block = lzw->buf;
		(*code_block)[0] = ch;
		if(read(gif, &((*code_block)[1]), ch) != ch)
		{
			printf("read file error, %s:%d\n", __FILE__, __LINE__);		
			return -1;
		}
	}
	else
	{
		*code_block = NULL;
		lzw->buf[0] = 0;
		lzw->pixel_count = 0;
	}

	return 0;
}

int gif_get_line(gif_context_t *gif, gif_pixel_t *line, int line_len)
{
	unsigned char *dummy;
	gif_lzw_t *lzw = gif->lzw_data;

	if(line_len == 0)
	{
		line_len = gif->image.width;
	}

	lzw->pixel_count -= line_len;

	if(gif_uncompress_line(gif, line, line_len) == 0)
	{
		printf("pixel_count=%d\n", gif->lzw_data->pixel_count);
		if(gif->lzw_data->pixel_count == 0)
		{
			do
			{
				if(gif_get_code_next(gif, &dummy) < 0)
				{
					return -1;
				}
			}while(dummy != NULL);
		}
		return 0;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

int gif_handle_a_record(gif_context_t *gif, int *rec_type)
{
	int i, j;
	int alpha = -1;

	if(gif_get_record_type(gif, rec_type) < 0)	
	{
		return -1;
	}

	switch(*rec_type)
	{
	case IMAGE_DESC_RECORD_TYPE:
		{
			int row = gif->image.top;
			int col = gif->image.left;
			printf("IMAGE_DESC_RECORD_TYPE\n");
			if(gif_get_image_desc(gif) < 0)
			{
				printf("get image description error\n");
				return -1;
			}
			if(gif->image.left+gif->image.width>gif->width ||
			   gif->image.top+gif->image.height>gif->height)
			{
				printf("current image size error\n");
				return -1;
			}

			// todo
			//printf("GIF-LZW data todo\n");
#if 0  // test code, skip lzw data
			{  
				unsigned char buf[1024];
				unsigned char ch;
				do
				{
					read(gif, &ch, 1);
					printf("ch=%d\n", ch);
					read(gif, buf, ch);
				}while(ch);
			}
			return 0;
#endif			
			// todo
			if(gif->image.interlace) // interlace
			{
				
			}
 			else  // sequential
			{
				for(j=0; j<gif->image.height; j++)
				{
					row++;
					printf("gif get line\n");
					if(gif_get_line(gif, &gif->gif_row[col], 
						gif->image.width) < 0)
					{
						printf("gif get line error, %s:%d\n",
							__FILE__, __LINE__);
						return -1;
					}

					for(i=0; i<gif->image.width; i++)
					{
						rgb_t rgb;
						if(alpha != -1 && alpha == gif->gif_row[i])
						{
							continue;
						}
						rgb = gif->g_cm->colors[gif->gif_row[i]];
						putpixel_by_rgb(i+col+STARTX, row+STARTY, 
							rgb.r, rgb.g, rgb.b);
					}
				}
			}
		}
		break;
	
	case EXTENSION_RECORD_TYPE:
		{
			printf("EXTENSION_RECORD_TYPE\n");
		}
		break;
	
	case TERMINATE_RECORD_TYPE:
		{
			printf("TERMINATE_RECORD_TYPE\n");
		}
		break;
		
	default:
		printf("error\n");
		return -1;
		break;
	}

	return 0;
}

int gif_play_frame(gif_context_t *gif)
{
	int rec_type = 0;
	static int i = 0;
	static int alpha = -1;

	do
	{
		if(gif_handle_a_record(gif, &rec_type)<0)
		{
			printf("error: %s, %d\n", __FILE__, __LINE__);
			break;
		}
		
		if(IMAGE_DESC_RECORD_TYPE == rec_type)
		{
			continue;
		}

		if(rec_type == TERMINATE_RECORD_TYPE)
		{
			break;
		}
	}while(1);
}

void gif_close(gif_context_t *gif)
{
	fclose(gif->gif_fp);

	if(gif->g_cm)
	{
		free(gif->g_cm);
	}

	if(gif->gif_row)
	{
		free(gif->gif_row);
	}

	if(gif->lzw_data)
	{
		free(gif->lzw_data);
	}

	free(gif);
}

int gif_main()
{
	gif_context_t *gif = NULL;

	//dc_init();

	gif = gif_open("solid2.gif");
	gif_play_frame(gif);

	gif_close(gif);
	
	//dc_release();

	return 0;
}

void main_test()
{
	gif_main();
}

