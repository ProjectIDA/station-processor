#ifndef __DUMP_HEX__
#define __DUMP_HEX__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

void dump_hex( const uint8_t *data, const size_t len, size_t bytes_per_line, size_t lines_per_section );

#endif

