#ifndef FALCON_STRUCTURES_H
#define FALCON_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

#include <simclist.h>

#define MAX_CONTEXT_ALARMS 100
#define MAX_ALARMS 256

#define MAX_MATCHES 32 // For regular expression matching

#define TM_MINUTE 60
#define TM_HOUR 3600

#define MAX_ROWS 60 // for CSV context

#ifndef NULL
  #define NULL 0
#endif


/* === Alarms ======================================================= */
typedef list_t alarm_context_t;

typedef struct {
    char*    text;
    uint32_t hash;
    uint8_t  sent;

    time_t   timestamp;
    uint16_t channel;
    uint8_t  event;
    char     description[9];
} alarm_line_t;

alarm_context_t* alarm_context_init();
alarm_context_t* alarm_context_destroy( alarm_context_t* alarm_list );

alarm_line_t* alarm_line_init();
alarm_line_t* alarm_line_duplicate( alarm_line_t* alarm_line );
alarm_line_t* alarm_line_destroy( alarm_line_t* alarm_line );

int _alarm_list_comparator( const void* a, const void* b );
int _alarm_list_seeker( const void* element, const void* indicator );


/* === CSV ========================================================== */
typedef list_t csv_context_t;

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

csv_context_t* csv_context_init();
csv_context_t* csv_context_destroy( csv_context_t* buffer_list );

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

int _buffer_list_seeker( const void* element, const void* indicator );
int _buffer_list_comparator( const void* a, const void* b );

/* === Variable Length Integer Support ====================================== */
// Because there is space reserved for information at the beginning of
// the least significant byte the varint should be implemented with the
// bytes in little-endian order. This makes it easy for the user to
// check the reserved space without having to know the size of
// the varint.
typedef struct {
    size_t bytes;
    uint8_t num[5];
} varint;

int32_t varint_to_int32( uint8_t* integer, size_t* bytes );
varint int32_to_varint( int32_t integer );

/* === Store 3-bit values in a bitstream ==================================== */
void map_3_set( uint8_t* map, int index, uint8_t value );
uint8_t map_3_get( const uint8_t* map, int index );

/* === Store 4-bit values in a bitstream ==================================== */
void map_4_set( uint8_t* map, int index, uint8_t value );
uint8_t map_4_get( const uint8_t* map, int index );


#endif

