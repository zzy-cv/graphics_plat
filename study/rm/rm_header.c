#include <stdio.h>
#include <stdlib.h>
#include "rm.h"

const char * const ff_rm_metadata[4] = 
{
    "title",
    "author",
    "copyright",
    "comment"
};

const AVCodecTag ff_rm_codec_tags[] = {
    { AV_CODEC_ID_RV10,   MKTAG('R','V','1','0') },
    { AV_CODEC_ID_RV20,   MKTAG('R','V','2','0') },
    { AV_CODEC_ID_RV20,   MKTAG('R','V','T','R') },
    { AV_CODEC_ID_RV30,   MKTAG('R','V','3','0') },
    { AV_CODEC_ID_RV40,   MKTAG('R','V','4','0') },
    { AV_CODEC_ID_AC3,    MKTAG('d','n','e','t') },
    { AV_CODEC_ID_RA_144, MKTAG('l','p','c','J') },
    { AV_CODEC_ID_RA_288, MKTAG('2','8','_','8') },
    { AV_CODEC_ID_COOK,   MKTAG('c','o','o','k') },
    { AV_CODEC_ID_ATRAC3, MKTAG('a','t','r','c') },
    { AV_CODEC_ID_SIPR,   MKTAG('s','i','p','r') },
    { AV_CODEC_ID_AAC,    MKTAG('r','a','a','c') },
    { AV_CODEC_ID_AAC,    MKTAG('r','a','c','p') },
    { AV_CODEC_ID_RALF,   MKTAG('L','S','D',':') },
    { AV_CODEC_ID_NONE },
};

int rm_context_open(rm_context_t **rm_ctx, 
	unsigned char *file_name)
{
	rm_context_t *rm_ctx_tmp = *rm_ctx;
	FILE *fp = NULL;

	if(NULL == rm_ctx_tmp) // alloc memory for rm context
	{
		rm_ctx_tmp = (rm_context_t *)malloc(sizeof(rm_context_t));
		if(NULL == rm_ctx_tmp)
		{
			printf("alloc error, %s:%d\n", __FILE__, __LINE__);
			return -1;
		}
	}

	fp = fopen(file_name, "rb");
	if(NULL == fp)
	{
		printf("file open error, %s:%d\n", __FILE__, __LINE__);		
		return -1;
	}

	rm_ctx_tmp->fp = fp;
	printf("rm context create ok\n");

	*rm_ctx = rm_ctx_tmp;

	return 0;
}

int rm_context_close(rm_context_t *rm_ctx)
{
	fclose(rm_ctx->fp);
	free(rm_ctx);

	return 0;
}

int rm_read(rm_context_t *rm_ctx, unsigned char *buf, int size)
{
	return fread(buf, 1, size, rm_ctx->fp);
}

int rm_seek(rm_context_t *rm_ctx, int offset, int whence)
{
	return fseek(rm_ctx->fp, offset, whence);
}

unsigned int rm_read_8bits(rm_context_t *rm_ctx)
{
	int ret = 0;
	unsigned char ch = 0;

	ret = fread(&ch, 1, 1, rm_ctx->fp);
	if(ret != 1)
	{
		printf("file read error: %s, %d\n", __FILE__, __LINE__);
	}

	return (unsigned int)ch;
}

unsigned int rm_read_rl16bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;

	ret = rm_read_8bits(rm_ctx);
	ret |= rm_read_8bits(rm_ctx) << 8;

	return ret;
}

unsigned int rm_read_rl24bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;
	
	ret = rm_read_rl16bits(rm_ctx);
	ret |= rm_read_8bits(rm_ctx)<<16;

	return ret;
}

unsigned int rm_read_rl32bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;

	ret = rm_read_rl16bits(rm_ctx);
	ret |= rm_read_rl16bits(rm_ctx) << 16;

	return ret;
}

uint64 rm_read_rl64bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;

	ret = (uint64)rm_read_rl32bits(rm_ctx); 
	ret |= (uint64)rm_read_rl32bits(rm_ctx) << 32;

	return ret;
}

