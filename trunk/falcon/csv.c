#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <csv.h>
#include <format_data.h>
#include <get.h>

csv_context_t* csv_context_init()
{
    csv_context_t* csv_buffer_list = NULL;
    csv_buffer_list = (csv_context_t*)malloc(sizeof(csv_context_t));
    if (csv_buffer_list) {
        if (list_init(csv_buffer_list) == -1) {
            free(csv_buffer_list);
            csv_buffer_list = NULL;
        } else {
            list_attributes_seeker( csv_buffer_list,  _buffer_list_seeker );
            list_attributes_comparator( csv_buffer_list,  _buffer_list_comparator );
        }
    }
    return csv_buffer_list;   
}

csv_context_t* csv_context_destroy( csv_context_t* csv_buffer_list )
{
    csv_buffer_t* csv_buffer = NULL;
    if (csv_buffer_list) {
        while (!list_empty(csv_buffer_list)) {
            csv_buffer = list_fetch(csv_buffer_list);
            csv_buffer = csv_buffer_destroy(csv_buffer);
        }
        list_destroy(csv_buffer_list);
        free(csv_buffer_list);
        csv_buffer_list = NULL;
    }
    return csv_buffer_list;
}

void csv_poll_channels( csv_context_t* csv_buffer_list, buffer_t* url_str,
                        st_info_t* st_info )
{
    list_t* file_list = NULL;
    buffer_t* buf = NULL;
    buffer_t* url = NULL;
    char* file_name = NULL;
    const char* path = "/data/minute";
    csv_buffer_t csv_tmp;
    csv_buffer_t* csv_buffer;
    csv_row_t* csv_row;
    uint8_t* msh_data;
    size_t msh_length = 0;
    int location = 0;

    uint8_t buffer_exists = 0;
    csv_buffer_t* final_csv = NULL;;
    void* tmp = NULL;

    /* set up the csv directory url */
    url = buffer_init();
    buffer_write(url, url_str->content, url_str->length);
    buffer_write(url, (uint8_t*)path, strlen(path));
    buffer_terminate(url);

    /* initialize file list */
    file_list = (list_t*)malloc(sizeof(list_t));
    if (!file_list)
        goto clean;
    list_init(file_list);
    list_attributes_seeker( file_list,  _file_list_seeker );
    list_attributes_comparator( file_list,  _file_list_comparator );

    /* get the html page listing the available CSV files */
    buf = buffer_init();
    get_page((char*)url->content, buf);

    /* generate a list of files from the page */
    if (!csv_get_file_list(file_list, buf))
        goto clean;
    buffer_reset(buf);
    buffer_reset(url);

    /* step through each CSV file */
    while (!list_empty(file_list)) 
    {
        file_name = (char*)list_fetch(file_list);
        memset(&csv_tmp, 0, sizeof(csv_tmp));
        csv_tmp.file_name = file_name;

        /* if there is not a buffer for this csv file, add a new buffer */
        if ((location = list_locate(csv_buffer_list, &csv_tmp) < 0))
        {
            csv_buffer = csv_buffer_init();
            csv_buffer->file_name = file_name;
            list_append(csv_buffer_list, csv_buffer);
            buffer_exists = 1;
        } else { 
            csv_buffer = list_get_at(csv_buffer_list, location);
            buffer_exists = 0;
        }

        /* process the contents of this csv file */
        buffer_write(url, url_str->content, url_str->length);
        buffer_write(url, (uint8_t*)file_name, strlen(file_name));
        buffer_terminate(url);
        get_page((char*)url->content, buf);
        printf("file '%s' [0x%08lx] uncompressed size is %lu bytes\n", file_name, (unsigned long)file_name, (unsigned long)buf->length);
        csv_parse_file(csv_buffer, buf);
        buffer_reset(buf);
        buffer_reset(url);
        if (!buffer_exists) {
            free(file_name);
        }
    }

    /* regurgitate (table printout of csv file contents for debugging) */

    /*
    if (gDebug)
    {
        printf("CSV Files (%d):\n", list_size(csv_buffer_list));
        list_iterator_stop(csv_buffer_list);
        list_iterator_start(csv_buffer_list);
        while (list_iterator_hasnext(csv_buffer_list)) {
            csv_buffer = list_iterator_next(csv_buffer_list);

            printf("=== PRE COMPRESSION ====================================\n");
            printf("    file:         %s\n", csv_buffer->file_name);
            printf("    channel:      %d\n", csv_buffer->header->channel);
            printf("    description:  %s\n", csv_buffer->header->description);
            printf("    lines:        %d\n", list_size(csv_buffer->list));
            list_iterator_stop(csv_buffer->list);
            list_iterator_start(csv_buffer->list);
                printf("        ------------- ----------- ----------- ----------- \n");
                printf("       | Timestamp   | Average   | High      | Low       |\n");
                printf("        ------------- ----------- ----------- ----------- \n");
            while (list_iterator_hasnext(csv_buffer->list)) {
                csv_row = list_iterator_next(csv_buffer->list);
                printf("       | % 10d | % 9d | % 9d | % 9d |\n", (int)csv_row->timestamp,
                       csv_row->average, csv_row->high, csv_row->low );
            }
            list_iterator_stop(csv_buffer->list);

            if (!fmash_csv_to_msh( csv_buffer, &msh_data, &msh_length ))
            {
                goto loop_clean;
            }

            if (!fmash_msh_to_csv( &final_csv, msh_data, msh_length ))
            {
                goto loop_clean;
            }

            if (!final_csv)
            {
                goto loop_clean;
            }
            if (!final_csv->header)
            {
                goto loop_clean;
            }
            if (!final_csv->list)
            {
                goto loop_clean;
            }

            printf("=== POST EXPANSION =====================================\n");
            printf("    file:         %s\n", csv_buffer->file_name);
            printf("    channel:      %d\n", csv_buffer->header->channel);
            printf("    description:  %s\n", csv_buffer->header->description);
            printf("    lines:        %d\n", list_size(final_csv->list));
            list_iterator_stop(final_csv->list);
            list_iterator_start(final_csv->list);
                printf("        ------------- ----------- ----------- ----------- \n");
                printf("       | Timestamp   | Average   | High      | Low       |\n");
                printf("        ------------- ----------- ----------- ----------- \n");
            while (list_iterator_hasnext(final_csv->list))
            {
                csv_row = list_iterator_next(final_csv->list);
                printf("       | % 10d | % 9d | % 9d | % 9d |\n", (int)csv_row->timestamp,
                       csv_row->average, csv_row->high, csv_row->low );
            }
            printf("\n");
            list_iterator_stop(final_csv->list);

     loop_clean:
            csv_buffer_destroy(final_csv);
            if (msh_data)
            {
                free(msh_data);
                msh_data = NULL;
            }
            msh_length = 0;
            tmp = NULL;

        }
        list_iterator_stop(csv_buffer_list);
    }
    // */

    list_iterator_stop(csv_buffer_list);
    list_iterator_start(csv_buffer_list);
    while (list_iterator_hasnext(csv_buffer_list)) 
    {
        csv_buffer = list_iterator_next(csv_buffer_list);
        while ((csv_buffer->end_time - csv_buffer->start_time) >= 3600) 
        {
            fmash_csv_to_msh(csv_buffer, &msh_data, &msh_length);
            if (gDebug) {
                printf("file '%s' compressed size is %lu bytes\n", csv_buffer->file_name, (unsigned long)msh_length);
                format_data(msh_data, msh_length, 0, 0);
            }
            QueueOpaque(msh_data, msh_length, st_info->station,
                        st_info->network, st_info->channel,
                        st_info->location, FALCON_IDSTRING);
        }
    }
    list_iterator_stop(csv_buffer_list);
    FlushOpaque();
    // */

clean:
    buf = buffer_destroy(buf);
    url = buffer_destroy(url);
    if (file_list) 
    {
        while(!list_empty(file_list))
        {
            file_name = list_fetch(file_list);
            if (file_name)
            {
                free(file_name);
            }
        }
        list_destroy(file_list);
        free(file_list);
    }
}

