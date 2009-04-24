#ifndef ALARMS_H
#define ALARMS_H

#include <buffer.h>
#include <falcon.h>
#include <simclist.h>

#define MAX_MATCHES 32

typedef list_t alarm_context_t;

typedef struct {
    char* text;
    uint32_t hash;
    uint8_t sent;
} alarm_line_t;

void alarm_poll( alarm_context_t* alarm_list, buffer_t* url_str,
                 st_info_t* st_info );
int alarm_filter_lines( alarm_context_t* lines, buffer_t* buf );
alarm_context_t* alarm_context_init();
alarm_context_t* alarm_context_destroy( alarm_context_t* alarm_list );

alarm_line_t* alarm_line_init();
alarm_line_t* alarm_line_duplicate( alarm_line_t* alarm_line );
alarm_line_t* alarm_line_destroy( alarm_line_t* alarm_line );

int _alarm_list_comparator( const void* a, const void* b );
/*int _alarm_list_seeker( const void* element, const void* indicator );*/

#endif

