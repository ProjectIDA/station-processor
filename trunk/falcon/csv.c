#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>

#include <csv.h>
#include <format_data.h>
#include <get.h>
#include <murmur.h>

/* ===== CSV Context Handlers =========================== */
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Poll the Falcon for the latest data, and write it to 
 *   the diskloop.
 */
void poll_falcon( csv_context_t* csv_buffer_list, alarm_context_t* alarm_lines,
                  buffer_t* url_str, st_info_t* st_info, time_t initial_time)
{
    time_t start_time = 0x7fffffff;
    time_t end_time   = 0x80000000;
    csv_buffer_t* csv_buffer = NULL;

    if (initial_time) {
        start_time = initial_time;
        end_time = initial_time;
    }

    // Get the CSV data from the falcon
    csv_poll(csv_buffer_list, url_str, st_info, initial_time);

    // Set the time constraints for the alarm history
    list_iterator_stop(csv_buffer_list);
    list_iterator_start(csv_buffer_list);
    while (list_iterator_hasnext(csv_buffer_list)) 
    {
        csv_buffer = list_iterator_next(csv_buffer_list);
        if (csv_buffer->start_time < start_time)
            start_time = csv_buffer->start_time;
        if (csv_buffer->end_time > end_time)
            end_time = csv_buffer->end_time;
    }
    list_iterator_stop(csv_buffer_list);
    if ( start_time > end_time ) {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end timestamps\n");
        else
            syslog(LOG_ERR, "falcon: mismatched start and end timestamps\n");
        return;
    }

    // Get the alarm history from the falcon
    alarm_poll(alarm_lines, start_time, end_time, url_str, st_info);

    // Archive the latest data
    csv_archive(csv_buffer_list, url_str, st_info);
    alarm_archive(alarm_lines, url_str, st_info);
}

