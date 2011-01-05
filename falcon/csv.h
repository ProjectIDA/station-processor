#ifndef CSV_H
#define CSV_H

#include <alarm.h>
#include <buffer.h>
#include <falcon.h>
#include <falcon_structures.h>
#include <fmash.h>
#include <simclist.h>

void csv_archive( csv_context_t* csv_buffer_list, buffer_t* url_str,
                  st_info_t* st_info );
void csv_poll( csv_context_t* csv_buffer_list, buffer_t* url_str,
               st_info_t* st_info, time_t initial_time );
int csv_parse_file( csv_buffer_t* csv_buffer, buffer_t* buf, time_t initial_time );
int csv_get_file_list( csv_context_t *file_list, buffer_t* buf );

/* internal list support functions */
int _file_list_seeker( const void* element, const void* indicator );
int _file_list_comparator( const void* a, const void* b );
int _csv_list_seeker( const void* element, const void* indicator );
int _csv_list_comparator( const void* a, const void* b );

#endif