unsigned int rm_read_rb16bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;

	ret = rm_read_8bits(rm_ctx)<<8;
	ret |= rm_read_8bits(rm_ctx);

	return ret;
}

unsigned int rm_read_rb24bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;

	ret = rm_read_rb16bits(rm_ctx) << 8;
	ret |= rm_read_8bits(rm_ctx);

	return ret;
}

unsigned int rm_read_rb32bits(rm_context_t *rm_ctx)
{
	unsigned int ret = 0;

	ret = rm_read_rb16bits(rm_ctx) << 16;
	ret |= rm_read_rb16bits(rm_ctx);

	return ret;
}

int rm_skip(rm_context_t *rm_ctx, int offset)
{
	return fseek(rm_ctx->fp, offset, SEEK_CUR);
}


int rm_tell(rm_context_t *rm_ctx)
{
	return ftell(rm_ctx->fp);
}

int get_strl(rm_context_t *rm_ctx,  char *buf_tmp, 
	int buf_size, int len)
{
	int i;
	char *q, r;

	q = buf_tmp;
	for(i=0;i<len;i++) 
	{
		r = rm_read_8bits(rm_ctx);
		if (i < buf_size - 1)
			*q++ = r;
	}

	if (buf_size > 0) *q = '\0';

	return 0;
}

static void get_str8(rm_context_t *rm_ctx, char *buf_tmp, int buf_size)
{
    get_strl(rm_ctx, buf_tmp, buf_size, rm_read_8bits(rm_ctx));
}

static void rm_read_metadata(rm_context_t *rm_ctx, int wide)
{
	char buf_tmp[1024];
	int i;

	for (i=0; i<FF_ARRAY_ELEMS(ff_rm_metadata); i++) 
	{
		int len = wide ? rm_read_rb16bits(rm_ctx) : rm_read_8bits(rm_ctx);
		get_strl(rm_ctx, buf_tmp, sizeof(buf_tmp), len);
		printf("%s: %s\n", ff_rm_metadata[i], buf_tmp);
		// av_dict_set(&s->metadata, ff_rm_metadata[i], buf, 0);
	}
}

static int av_toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        c ^= 0x20;
    return c;
}

unsigned int avpriv_toupper4(unsigned int x)
{
    return av_toupper(x & 0xFF) +
          (av_toupper((x >>  8) & 0xFF) << 8)  +
          (av_toupper((x >> 16) & 0xFF) << 16) +
          (av_toupper((x >> 24) & 0xFF) << 24);
}

int ff_codec_get_id(const AVCodecTag *tags, unsigned int tag)
{
    int i;
    for(i=0; tags[i].id != AV_CODEC_ID_NONE;i++) {
        if(tag == tags[i].tag)
            return tags[i].id;
    }
    for(i=0; tags[i].id != AV_CODEC_ID_NONE; i++) {
        if (avpriv_toupper4(tag) == avpriv_toupper4(tags[i].tag))
            return tags[i].id;
    }
    return AV_CODEC_ID_NONE;
}

