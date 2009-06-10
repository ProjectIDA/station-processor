#ifndef ALARMS_H
#define ALARMS_H

#include <buffer.h>
#include <falcon.h>
#include <falcon_structures.h>
#include <simclist.h>

#include <arpa/inet.h>

void alarm_archive( alarm_context_t* alarm_list, buffer_t* url_str,
                    st_info_t* st_info, time_t start_time );
void alarm_poll( alarm_context_t* alarm_list, time_t start_time, time_t end_time,
                 buffer_t* url_str, st_info_t* st_info );
int alarm_filter_lines( alarm_context_t* alarm_list, buffer_t* buf,
                        time_t start_time, time_t end_time );

#endif

