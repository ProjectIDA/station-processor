#ifndef CSV_H
#define CSV_H

#include <buffer.h>
#include <falcon.h>
#include <fmash.h>
#include <alarm.h>
#include <simclist.h>

#define MAX_MATCHES 32

typedef list_t csv_context_t;

void poll_falcon( csv_context_t* csv_buffer_list, alarm_context_t* alarm_lines,
                  buffer_t* url_str, st_info_t* st_info) ;

void csv_archive( csv_context_t* csv_buffer_list, buffer_t* url_str,
                  st_info_t* st_info );
void csv_poll( csv_context_t* csv_buffer_list, buffer_t* url_str,
               st_info_t* st_info );
int csv_parse_file( csv_buffer_t* csv_buffer, buffer_t* buf );
int csv_get_file_list( csv_context_t *file_list, buffer_t* buf );
csv_context_t* csv_context_init();
csv_context_t* csv_context_destroy( csv_context_t* buffer_list );

/* internal list support functions */
int _file_list_seeker( const void* element, const void* indicator );
int _file_list_comparator( const void* a, const void* b );
int _buffer_list_seeker( const void* element, const void* indicator );
int _buffer_list_comparator( const void* a, const void* b );
int _csv_list_seeker( const void* element, const void* indicator );
int _csv_list_comparator( const void* a, const void* b );

#endif