int csv_parse_file( csv_buffer_t* csv_buffer, buffer_t* buf )
{
    int result = 1;
    regex_t regex;
    regmatch_t match_list[MAX_MATCHES];
    regmatch_t* match = NULL;
    csv_row_t* csv_row = NULL;
    buffer_t* description = NULL;
    char* content_string = NULL;
    char* timestamp = NULL;
    char* average = NULL;
    char* high = NULL;
    char* low = NULL;
    struct tm time_struct;
    char tmp_char = '\0';
    csv_header_t* csv_header;
    csv_context_t* csv_list;

    if (!csv_buffer || !buf || !buf->content) {
        goto unclean;
    }

    csv_header = csv_buffer->header;
    csv_list = csv_buffer->list;

    description = buffer_init();
    content_string = (char *)buf->content;

    if (regcomp(&regex, "^Chan:,(.*)$", REG_EXTENDED | REG_NEWLINE)) {
        goto unclean;
    }
    if (regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) {
        goto unclean;
    }
    tmp_char = content_string[match_list[1].rm_eo];
    content_string[match_list[1].rm_eo] = '\0';
    csv_header->channel = atol(content_string + match_list[1].rm_so);
    content_string[match_list[1].rm_eo] = tmp_char;

    regfree(&regex);
    /* extract the description from the CSV file */
    if (regcomp(&regex, "^Desc:,([^:\r\n)]+)[:](.*)$", REG_EXTENDED | REG_NEWLINE)) {
        goto unclean;
    }
    if (regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) {
        regfree(&regex);
        if (regcomp(&regex, "^Desc:,([^\r\n]*)$", REG_EXTENDED | REG_NEWLINE)) {
            goto unclean;
        }
        if (regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) {
            goto unclean;
        }
    }
    buffer_write(description, (uint8_t*)(content_string + match_list[1].rm_so),
                 match_list[1].rm_eo - match_list[1].rm_so);
    buffer_terminate(description);
    tmp_char = content_string[match_list[1].rm_eo];
    content_string[match_list[1].rm_eo] = '\0';
    /* The following description never gets freed! */
    if (csv_header->description)
    {
        free(csv_header->description);
    }
    csv_header->description = (char *)buffer_detach(description);
    description = buffer_destroy(description);
    content_string[match_list[1].rm_eo] = tmp_char;

    /* build regex for parsing CSV file lines */
    regfree(&regex);
    if (regcomp(&regex, "^([0-9]{2}/[0-9]{2}/[0-9]{2},[0-9]{1,2}:[0-9]{2}),([0-9-]+),([0-9-]+),([0-9-]+)$", REG_EXTENDED | REG_NEWLINE)) 
    {
        goto unclean;
    }

    /* locate and break down CSV file lines */
    while (!regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) 
    {
        match = match_list; match++;
        timestamp = content_string + match->rm_so;
        content_string[match->rm_eo] = '\0'; match++;
        average   = content_string + match->rm_so;
        content_string[match->rm_eo] = '\0'; match++;
        high      = content_string + match->rm_so;
        content_string[match->rm_eo] = '\0'; match++;
        low       = content_string + match->rm_so;
        content_string[match->rm_eo] = '\0';
        content_string += match->rm_eo + 1;

        csv_row = csv_row_init();
        if (!csv_row) 
        {
            fprintf(stderr, "Could not allocate memory for csv row.\n");
            goto unclean;
        }

        memset(&time_struct, 0, sizeof(struct tm));
        strptime(timestamp, "%m/%d/%y,%H:%M", &time_struct);
        csv_row->timestamp = mktime(&time_struct);
        csv_row->average   = (int32_t)atol(average);
        csv_row->high      = (int32_t)atol(high);
        csv_row->low       = (int32_t)atol(low);
        csv_row->empty     = 0;

        /* make sure we don't duplicate lines */
        if (csv_buffer->end_time < csv_row->timestamp) 
        {
            if (csv_buffer->start_time == 0) 
            {
                csv_buffer->start_time = csv_row->timestamp;
            }
            /* add new row to the list */
            list_append(csv_list, csv_row);
            csv_buffer->end_time = csv_row->timestamp;
        } 
        else 
        {
            csv_row = csv_row_destroy(csv_row);
        }
        
        memset(&match_list, 0, sizeof(match_list));
    }

    goto clean;
 unclean:
    result = 0;
 clean:
    regfree(&regex);
    description = buffer_destroy(description);
    return result;
}

