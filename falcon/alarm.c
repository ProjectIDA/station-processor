#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>

#include <alarm.h>
#include <falcon.h>
#include <get.h>
#include <murmur.h>
#include <jio.h>
#include <format_data.h>

#include <include/archd.h>

/*
 * Alarm record(s) internal format
 * 
 * HEADER
 * ------
 * Version/Type : uint16_t ---- (2 bytes)
 * Start Time   : time_t ------ (4 bytes)
 * End Time     : time_t ------ (4 bytes)
 * Alarm Count  : uint16_t ---- (2 bytes)
 *
 * RECORD
 * ------
 * Channel ID   : uint16_t ---- (2 bytes)
 * Timestamp    : uint32_t ---- (4 bytes)
 * Event Type   : uint8_t ----- (1 byte)
 * Description  : pascal str. - (3-10 bytes)
 * 
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Write all new alarms to the diskloop.
 */
void alarm_archive( alarm_context_t* alarm_list, buffer_t* url_str,
                    st_info_t* st_info )
{
    time_t   start_time = 0;
    time_t   end_time   = 0;
    uint8_t  buf_byte   = 0;
    uint16_t buf_word   = 0;
    uint32_t buf_dword  = 0L;
    //uint64_t buf_qword  = 0LL;

    uint16_t  version_type = 0x8000 | FALCON_VERSION;
    uint16_t  alarm_count  = 0;
    char*     retmsg     = NULL;
    buffer_t* alarm_data = NULL;
    alarm_line_t* alarm  = NULL;

    alarm_data = buffer_init();

    if (!alarm_data) {
        if (gDebug)
            fprintf(stderr, "falcon: unable to allocate space for alarm data\n");
        else
            syslog(LOG_ERR, "falcon: unable to allocate space for alarm data\n");
        return;
    }

    // Print each line in the filtered list
    list_iterator_stop(alarm_list);
    list_iterator_start(alarm_list);
    while (list_iterator_hasnext(alarm_list)) 
    {
        alarm = (alarm_line_t*)list_iterator_next(alarm_list);
        if (alarm->sent)
            continue;

        if (!start_time)
          start_time = alarm->timestamp;
        if (!end_time)
          end_time = alarm->timestamp;

        if (gDebug)
            fprintf(stdout, "DEBUG %s, line %d, date %s: %s\n", 
                    __FILE__, __LINE__, __DATE__, alarm->text);

        if ((retmsg = q330LogMsg(alarm->text, st_info->station,
                                 st_info->network, "LOG",
                                 st_info->location)) != NULL)
        { // error trying to log the message
            if (gDebug)
                fprintf(stderr, "falcon: failed to write alarms to log: %s\n", retmsg);
            else
                syslog(LOG_ERR, "falcon: failed to write alarms to log: %s\n", retmsg);
            exit(1);
        }

        if (gDebug)
        {
            fprintf(stdout, "Alarm '%s':\n", alarm->description);
            fprintf(stdout, "    Channel    : %02x\n", (alarm->channel));
            fprintf(stdout, "    Timestamp  : %li\n", (long)(alarm->timestamp));
            fprintf(stdout, "    Event Code : 0x%02x\n", (alarm->event));
            fprintf(stdout, "    Sent       : %s\n", alarm->sent ? "Yes" : "No");
            fprintf(stdout, "    Hash       : 0x%08lx\n", (unsigned long)(alarm->hash));
            fprintf(stdout, "    Text       : %s\n", alarm->text);
        }

        if (start_time > alarm->timestamp)
            start_time = alarm->timestamp;
        if (end_time < alarm->timestamp)
            end_time = alarm->timestamp;

        if (!alarm->description[0])
        {
            if (gDebug)
                fprintf(stderr, "falcon: description code not found\n");
            else
                syslog(LOG_ERR, "falcon: description code not found\n");
            continue;
        }

        // There must be at least enough space for one more alarm.
        // If there isn't, queue this buffer's contents, and reset it.
        // This should prevent us from ever fragmenting alarm data
        // across opaque blockettes.
        if (alarm_count && (alarm_data->length > 400))
        {
            buffer_seek(alarm_data, 2);
            buf_dword = htonl(start_time);
            buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
            buf_dword = htonl(end_time);
            buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
            buf_word = htons(alarm_count);
            buffer_write(alarm_data, (uint8_t*)(&buf_word), sizeof(buf_word));

            if (gDebug) {
                fprintf(stdout, "falcon: [MID] alarms were found\n");
                fprintf(stdout, "[MID] raw buffer:\n");
                format_data(alarm_data->content, alarm_data->length, 0, 0);
            }

            QueueOpaque(alarm_data->content, (int)alarm_data->length,
                        st_info->station, st_info->network,
                        st_info->alarm_chan, st_info->location,
                        FALCON_IDSTRING);
            alarm_data  = buffer_destroy(alarm_data);
            alarm_count = 0;
            start_time  = alarm->timestamp;
            end_time    = alarm->timestamp;
            alarm_data  = buffer_init();
            if (!alarm_data) {
                if (gDebug)
                    fprintf(stderr, "falcon: unable to allocate space for alarm data\n");
                else
                    syslog(LOG_ERR, "falcon: unable to allocate space for alarm data\n");
                return;
            }
        } // If we've built up a records worth of alarm data

        if (!alarm_count)
        {
            // Write alarm header info
            if (gDebug)
            {
                fprintf(stderr, "falcon: Writing alarm header info.\n");
            }
            buf_word = htons(version_type);
            buffer_write(alarm_data, (uint8_t*)(&buf_word), sizeof(buf_word));
            // Reserve space for elements that will be assigned just 
            // prior to queueing data
            buf_dword = htonl(start_time);
            buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
            buf_dword = htonl(end_time);
            buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
            buf_word = htons(alarm_count);
            buffer_write(alarm_data, (uint8_t*)(&buf_word), sizeof(buf_word));
        }

        // Add an alarm
        buf_word = htons(alarm->channel);
        buffer_write(alarm_data, (uint8_t*)(&buf_word), sizeof(buf_word));
        buf_dword = htonl(alarm->timestamp);
        buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
        buffer_write(alarm_data, &(alarm->event), sizeof(alarm->event));
        buf_byte = (uint8_t)strlen(alarm->description);
        buffer_write(alarm_data, &buf_byte, sizeof(buf_byte));
        buffer_write(alarm_data, (uint8_t*)(alarm->description), (size_t)buf_byte);
        buffer_terminate(alarm_data);

        alarm_count++;
        alarm->sent = 1;
    }
    list_iterator_stop(alarm_list);

    if (alarm_count && alarm_data && alarm_data->content && alarm_data->length)
    {
        buffer_seek(alarm_data, 2);
        buf_dword = htonl(start_time);
        buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
        buf_dword = htonl(end_time);
        buffer_write(alarm_data, (uint8_t*)(&buf_dword), sizeof(buf_dword));
        buf_word = htons(alarm_count);
        buffer_write(alarm_data, (uint8_t*)(&buf_word), sizeof(buf_word));

        if (gDebug) {
            fprintf(stderr, "falcon: [END] alarms were found\n");
            fprintf(stderr, "[END] raw buffer:\n");
            format_data(alarm_data->content, alarm_data->length, 0, 0);
        }

        QueueOpaque(alarm_data->content, (int)alarm_data->length,
                    st_info->station, st_info->network,
                    st_info->alarm_chan, st_info->location,
                    FALCON_IDSTRING);
    }
    alarm_data = buffer_destroy(alarm_data);

    // Make sure we limit the accumulation of alarm messages
    while (list_size(alarm_list) > MAX_CONTEXT_ALARMS)
    {
        alarm = list_fetch(alarm_list);
        alarm = alarm_line_destroy(alarm);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Update the alarm list with the latest content from the
 *   Falcon. Make sure there are no duplicates.
 */
void alarm_poll( alarm_context_t* alarm_list, buffer_t* url_str,
                 st_info_t* st_info, time_t last_alarm_time )
{
    buffer_t* url = NULL;
    buffer_t* buf = NULL;
    const char* file_path = "/data/alarmhistory.txt";

    url = buffer_init();
    buffer_write(url, url_str->content, url_str->length);
    buffer_write(url, (uint8_t*)file_path, strlen(file_path));
    buffer_terminate(url);

    // Get alarm file text
    buf = buffer_init();
    get_page((char*)url->content, buf);

    // Getting alarm lines
    alarm_filter_lines( alarm_list, buf, last_alarm_time );

    // Re-order alarms from oldest to youngest
    list_sort(alarm_list, 1);

    // Cleanup
    url = buffer_destroy(url);
    buf = buffer_destroy(buf);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Strip out any lines that are internal Falcon issues, append
 *  the remaining lines to the alarm context list.
 */
int alarm_filter_lines( alarm_context_t* alarm_list, buffer_t* buf,
                        time_t last_alarm_time )
{
    int result = 1;

    regex_t regex;
    regmatch_t match_list[MAX_MATCHES];
    regmatch_t* match = NULL;
    regmatch_t* code_match = NULL;
    regmatch_t* desc_match = NULL;
    regmatch_t* evt_match  = NULL;
    regmatch_t* time_match = NULL;
    buffer_t* line = NULL;
    alarm_line_t* line_element = NULL;
    char* content_string = NULL;
    char tmp_char = '\0';
    size_t code_si = 0;
    size_t code_ei = 0;
    size_t desc_si = 0;
    size_t desc_ei = 0;
    size_t evt_si  = 0;
    size_t evt_ei  = 0;
    size_t time_si = 0;
    size_t time_ei = 0;
    struct tm time_struct;
    uint16_t code = 0;

    // Set up the csv directory url
    if (buffer_size(buf) && alarm_list) 
    {
        // Construct the regular expression for filtering alarm messages
        if (regcomp(&regex, "AH([0-9]{3})[-]([0-9]{4})[-]([^ ]+)[ ]*[-]([0-9]{2}[/][0-9]{2}[/][0-9]{2} [0-9]{2}[:][0-9]{2}[:][0-9]{2})[^:]*? ([^: ]+)([:][^\n\r]+)", REG_EXTENDED | REG_NEWLINE))
        {
            goto unclean;
        }

        memset(&match_list, 0, sizeof(match_list));
        content_string = (char*)buf->content;
        // Only process lines we are interested in
        while (!regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) 
        {
            match = match_list;
            code_match = &match_list[2];
            evt_match  = &match_list[3];
            time_match = &match_list[4];
            desc_match = &match_list[5];
            if (match->rm_so && (match->rm_eo > match->rm_so)) 
            {
                if (gDebug) printf("Parsing Alarm\n");

                line_element = alarm_line_init();
                if (!line_element)
                    goto unclean;
                    
                line = buffer_init();
                if (!line)
                    goto unclean;

                buffer_write(line, (uint8_t*)(content_string + match->rm_so),
                                   (size_t)(match->rm_eo - match->rm_so));
                buffer_terminate(line);
                line_element->text = (char*)buffer_detach(line);
                line = buffer_destroy(line);
                line_element->hash = murmur_32(line_element->text, 
                                               strlen(line_element->text),
                                               HASH_SEED_32);
                if (gDebug) printf("  Text: %s\n", line_element->text);

                // Parse time
                time_si = time_match->rm_so - match->rm_so;
                time_ei = time_match->rm_eo - match->rm_so;
                tmp_char = line_element->text[time_ei];
                line_element->text[time_ei] = '\0'; // temporarily terminate at end of time
                if (gDebug) printf("  Timestamp: %s\n", line_element->text + time_si);
                memset(&time_struct, 0, sizeof(struct tm));
                strptime((const char*)(line_element->text + time_si),
                         "%m/%d/%y %H:%M:%S", &time_struct);
                line_element->timestamp = mktime(&time_struct);
                line_element->text[time_ei] = tmp_char; // restore character

                // Get the event code
                code_si = code_match->rm_so - match->rm_so;
                code_ei = code_match->rm_eo - match->rm_so;
                tmp_char = line_element->text[code_ei];
                line_element->text[code_ei] = '\0';
                if (gDebug) printf("  Code: %s\n", line_element->text + code_si);
                code = atoi(line_element->text + code_si);
                line_element->channel = code / 10;
                line_element->event = code % 10;
                line_element->text[code_ei] = tmp_char;

                // Determine if the alarm is a trigger or return
                evt_si = evt_match->rm_so - match->rm_so;
                evt_ei = evt_match->rm_eo - match->rm_so;
                tmp_char = line_element->text[evt_ei];
                line_element->text[evt_ei] = '\0';
                if (gDebug) printf("  Event: %s\n", line_element->text + evt_si);
                if (!strcmp("RTN", line_element->text + evt_si)) {
                    line_element->event |= 0x80;
                }
                line_element->text[evt_ei] = tmp_char;

                // Record the description code
                desc_si = desc_match->rm_so - match->rm_so;
                desc_ei = desc_match->rm_eo - match->rm_so;
                tmp_char = line_element->text[desc_ei];
                line_element->text[desc_ei] = '\0';
                if (gDebug) printf("  Description: %s\n", line_element->text + desc_si);
                strncpy(line_element->description, line_element->text + desc_si, 8);
                line_element->description[8] = '\0';
                line_element->text[desc_ei] = tmp_char;

                // If this is a duplicate message, throw it out
                if ((line_element->timestamp <= last_alarm_time) ||
                    (list_locate(alarm_list, line_element) > -1))
                {
                    free(line_element->text);
                    free(line_element);
                }
                // Otherwise, add it to the list
                else
                {
                    list_append(alarm_list, line_element);
                    if ( list_size(alarm_list) > MAX_ALARMS )
                    {
                        line_element = list_fetch(alarm_list);
                        line_element = alarm_line_destroy(line_element);
                    }
                }
            } 
            content_string += match->rm_eo;
            memset(&match_list, 0, sizeof(match_list));
        }
    }

    goto clean;
 unclean:
    result = 0;
    line_element = alarm_line_destroy(line_element);
    line = buffer_destroy(line);

 clean:
    regfree(&regex);

    return result;
} // alarm_filter_lines()