static int rm_read_audio_stream_info(rm_context_t *rm_ctx, int read_all)
{
	unsigned char buf_tmp[256];
	unsigned int version;
	int ret;

	/* ra type header */
	version = rm_read_rb16bits(rm_ctx); /* version */
	printf("version: %d\n", version);
	if (version == 3) 
	{
		unsigned bytes_per_minute;
		int header_size = rm_read_rb16bits(rm_ctx);
		int startpos = rm_tell(rm_ctx);
		
		rm_skip(rm_ctx, 8);
		bytes_per_minute = rm_read_rb16bits(rm_ctx);
		rm_skip(rm_ctx, 4);
		rm_read_metadata(rm_ctx, 0);
		if ((startpos + header_size) >= rm_tell(rm_ctx) + 2) 
		{
			// fourcc (should always be "lpcJ")
			rm_read_8bits(rm_ctx);
			get_str8(rm_ctx, buf_tmp, sizeof(rm_ctx));
		}
		// Skip extra header crap (this should never happen)
		if ((startpos + header_size) > rm_tell(rm_ctx))
			rm_skip(rm_ctx, header_size + startpos - rm_tell(rm_ctx));
		//if (bytes_per_minute)
		//st->codec->bit_rate = 8LL * bytes_per_minute / 60;
		//st->codec->sample_rate = 8000;
		//st->codec->channels = 1;
		//st->codec->channel_layout = AV_CH_LAYOUT_MONO;
		//st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
		//st->codec->codec_id = AV_CODEC_ID_RA_144;
		//ast->deint_id = DEINT_ID_INT0;
	} 
	else 
	{
		int flavor, sub_packet_h, coded_framesize, sub_packet_size;
		int codecdata_length;
		unsigned bytes_per_minute;
		int block_align, sample_rate, channels, deint_id;
		int codec_type, codec_tag, codec_id;
		
		/* old version (4) */
		rm_skip(rm_ctx, 2); /* unused */
		rm_read_rb32bits(rm_ctx); /* .ra4 */
		rm_read_rb32bits(rm_ctx); /* data size */
		rm_read_rb16bits(rm_ctx); /* version2 */
		rm_read_rb32bits(rm_ctx); /* header size */
		flavor= rm_read_rb16bits(rm_ctx); /* add codec info / flavor */
		coded_framesize = rm_read_rb32bits(rm_ctx); /* coded frame size */
		rm_read_rb32bits(rm_ctx); /* ??? */
		bytes_per_minute = rm_read_rb32bits(rm_ctx);
		if (version == 4) 
		{
			if (bytes_per_minute)
				; //st->codec->bit_rate = 8LL * bytes_per_minute / 60;
		}
		rm_read_rb32bits(rm_ctx); /* ??? */
		sub_packet_h = rm_read_rb16bits(rm_ctx); /* 1 */
		block_align= rm_read_rb16bits(rm_ctx); /* frame size */
		sub_packet_size = rm_read_rb16bits(rm_ctx); /* sub packet size */
		rm_read_rb16bits(rm_ctx); /* ??? */
		if (version == 5) 
		{
			rm_read_rb16bits(rm_ctx); 
			rm_read_rb16bits(rm_ctx); 
			rm_read_rb16bits(rm_ctx); 
		}
		sample_rate = rm_read_rb16bits(rm_ctx);
		rm_read_rb32bits(rm_ctx);
		channels = rm_read_rb16bits(rm_ctx);
		if (version == 5) 
		{
			deint_id = rm_read_rl32bits(rm_ctx);
			rm_read(rm_ctx, buf_tmp, 4);
			buf_tmp[4] = 0;
		} 
		else 
		{
			get_str8(rm_ctx, buf_tmp, sizeof(buf_tmp)); /* desc */
			deint_id = AV_RL32(buf_tmp);
			get_str8(rm_ctx, buf_tmp, sizeof(buf_tmp)); /* desc */
		}
		codec_type = AVMEDIA_TYPE_AUDIO;
		codec_tag  = AV_RL32(buf_tmp);
		codec_id   = ff_codec_get_id(ff_rm_codec_tags, codec_tag);
		rm_codec_print_id(codec_id);

		switch (codec_id) 
		{
			case AV_CODEC_ID_AC3:
				//st->need_parsing = AVSTREAM_PARSE_FULL;
				break;
			case AV_CODEC_ID_RA_288:
				//st->codec->extradata_size= 0;
				//ast->audio_framesize = st->codec->block_align;
				//st->codec->block_align = coded_framesize;
				break;
			case AV_CODEC_ID_COOK:
				//st->need_parsing = AVSTREAM_PARSE_HEADERS;
			case AV_CODEC_ID_ATRAC3:
			case AV_CODEC_ID_SIPR:
				if (read_all) 
				{
					codecdata_length = 0;
				} 
				else 
				{
					rm_read_rb16bits(rm_ctx); 
					rm_read_8bits(rm_ctx);
					if (version == 5)
					{
						rm_read_8bits(rm_ctx);
					}
					
					codecdata_length = rm_read_rb32bits(rm_ctx);
					printf("codec data length: %d\n", codecdata_length);
					//if(codecdata_length + FF_INPUT_BUFFER_PADDING_SIZE 
					//	<= (unsigned)codecdata_length)
					//{
					//	av_log(s, AV_LOG_ERROR, "codecdata_length too large\n");
					//	return -1;
					//}
				}

				//ast->audio_framesize = st->codec->block_align;
				if (codec_id == AV_CODEC_ID_SIPR) 
				{
					if (flavor > 3) 
					{
						// av_log(s, AV_LOG_ERROR, "bad SIPR file flavor %d\n",
						// flavor);
						return -1;
					}
					// st->codec->block_align = ff_sipr_subpk_size[flavor];
				} 
				else 
				{
					if(sub_packet_size <= 0)
					{
						// av_log(s, AV_LOG_ERROR, "sub_packet_size is invalid\n");
						return -1;
					}
					// st->codec->block_align = ast->sub_packet_size;
				}
				
				// if ((ret = rm_read_extradata(pb, st->codec, codecdata_length)) < 0)
				// return ret;
				// read extra data
				rm_skip(rm_ctx, codecdata_length);
				
				break;

			case AV_CODEC_ID_AAC:
				rm_read_rb16bits(rm_ctx); 
				rm_read_8bits(rm_ctx);

				if (version == 5)
				{
					rm_read_8bits(rm_ctx);
				}
				
				codecdata_length = rm_read_rb32bits(rm_ctx);
				//if(codecdata_length + FF_INPUT_BUFFER_PADDING_SIZE <= (unsigned)codecdata_length){
				//av_log(s, AV_LOG_ERROR, "codecdata_length too large\n");
				//return -1;
				//}
				
				if (codecdata_length >= 1) 
				{
					rm_read_8bits(rm_ctx);
					//if ((ret = rm_read_extradata(pb, st->codec, codecdata_length - 1)) < 0)
					//return ret;
					rm_skip(rm_ctx, codecdata_length);
				}
				break;
			default:
				;
				// av_strlcpy(st->codec->codec_name, buf, sizeof(st->codec->codec_name));
		}

		#if 0
		if (ast->deint_id == DEINT_ID_INT4 ||
		ast->deint_id == DEINT_ID_GENR ||
		ast->deint_id == DEINT_ID_SIPR) 
		{
			if (st->codec->block_align <= 0 ||
			ast->audio_framesize * sub_packet_h > (unsigned)INT_MAX ||
			ast->audio_framesize * sub_packet_h < st->codec->block_align)
				return AVERROR_INVALIDDATA;
			if (av_new_packet(&ast->pkt, ast->audio_framesize * sub_packet_h) < 0)
				return AVERROR(ENOMEM);
		}
		#endif
		
		switch (deint_id) 
		{
			#if 0
			case DEINT_ID_INT4:
				if (ast->coded_framesize > ast->audio_framesize ||
				sub_packet_h <= 1 ||
				ast->coded_framesize * sub_packet_h > (2 + (sub_packet_h & 1)) * ast->audio_framesize)
				return AVERROR_INVALIDDATA;
				break;
			case DEINT_ID_GENR:
				if (ast->sub_packet_size <= 0 ||
				ast->sub_packet_size > ast->audio_framesize)
				return AVERROR_INVALIDDATA;
				break;
			case DEINT_ID_SIPR:
			case DEINT_ID_INT0:
			case DEINT_ID_VBRS:
			case DEINT_ID_VBRF:
				break;
			default:
				// av_log(s, AV_LOG_ERROR, "Unknown interleaver %X\n", ast->deint_id);
				// return AVERROR_INVALIDDATA;
				return -1;
			#endif
		}

		if (read_all) 
		{
			rm_read_8bits(rm_ctx);
			rm_read_8bits(rm_ctx);
			rm_read_8bits(rm_ctx);
			rm_read_metadata(rm_ctx, 0);
		}
	}
	return 0;
}

