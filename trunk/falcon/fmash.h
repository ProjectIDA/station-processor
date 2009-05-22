#ifndef FMASH_H
#define FMASH_H

#include <buffer.h>
#include <falcon.h>
#include <simclist.h>
#include <stdint.h>
#include <time.h>

#define TM_MINUTE  60
#define TM_HOUR  3600

#define MAX_ROWS  60

/* Variable Length Integer */
typedef struct {
    size_t bytes;
    uint8_t num[5];
} varint;

/* ==== CSV Data Structures ============================================ */
typedef list_t csv_list_t;

typedef struct {
    time_t timestamp;
    int32_t average;
    int32_t high;
    int32_t low;
    uint8_t empty;
} csv_row_t;

typedef struct {
    char* name;
    char* description;
    char* unit_of_measure;
    uint16_t channel;
} csv_header_t;

typedef struct {
    char* file_name;
    csv_header_t* header;
    csv_list_t* list;
    time_t start_time;
    time_t end_time;
    time_t last_flush;
} csv_buffer_t;


/* ==== CSV Data Structure Constructors/Destructors ========================= */
csv_buffer_t* csv_buffer_init();
csv_buffer_t* csv_buffer_destroy(csv_buffer_t* csv_buffer);

csv_header_t* csv_header_init();
csv_header_t* csv_header_duplicate(csv_header_t* csv_header);
csv_header_t* csv_header_destroy(csv_header_t* csv_header);

csv_list_t* csv_list_init();
csv_list_t* csv_list_destroy(csv_list_t* csv_list);

csv_row_t* csv_row_init();
csv_row_t* csv_row_duplicate(csv_row_t* csv_row);
csv_row_t* csv_row_destroy(csv_row_t* csv_row);

/* =====-----------------------------------------============================ */
/* ==== FMash Compression/Decompression Functions =========================== */
/* =====-----------------------------------------============================ */
int fmash_csv_to_msh( csv_buffer_t* csv, uint8_t** raw_msh, size_t* length );
int fmash_msh_to_csv( csv_buffer_t** csv, uint8_t* raw_msh, size_t length );

/* ==== Variable Length Integer Support ===================================== */
// Because there is space reserved for information at the beginning of
// the least significant byte the varint should be implemented with the
// bytes in little-endian order. This makes it easy for the user to
// check the reserved space without having to know the size of
// the varint.
int32_t varint_to_int32( uint8_t* integer, size_t* bytes );
varint int32_to_varint( int32_t integer );

/* ==== Store 3-bit values in a bitstream =================================== */
void map_3_set( uint8_t* map, int index, uint8_t value );
uint8_t map_3_get( const uint8_t* map, int index );

/* ==== Store 4-bit values in a bitstream =================================== */
void map_4_set( uint8_t* map, int index, uint8_t value );
uint8_t map_4_get( const uint8_t* map, int index );

#endif

