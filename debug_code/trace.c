#include <stdio.h>
#include <stdlib.h>
#include "trace.h"

#define TRACE_OPEN

static FILE *f_trace;

void open_trace_file()
{
    f_trace = fopen("trace.txt","w+");
}

void write_trace(char *str, int i)
{
#ifdef TRACE_OPEN
    fprintf(f_trace, "%s", str);
    if(i != -1)
        fprintf(f_trace, "0x%x\n", i);
#endif
}

void close_trace_file()
{
    fclose(f_trace);
}