int rm_codec_print_id(int tag_id)
{
	int i;
	switch(tag_id)
	{
		case AV_CODEC_ID_RV10:
			printf("codec id: AV_CODEC_ID_RV10\n");
			break;		
		case AV_CODEC_ID_RV20:
			printf("codec id: AV_CODEC_ID_RV20\n");
			break;		
		case AV_CODEC_ID_RV30:
			printf("codec id: AV_CODEC_ID_RV30\n");
			break;
		case AV_CODEC_ID_RV40:
			printf("codec id: AV_CODEC_ID_RV40\n");
			break;			
		case AV_CODEC_ID_AC3:
			printf("codec id: AV_CODEC_ID_AC3\n");
			break;			
		case AV_CODEC_ID_RA_144:
			printf("codec id: AV_CODEC_ID_RA_144\n");
			break;			
		case AV_CODEC_ID_RA_288:
			printf("codec id: AV_CODEC_ID_RA_288\n");
			break;			
		case AV_CODEC_ID_COOK:
			printf("codec id: AV_CODEC_ID_COOK\n");
			break;			
		case AV_CODEC_ID_ATRAC3:
			printf("codec id: AV_CODEC_ID_ATRAC3\n");
			break;			
		case AV_CODEC_ID_SIPR:
			printf("codec id: AV_CODEC_ID_SIPR\n");
			break;			
		case AV_CODEC_ID_AAC:
			printf("codec id: AV_CODEC_ID_AAC\n");
			break;						
		case AV_CODEC_ID_RALF:
			printf("codec id: AV_CODEC_ID_RALF\n");
			break;			
		case AV_CODEC_ID_NONE: 
			printf("codec id: AV_CODEC_ID_NONE\n");
			break;
		default:
			printf("codec id: error\n");
			break;
	}
	
	return 0;
}

