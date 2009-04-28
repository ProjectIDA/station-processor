#ifndef FMASH_H
#define FMASH_H

#include <buffer.h>
#include <falcon.h>
#include <simclist.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    size_t bytes;
    uint8_t num[5];
} varint;

int32_t varint_to_int32( uint8_t* integer, size_t* bytes );
varint int32_to_varint( int32_t integer );

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

#endif
