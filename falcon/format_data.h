#ifndef __FORMAT_DATA__
#define __FORMAT_DATA__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

void format_data( const uint8_t *data, const size_t len, size_t bytes_per_line, size_t lines_per_section );

#endif