int
rm_read_mdpr_codecdata (rm_context_t *rm_ctx, 
	int codec_data_size, const unsigned char *mime)
{
	unsigned int v;
	int size;
	int codec_pos;
	int ret;

	codec_pos = rm_tell(rm_ctx);
	v = rm_read_rb32bits(rm_ctx);

	if (v == MKTAG(0xfd, 'a', 'r', '.')) 
	{
		/* ra type header */
		printf("tag: [0xfd]ar.\n");
		printf("codec type: AUDIO\n");
		if (rm_read_audio_stream_info(rm_ctx, 0))
			return -1;
	} 
	else if (v == MKBETAG('L', 'S', 'D', ':')) 
	{
		printf("tag: LSD:\n");
		printf("codec type: AUDIO\n");
		// avio_seek(pb, -4, SEEK_CUR);
		rm_seek(rm_ctx, -4, SEEK_CUR);
		
		// if ((ret = rm_read_extradata(pb, st->codec, codec_data_size)) < 0)
		// return ret;
		// read extra data
		//*buf = *buf + codec_data_size;
		rm_skip(rm_ctx, codec_data_size);

		// st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
		// st->codec->codec_tag  = AV_RL32(st->codec->extradata);
		// st->codec->codec_id   = ff_codec_get_id(ff_rm_codec_tags,
		// st->codec->codec_tag);
	} 
	else if(mime && !strcmp(mime, "logical-fileinfo"))
	{
		int stream_count, rule_count, property_count, i;

		printf("tag: logical-fileinfo\n");
		// ff_free_stream(s, st);
		if (rm_read_rb16bits(rm_ctx) != 0) 
		{
			printf("unsupported version\n");
			// av_log(s, AV_LOG_WARNING, "Unsupported version\n");
			goto skip;
		}

		stream_count = rm_read_rb16bits(rm_ctx);
		rm_skip(rm_ctx, 6*stream_count);
		rule_count = rm_read_rb16bits(rm_ctx);
		rm_skip(rm_ctx, 2*rule_count);
		property_count = rm_read_rb16bits(rm_ctx);
		printf("property count: %d\n", property_count);
		for(i=0; i<property_count; i++)
		{
			unsigned char name[128], val[128];
			
			rm_read_rb32bits(rm_ctx);
			if (rm_read_rb16bits(rm_ctx) != 0) 
			{
				printf("unsupported name value property\n");
				// av_log(s, AV_LOG_WARNING, "Unsupported Name value property version\n");
				goto skip; //FIXME skip just this one
			}
			get_str8(rm_ctx, name, sizeof(name));
			switch(rm_read_rb32bits(rm_ctx)) 
			{
				case 2: get_strl(rm_ctx, val, sizeof(val), 
					rm_read_rb16bits(rm_ctx));
					printf("%-17s: %s\n", name, val);
					// av_dict_set(&s->metadata, name, val, 0);
					break;
				default: 
					rm_skip(rm_ctx, rm_read_rb16bits(rm_ctx));
			}
		}
	}
	else 
	{
		int fps;
		int codec_tag, codec_id;
		int codec_width, codec_height;

		printf("tag: VIDO\n");
		if (rm_read_rl32bits(rm_ctx) != MKTAG('V', 'I', 'D', 'O')) 
		{
			fail1:
				printf("unsupported stream type: %08x\n", v);
				// av_log(s, AV_LOG_WARNING, 
				// "Unsupported stream type %08x\n", v);
				goto skip;
		}
		printf("codec type: VIDO\n");
		codec_tag = rm_read_rl32bits(rm_ctx);
		codec_id  = ff_codec_get_id(ff_rm_codec_tags, codec_tag);
		rm_codec_print_id(codec_id);

		// av_dlog(s, "%X %X\n", st->codec->codec_tag, MKTAG('R', 'V', '2', '0'));
		
		if (codec_id == AV_CODEC_ID_NONE)
			goto fail1;
		codec_width  = rm_read_rb16bits(rm_ctx);
		codec_height = rm_read_rb16bits(rm_ctx);
		printf("codec width=%d, height=%d\n", codec_width, codec_height);
		rm_skip(rm_ctx, 2); // looks like bits per sample
		rm_skip(rm_ctx, 4); // always zero?
		//st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
		//st->need_parsing = AVSTREAM_PARSE_TIMESTAMPS;
		fps = rm_read_rb32bits(rm_ctx);

		//if ((ret = rm_read_extradata(pb, st->codec, codec_data_size - (avio_tell(pb) - codec_pos))) < 0)
		//	return ret;
		rm_skip(rm_ctx, codec_data_size);

		//av_reduce(&st->avg_frame_rate.den, &st->avg_frame_rate.num,
		//0x10000, fps, (1 << 30) - 1);
#if FF_API_R_FRAME_RATE
		// st->r_frame_rate = st->avg_frame_rate;
#endif
	}

skip:
    /* skip codec info */
    size = rm_tell(rm_ctx) - codec_pos;
    rm_skip(rm_ctx, codec_data_size - size);

    return 0;
}

