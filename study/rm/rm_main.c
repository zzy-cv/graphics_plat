#include <stdio.h>
#include <stdlib.h>
#include "rm.h"

int main(int argc, char **argv)
{
	rm_context_t *rm_ctx = NULL;

	if(rm_context_open(&rm_ctx, "01.rmvb")<0) // create rm context
	{
		return -1;
	}

	rm_read_header(rm_ctx);

	rm_context_close(rm_ctx);

	return 0;
}

