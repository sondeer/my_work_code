#pragma once

#include <stdio.h>

#ifdef _DEBUG

#define Debug_Printf(format, ...)   printf(format, __VA_ARGS__)

#else

#define Debug_Printf(format, ...) 

#endif

