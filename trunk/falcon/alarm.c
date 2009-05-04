#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include <alarm.h>
#include <falcon.h>
#include <get.h>
#include <murmur.h>

/* ===== Alarm Line Handlers ================================ */
alarm_line_t* alarm_line_init()
{
    alarm_line_t* alarm_line = NULL;
    alarm_line = (alarm_line_t*)calloc(1, sizeof(alarm_line_t));
    return alarm_line;
}

alarm_line_t* alarm_line_duplicate( alarm_line_t* alarm_line )
{
    alarm_line_t* new_line = NULL;
    if (alarm_line) {
        new_line = (alarm_line_t*)malloc(sizeof(alarm_line_t));
        if (new_line) {
            memcpy(new_line, alarm_line, sizeof(alarm_line_t));
        }
    }
    return new_line;
}

alarm_line_t* alarm_line_destroy( alarm_line_t* alarm_line )
{
    if (alarm_line) {
        if (alarm_line->text) {
            free(alarm_line->text);
        }
        free(alarm_line);
        alarm_line = NULL;
    }
    return alarm_line;
}

/* ===== Alarm Context Handlers =========================== */
alarm_context_t* alarm_context_init()
{
    alarm_context_t* alarm_list = NULL;
    alarm_list = (alarm_context_t*)calloc(1, sizeof(alarm_context_t));
    if (alarm_list) {
        if (list_init(alarm_list) == -1) {
            free(alarm_list);
            alarm_list = NULL;
        } else {
            list_attributes_comparator(alarm_list, _alarm_list_comparator);
        }
    }
    return alarm_list;   
}

alarm_context_t* alarm_context_destroy( alarm_context_t* alarm_list )
{
    alarm_line_t* line = NULL;
    if (alarm_list) {
        while (!list_empty(alarm_list)) {
            line = (alarm_line_t*)list_fetch(alarm_list);
            alarm_line_destroy(line);
        }
        list_destroy(alarm_list);
        free(alarm_list);
        alarm_list = NULL;
    }
    return alarm_list;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Update the alarm list with the latest content from the
 *   Falcon. Make sure there are no duplicates, then write
 *   all new alarms to the diskloop.
 */
void alarm_poll( alarm_context_t* alarm_list, buffer_t* url_str, st_info_t* st_info )
{
    buffer_t* url = NULL;
    buffer_t* buf = NULL;
    alarm_line_t* alarm = NULL;
    const char* file_path = "/data/alarmhistory.txt";
    char* retmsg = NULL;

    // Set up the csv directory url
    url = buffer_init();
    buffer_write(url, url_str->content, url_str->length);
    buffer_write(url, (uint8_t*)file_path, strlen(file_path));
    buffer_terminate(url);

    // Get alarm file text
    buf = buffer_init();
    get_page((char*)url->content, buf);

    // Getting alarm lines
    alarm_filter_lines( alarm_list, buf );

    // Print each line in the filtered list
    list_iterator_stop(alarm_list);
    list_iterator_start(alarm_list);
    while (list_iterator_hasnext(alarm_list)) 
    {
        alarm = (alarm_line_t*)list_iterator_next(alarm_list);
        if (alarm->sent)
            continue;

        if (gDebug)
        {
            fprintf(stdout, "DEBUG %s, line %d, date %s: %s\n", 
                    __FILE__, __LINE__, __DATE__, alarm->text);
        }

        if ((retmsg = q330LogMsg(alarm->text, st_info->station,
                                 st_info->network, "LOG",
                                 st_info->location)) != NULL)
        {
            // error trying to log the message
            if (gDebug)
                fprintf(stderr, "falcon: %s\n", retmsg);
            else
                syslog(LOG_ERR, "falcon: %s\n", retmsg);
            exit(1);
        }
        alarm->sent = 1;
    }
    list_iterator_stop(alarm_list);

    // Make sure we don't continuously accumulate alarm messages
    while (list_size(alarm_list) > MAX_CONTEXT_ALARMS)
    {
        alarm = list_fetch(alarm_list);
        alarm = alarm_line_destroy(alarm);
    }

    url = buffer_destroy(url);
    buf = buffer_destroy(buf);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Strip out any lines that are internal Falcon issues, append
 *  the remaining lines to the alarm context list.
 */
int alarm_filter_lines( alarm_context_t* alarm_list, buffer_t* buf )
{
    int result = 1;

    regex_t regex;
    regmatch_t match_list[MAX_MATCHES];
    regmatch_t* match = NULL;
    buffer_t* line = NULL;
    alarm_line_t* line_element = NULL;
    char* content_string = NULL;

    if (buffer_size(buf) && alarm_list) 
    {
        // Construct the regular expression for filtering alarm messages
        if (regcomp(&regex, "AH([0-9]{3})[-]([0-9]{4})[-]([^ ]+)[ ]*[-]([0-9]{2}[/][0-9]{2}[/][0-9]{2} [0-9]{2}[:][0-9]{2}[:][0-9]{2})[ ]+([^\n\r]+)", REG_EXTENDED | REG_NEWLINE))
        {
            goto unclean;
        }

        memset(&match_list, 0, sizeof(match_list));
        content_string = (char*)buf->content;
        // Only process lines we are interested in
        while (!regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) 
        {
            match = match_list;
            if (match->rm_so && (match->rm_eo > match->rm_so)) 
            {
                line_element = (alarm_line_t*)calloc(sizeof(alarm_line_t), 1);
                if (!line_element) {
                    goto unclean;
                }
                line = buffer_init();
                buffer_write(line, (uint8_t*)(content_string + match->rm_so),
                                   (size_t)(match->rm_eo - match->rm_so));
                buffer_terminate(line);
                line_element->text = (char*)buffer_detach(line);
                line = buffer_destroy(line);
                line_element->hash = murmur_32(line_element->text, 
                                               strlen(line_element->text),
                                               HASH_SEED_32);
                // If this is a duplicate message, throw it out
                if (list_locate(alarm_list, line_element) > -1) {
                    free(line_element->text);
                    free(line_element);
                // Otherwise, add it to the list
                } else {
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
 clean:
    regfree(&regex);

    return result;
}

int _alarm_list_comparator( const void* a, const void* b )
{
    if (a && b && ((alarm_line_t*)a)->text && ((alarm_line_t*)b)->text) {
        return strcmp(((alarm_line_t*)b)->text, ((alarm_line_t*)a)->text);
    } else if (!a && b) {
        return 1;
    } else if (a && !b) {
        return -1;
    }
    return 0;
}