int rm_read_header_old(rm_context_t *rm_ctx)
{
	printf("todo: rm read header old\n");
	return 0;
}

int rm_read_header(rm_context_t *rm_ctx)
{
	// todo check buf overflow
	unsigned int tag = 0;
	int tag_size = 0;
	unsigned int duration = 0;
	unsigned int index_off = 0, data_off = 0;
	int flags = 0;
	int file_offset;

	file_offset = rm_tell(rm_ctx);
	printf("file offset: 0x%x\n", file_offset);
	tag = rm_read_rl32bits(rm_ctx);
	if (tag == MKTAG('.', 'r', 'a', 0xfd)) 
	{
		printf("tag=.ra[0xfd]\n");
		return rm_read_header_old(rm_ctx);
	} 
	else if (tag != MKTAG('.', 'R', 'M', 'F')) 
	{
		printf("not rmvb format\n");
	}

	tag_size = rm_read_rb32bits(rm_ctx);
	printf("tag=.RMF size=%d\n", tag_size);

	rm_skip(rm_ctx, tag_size-8);

	for(;;)
	{
		int offset = 0;

		offset = rm_tell(rm_ctx);
		printf("file offset: 0x%x\n",  offset);
		tag = rm_read_rl32bits(rm_ctx);
		tag_size = rm_read_rb32bits(rm_ctx);
		rm_read_rb16bits(rm_ctx);
		printf("tag=%c%c%c%c(0x%08x) size=%d\n", 
			(tag      ) & 0xff,
			(tag >>  8) & 0xff,
			(tag >> 16) & 0xff,
			(tag >> 24) & 0xff,
			tag,
			tag_size			
			);
		if(tag_size < 10 && tag != MKTAG('D', 'A', 'T', 'A'))
		{
			return -1;
		}
		switch(tag)
		{
		case MKTAG('P', 'R', 'O', 'P'):
			/* file header */
			rm_read_rb32bits(rm_ctx); /* max bit rate */
			rm_read_rb32bits(rm_ctx); /* avg bit rate */
			rm_read_rb32bits(rm_ctx); /* max packet size */
			rm_read_rb32bits(rm_ctx); /* avg packet size */
			rm_read_rb32bits(rm_ctx); /* nb packets */
			duration = rm_read_rb32bits(rm_ctx); /* duration */
			// duration = av_rescale(duration, AV_TIME_BASE, 1000);
			rm_read_rb32bits(rm_ctx); /* preroll */
			index_off = rm_read_rb32bits(rm_ctx); /* index offset */
			data_off = rm_read_rb32bits(rm_ctx); /* data offset */
			rm_read_rb16bits(rm_ctx); /* nb streams */
			flags = rm_read_rb16bits(rm_ctx); /* flags */
			printf("\n");
			break;
		case MKTAG('C', 'O', 'N', 'T'):
			rm_read_metadata(rm_ctx, 1);
			printf("\n");
			break;
		case MKTAG('M', 'D', 'P', 'R'):
			{
				int st_id = 0;
				int st_bit_rate = 0;
				unsigned int start_time = 0;
				unsigned int st_start_time = 0;
				unsigned int st_duration = 0;
				unsigned int s_duration = 0;
				char buf_tmp[128], mime[128];
				int st_codec_type = 0;
				
				st_id = rm_read_rb16bits(rm_ctx);
				rm_read_rb32bits(rm_ctx); /* max bit rate */
				st_bit_rate = rm_read_rb32bits(rm_ctx); /* bit rate */
				rm_read_rb32bits(rm_ctx); /* max packet size */
				rm_read_rb32bits(rm_ctx); /* avg packet size */
				start_time = rm_read_rb32bits(rm_ctx); /* start time */
				rm_read_rb32bits(rm_ctx); /* preroll */
				duration = rm_read_rb32bits(rm_ctx); /* duration */
				st_start_time = start_time;
				st_duration = duration;
				if(duration>0)
				{
					s_duration = AV_NOPTS_VALUE;
				}
				get_str8(rm_ctx, buf_tmp, sizeof(buf_tmp)); /* desc */
				printf("desc: %s\n", buf_tmp);
				get_str8(rm_ctx, mime, sizeof(mime)); /* mimetype */
				printf("mime: %s\n", mime);
				st_codec_type = AVMEDIA_TYPE_DATA;
				// st->priv_data = ff_rm_alloc_rmstream();
				if (rm_read_mdpr_codecdata(rm_ctx, 
					rm_read_rb32bits(rm_ctx), mime) < 0)
				{
					printf("read mdpr codec data error");
					return -1;
				}
				printf("\n");
				break;
			}
		case MKTAG('D', 'A', 'T', 'A'):
			printf("\n");
			goto header_end;
		default:
			/* unknown tag: skip it */
			rm_skip(rm_ctx, tag_size - 10);
			break;
		}
	}

header_end:
	{
		int nb_packets;
		nb_packets = rm_read_rb32bits(rm_ctx); /* number of packets */
		if (!nb_packets && (flags & 4))
		{
			nb_packets = 3600 * 25;
		}
		rm_read_rb32bits(rm_ctx); /* next data header */

		if (!data_off)
		{
			data_off = rm_tell(rm_ctx) - 18;
		}

		printf("index_off=0x%x, data_off=0x%x\n", index_off, data_off);
		//*buf = &probe_buf[index_off];

		//rm_read_index(s);		
		//avio_seek(pb, data_off + 18, SEEK_SET);
	}	
	return 0;
}