int csv_get_file_list( csv_context_t *file_list, buffer_t* buf )
{
    int result = 1;
    int i = 0;
    regex_t regex;
    regmatch_t match_list[MAX_MATCHES];
    regmatch_t* match = NULL;
    buffer_t* file_name = NULL;
    char* content_string = NULL;
    size_t max_off = 0;

    if (buffer_size(buf)) 
    {
        file_name = buffer_init();
        memset(&match_list, 0, sizeof(match_list));
        content_string = (char*)buf->content;

        if (regcomp(&regex, "HREF[=]\"(/data/[^\"]+?[.]csv)\"", REG_EXTENDED))
        {
            goto unclean;
        }
        /* find the names of all the csv files with minute resolution */
        while (!regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) 
        {
            match = match_list;
            match++;
            max_off = 0;
            for (i = 1; i < MAX_MATCHES; i++, match++) 
            {
                if (match->rm_so && (match->rm_eo > match->rm_so)) 
                {
                    buffer_write(file_name, (uint8_t*)(content_string + match->rm_so),
                                 (size_t)(match->rm_eo - match->rm_so) );
                    buffer_terminate(file_name);
                    list_append(file_list, buffer_detach(file_name));
                    if (max_off < match->rm_eo)
                    {
                        max_off = match->rm_eo;
                    }
                }
            }
            content_string += max_off;
            memset(&match_list, 0, sizeof(match_list));
        }
    }
    goto clean;
 unclean:
    result = 0;
 clean:
    regfree(&regex);
    file_name = buffer_destroy(file_name);
    return result;
}

