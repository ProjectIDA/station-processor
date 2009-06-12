#ifndef ALARMS_H
#define ALARMS_H

#include <buffer.h>
#include <falcon.h>
#include <falcon_structures.h>
#include <simclist.h>

#include <arpa/inet.h>

void alarm_archive( alarm_context_t* alarm_list, buffer_t* url_str,
                    st_info_t* st_info );
void alarm_poll( alarm_context_t* alarm_list, buffer_t* url_str,
                 st_info_t* st_info, time_t last_alarm_time );
int alarm_filter_lines( alarm_context_t* alarm_list, buffer_t* buf,
                        time_t last_alarm_time );

#endif

