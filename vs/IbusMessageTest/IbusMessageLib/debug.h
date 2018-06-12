#pragma once

#include <stdio.h>

//#ifdef _DEBUG
extern FILE *fp;
#define Debug_Printf(format, ...)   printf(format, __VA_ARGS__);fprintf(fp,format,__VA_ARGS__)

//#else

//#define Debug_Printf(format, ...) 

//#endif