int _file_list_seeker( const void* element, const void* indicator )
{
    if ( element && indicator && !strcmp((char*)element, (char*)indicator))
        return 1;
    return 0;
}

int _file_list_comparator( const void* a, const void* b )
{
    if ( a && b )
        return strcmp((const char*)b, (const char*)a);
    else if ( a && !b ) 
        return -1;
    else if ( !a && b )
        return 1;
    return 0;
}

int _buffer_list_seeker( const void* element, const void* indicator )
{
    if ( element && indicator &&
         !strcmp(((csv_buffer_t*)element)->file_name,
                 ((csv_buffer_t*)indicator)->file_name) )
        return 1;
    return 0;
}

int _buffer_list_comparator( const void* a, const void* b )
{
    if ( a && b )
        return strcmp(((csv_buffer_t*)b)->file_name,
                      ((csv_buffer_t*)a)->file_name);
    else if ( a && !b ) 
        return -1;
    else if ( !a && b )
        return 1;
    return 0;
}

int _csv_list_seeker( const void* element, const void* indicator )
{
    if ( element && indicator &&
         (((csv_row_t*)element)->timestamp == ((csv_row_t*)indicator)->timestamp) )
        return 1;
    return 0;
}

int _csv_list_comparator( const void* a, const void* b )
{
    if ( a && b ) {
        if (((csv_row_t*)a)->timestamp < ((csv_row_t*)b)->timestamp)
            return 1;
        else if (((csv_row_t*)a)->timestamp > ((csv_row_t*)b)->timestamp)
            return -1;
    }
    else if ( !a && b )
        return 1;
    else if ( a && !b ) 
        return -1;
    return 0;
}