void csv_archive( csv_context_t* csv_buffer_list, buffer_t* url_str,
                  st_info_t* st_info )
{
    csv_buffer_t* csv_buffer;
    csv_row_t* csv_row;
    uint8_t* msh_data;
    size_t msh_length = 0;
    csv_buffer_t* final_csv = NULL;

    // Step through the csv buffers list
    list_iterator_stop(csv_buffer_list);
    list_iterator_start(csv_buffer_list);
    while (list_iterator_hasnext(csv_buffer_list)) 
    {
        // Grab one buffer from the list
        csv_buffer = list_iterator_next(csv_buffer_list);
        while ((csv_buffer->end_time - csv_buffer->start_time) >= TM_HOUR) 
        {
            // Compress the csv data to FMash format
            fmash_csv_to_msh(csv_buffer, &msh_data, &msh_length);
            if (gDebug) {
                // Brag about our accomplishments
                fprintf(stdout, "compressed data size is %lu bytes\n", (unsigned long)msh_length);
                format_data(msh_data, msh_length, 0, 0);

                //* Verify the contents of the buffer are correct...
                if (!fmash_msh_to_csv( &final_csv, msh_data, msh_length )) {
                    fprintf(stdout, "FMash to CSV conversion failed\n");
                    goto queue_it;
                }
                if (!final_csv) {
                    fprintf(stdout, "final_csv not created\n");
                    goto queue_it;
                }
                if (!final_csv->header) {
                    fprintf(stdout, "final_csv->header not created\n");
                    goto queue_it;
                }
                if (!final_csv->list) {
                    fprintf(stdout, "final_csv->list not created\n");
                    goto queue_it;
                }
                fprintf(stdout, "=== FMASH VERIFICATION =================================\n");
                fprintf(stdout, "    file:         %s\n", csv_buffer->file_name);
                fprintf(stdout, "    channel:      %d\n", csv_buffer->header->channel);
                fprintf(stdout, "    description:  %s\n", csv_buffer->header->description);
                fprintf(stdout, "    lines:        %d\n", list_size(final_csv->list));
                list_iterator_stop(final_csv->list);
                list_iterator_start(final_csv->list);
                    fprintf(stdout, "        ------------- ----------- ----------- ----------- \n");
                    fprintf(stdout, "       | Timestamp   | Average   | High      | Low       |\n");
                    fprintf(stdout, "        ------------- ----------- ----------- ----------- \n");
                while (list_iterator_hasnext(final_csv->list))
                {
                    csv_row = list_iterator_next(final_csv->list);
                    fprintf(stdout, "       | % 10d | % 9d | % 9d | % 9d |\n", (int)csv_row->timestamp,
                           csv_row->average, csv_row->high, csv_row->low );
                }
                printf("\n");
                list_iterator_stop(final_csv->list);
                final_csv = csv_buffer_destroy(final_csv);
                fflush(stdout);
                // */
            }
 queue_it:
            // Add this as an opaque record
            if (msh_data) {
                QueueOpaque(msh_data, msh_length, st_info->station,
                            st_info->network, st_info->channel,
                            st_info->location, FALCON_IDSTRING);
                free(msh_data);
                msh_data = NULL;
            }
        }
        csv_buffer = NULL;
    }
    list_iterator_stop(csv_buffer_list);

    // Ensure all opaque blockettes have been sent
    FlushOpaque();

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Update the csv context with the latest contents from the
 *   Falcon. For each buffer that has at least on hour worth of
 *   data, compress and write the data to the diskloop.
 */
void csv_poll( csv_context_t* csv_buffer_list, buffer_t* url_str,
               st_info_t* st_info, time_t initial_time )
{
    list_t* file_list = NULL;
    buffer_t* buf = NULL;
    buffer_t* url = NULL;
    char* file_name = NULL;
    const char* path = "/data/minute";
    csv_buffer_t csv_tmp;
    csv_buffer_t* csv_buffer;
    int location = 0;
    uint64_t file_hash = 0LL;

    uint8_t buffer_found = 0;

    int tally = 0;

    // Build the CSV directory URL
    url = buffer_init();
    buffer_write(url, url_str->content, url_str->length);
    buffer_write(url, (uint8_t*)path, strlen(path));
    buffer_terminate(url);

    // Initialize the CSV file list
    file_list = (list_t*)malloc(sizeof(list_t));
    if (!file_list)
        goto clean;
    list_init(file_list);
    list_attributes_seeker( file_list,  _file_list_seeker );
    list_attributes_comparator( file_list,  _file_list_comparator );

    // Get the html page listing the available CSV files
    buf = buffer_init();
    get_page((char*)url->content, buf);

    // Generate a list of files from the page
    if (!csv_get_file_list(file_list, buf))
        goto clean;
    buffer_reset(buf);
    buffer_reset(url);

    // Step through each CSV file and update its csv_buffer
    // in the csv_buffer_list
    while (!list_empty(file_list)) 
    {
        tally++;
        file_name = (char*)list_fetch(file_list);
        memset(&csv_tmp, 0, sizeof(csv_tmp));
        csv_tmp.file_name = file_name;

        // If there is not a csv buffer for this csv file, create a
        // new buffer and add it to the list
        if ((location = list_locate(csv_buffer_list, &csv_tmp)) < 0)
        {
            csv_buffer = csv_buffer_init();
            csv_buffer->file_name = file_name;
            list_append(csv_buffer_list, csv_buffer);
            buffer_found = 0;
        // Otherwise re-use the old csv buffer
        } else { 
            csv_buffer = (csv_buffer_t*)list_get_at(csv_buffer_list, location);
            buffer_found = 1;
        }

     // Process the contents of this CSV file
        // Generate the URL for retrieving the file
        buffer_write(url, url_str->content, url_str->length);
        buffer_write(url, (uint8_t*)csv_buffer->file_name,
                     strlen(csv_buffer->file_name));
        buffer_terminate(url);
        if (gDebug) {
            printf("getting page %s\n", url->content);
        }
        // Download the file
        get_page((char*)url->content, buf);
        file_hash = murmur_64_b( buf->content, buf->length, HASH_SEED_64 );
        if (gDebug) {
            printf("file '%s' [0x%016llx] uncompressed size is %lu bytes\n",
                   csv_buffer->file_name, (unsigned long long)file_hash,
                   (unsigned long)buf->length);
        }
        // Populate a csv_buffer with the contents of the file
        csv_parse_file(csv_buffer, buf, initial_time);
        if (gDebug) {
            printf("The CSV buffer contains %lu rows\n", csv_buffer->list->numels);
        }
        // Empty our temporary buffers
        buffer_reset(buf);
        buffer_reset(url);
        if (buffer_found) {
            free(file_name);
        }
        file_name = NULL;
        csv_buffer = NULL;
    }

// Clean up all temporary resources
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Given the contents of a CSV file, populate/update a
 *  csv_buffer_t structure.
 */
int csv_parse_file( csv_buffer_t* csv_buffer, buffer_t* buf, time_t initial_time )
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

    // Find the channel in the CSV file
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

    // Find the description in the CSV file
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
    if (csv_header->description)
    {
        free(csv_header->description);
    }
    csv_header->description = (char *)buffer_detach(description);
    description = buffer_destroy(description);
    content_string[match_list[1].rm_eo] = tmp_char;
    regfree(&regex);

    /* build regex for parsing CSV file lines */
    if (regcomp(&regex, "^([0-9]{2}/[0-9]{2}/[0-9]{2},[0-9]{1,2}:[0-9]{2}),([0-9-]+),([0-9-]+),([0-9-]+)$", REG_EXTENDED | REG_NEWLINE)) 
    {
        goto unclean;
    }

    // Locate and break down CSV file lines
    while (!regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) 
    {
        // Null terminate to simplify 
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

        // Parse the timestamp in this row
        memset(&time_struct, 0, sizeof(struct tm));
        strptime(timestamp, "%m/%d/%y,%H:%M", &time_struct);
        csv_row->timestamp = mktime(&time_struct);
        csv_row->average   = (int32_t)atol(average);
        csv_row->high      = (int32_t)atol(high);
        csv_row->low       = (int32_t)atol(low);
        csv_row->empty     = 0;

        // Make sure we don't duplicate lines
        if ( (csv_buffer->end_time < csv_row->timestamp) &&
             ((!initial_time) || (csv_row->timestamp > initial_time)) )
        {
            if (csv_buffer->start_time == 0) 
            {
                csv_buffer->start_time = csv_row->timestamp;
            }
            // Add a new row to the list
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Parse Falcon's webpage containing the list of 1-minute
 *  resolution CSV files and populate file_list with the
 *  names of these files.
 */
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
        // Find the names of all the csv files with minute resolution
        while (!regexec(&regex, content_string, (size_t)MAX_MATCHES, match_list, 0)) 
        {
            match = match_list;
            match++;
            max_off = 0;
            for (i = 1; i < MAX_MATCHES; i++, match++) 
            {
                if (match->rm_so && (match->rm_eo > match->rm_so)) 
                {
                    // Add this file name to the list
                    buffer_write(file_name, (uint8_t*)(content_string + match->rm_so),
                                 (size_t)(match->rm_eo - match->rm_so) );
                    buffer_terminate(file_name);
                    if (gDebug) {
                        printf("found CSV file: %s\n", file_name->content);
                    }
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


/* * * * * * * * * * * * * * * * * * * * * *
 * Simclist comparator and seeker functions
 */
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
    if ( !a && !b )
        return 0;
    else if ( a && !b ) 
        return -1;
    else if ( !a && b )
        return 1;
    return strcmp( ((csv_buffer_t*)b)->file_name, ((csv_buffer_t*)a)->file_name );
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
        if ( ((csv_row_t*)a)->timestamp && !(((csv_row_t*)b)->timestamp))
            return 1;
        else if ( !(((csv_row_t*)a)->timestamp) && ((csv_row_t*)b)->timestamp)
            return -1;
        else if (((csv_row_t*)a)->timestamp < ((csv_row_t*)b)->timestamp)
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

