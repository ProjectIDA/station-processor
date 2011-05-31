#ifndef __JIO_H__
#define __JIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define INDENT_WIDTH 4

void jprintf( size_t depth, const char *str, ... );

#endif

