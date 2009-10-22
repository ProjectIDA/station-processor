/*  ofadump.c
 *
 *  Author: Joel Edwards
 *          jdedwards@usgs.gov, joeledwards@gmail.com
 * 
 *  Produces raw and formatted output of a file containing
 *  opaque SEED records containing compressed statistical
 *  data from an RLE Technologies (R)  Falcon (C)
 */

#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <fmash.h>
#include <falcon_structures.h>
#include <format_data.h>
#include <lib330.h>
#include <jio.h>
#include <record.h>
#include <tokens.h>

void print_usage( char **args, char *msg, ... );
int verify_int( char* arg_str, int* value );

int gDebug = 1;
int gReport = 0;

int main ( int argc, char **argv )
{
    bool eof_reached = false;
    FILE *file_handle = NULL;
    char *file_name = NULL;
    long int file_position = 0;

    uint8_t *buffer = NULL;
    size_t records_read = 0;
    size_t data_length = 0;
    size_t i = 0;

    alarm_line_t *alarm = NULL;
    uint8_t *alarm_data = NULL;
    uint16_t alarm_count = 0;
    uint8_t  desc_len = 0;

    uint8_t *current_record = NULL; // The selected SEED record
    uint8_t *current_blockette = NULL; // The selected blockette
    uint8_t *current_data = NULL; // Location of data within this blockette

    uint8_t *messy_pointer = NULL;

    buffer_t* msh_data = NULL;
    seed_file_info file_info;
    seed_header header_seed; // SEED header struct
    topaque_hdr header_opaque; // Opaque blockette header struct
    char rec_type_str[REC_TYPE_MAX_LEN + 1];

    size_t record_size = 0;
    int16_t next_blockette;
    int16_t blockette_type;
    int16_t blockette_length;

    uint32_t netuint32;
    int c;
    bool data_complete = false;

    // Arguments
    bool display_info   = false;
    bool display_raw    = false;
    bool display_header = false;
    bool limiting       = false;
    int first_records   = 0;
    int last_records    = 0;
    int record_index    = 0;
    int record_limit    = 0;

    csv_buffer_t* csv_buffer = NULL;
    csv_row_t* csv_row = NULL;
    
    char time_string[32];
    uint16_t version_type = 0;

    // Ensure we received the file argument
    if ( argc < 2 ) {
        print_usage( argv, NULL );
        goto clean;
    }

    file_name = argv[argc - 1];

    while ((c = getopt( argc, argv, "fhi:l:rF:IL:" )) != -1) {
        switch (c) {
            case 'h':
                display_header = true;
                break;
            case 'i':
                if (!verify_int(optarg, &record_index)) {
                    print_usage( argv, "E: record index: invalid value '%s'", optarg );
                    goto clean;
                }
                break;
            case 'l':
                if (!verify_int(optarg, &record_limit)) {
                    print_usage( argv, "E: record limit: invalid value '%s'", optarg );
                    goto clean;
                }
                break;
            case 'r':
                display_raw = true;
                break;
            case 'F':
                if (!verify_int(optarg, &first_records)) {
                    print_usage( argv, "E: first records: invalid value '%s'", optarg );
                    goto clean;
                }
                break;
            case 'I':
                display_info = true;
                break;
            case 'L':
                if (!verify_int(optarg, &last_records)) {
                    print_usage( argv, "E: last records: invalid value '%s'", optarg );
                    goto clean;
                }
                break;
            case 'f':
                gReport = 1;
                break;
            default:
                fprintf( stderr, "Got Filename\n" );
                break;
        }
    }

    get_seed_file_info( file_name, &file_info, 0 );
    if (!file_info.good) {
        fprintf( stderr, "File '%s' is invalid.\n", file_name );
        goto clean;
    }
    record_size = file_info.record_size;

    if ( display_info ) {
        printf( "Summary for file '%s':\n", file_name );
        print_seed_time( "  Start Time   : ", &(file_info.start_time), "\n" );
        print_seed_time( "  End Time     : ", &(file_info.end_time), "\n" );
        printf( "  File Length  : %lu\n", file_info.length );
        printf( "  Record Size  : %lu\n", file_info.record_size );
        printf( "  Num. Records : %lu\n", (file_info.length / file_info.record_size) );
        goto clean;
    }

    buffer = malloc( (size_t)(BUFFER_SIZE) );
    if ( !buffer ) {
        fprintf( stderr, "Could not allocate memory for read buffer.\n" );
        goto clean;
    }

    file_handle = fopen( file_name, "rb" );
    if ( !file_handle ) {
        fprintf( stderr, "Unable to open file '%s' for reading.\n", file_name ); goto clean;
    }

    if ( record_index ) {
        if ( (record_index * file_info.record_size) >
             (file_info.length - file_info.record_size) ) {
            fprintf( stderr, "start record index is too large for this file\n" );
        }
        fseek( file_handle, record_index * record_size, SEEK_SET );
    }

    if ( first_records || last_records ) {
        limiting = true;
    }

    // Interpret header type, and break into opaque block types
    while ( !eof_reached ) {

        if (limiting && !record_limit) {
            if (first_records > 0) {
                first_records--;
            } else if (last_records > 0) {
                fseek( file_handle, file_info.length - (last_records * record_size), SEEK_SET );
                last_records--;
            } else {
                break;
            }
        }

        file_position = ftell( file_handle );
        if (!gReport)
          printf( "File position: %lu\n", file_position );
        records_read = fread( buffer, record_size, 1, file_handle );
        file_position = ftell( file_handle );

        if ( feof(file_handle) || ferror(file_handle) ) {
            if (!gReport)
              fprintf( stdout, "EOF reached\n" );
            eof_reached = true;
            if ( !records_read ) {
                if (!gReport)
                  fprintf( stdout, "No records left\n" );
                break;
            }
        }
        if ( display_raw ) {
            fprintf( stdout, "Raw Data:\n");
            format_data( buffer, records_read * record_size, 
                         (size_t)DUMP_BYTES, 
                         record_size / (size_t)DUMP_BYTES );
        }

        // Populate the header structs
        messy_pointer = current_record = buffer;
        loadseedhdr( (pbyte *)(&messy_pointer), &header_seed, false );
        next_blockette = header_seed.first_blockette_byte;
        blockette_type = 0;

        while ( next_blockette ) {
            current_blockette = buffer + next_blockette;
            blockette_type    = ntohs(*(int16_t *)current_blockette);
            next_blockette    = ntohs(*(int16_t *)(current_blockette + 2));
            if (blockette_type == 2000) {
                blockette_length = ntohs(*(int16_t *)(current_blockette + 4));
                messy_pointer = current_blockette;
                loadopaquehdr( (pbyte *)(&messy_pointer), &header_opaque );

                if ( display_header ) {
                    printf( "Found an opaque blockette:\n" );
                    jprintf( 1, "Blockette type: %lu\n", 
                            (unsigned long)header_opaque.blockette_type);
                    jprintf( 1, "Next blockette: %lu\n", 
                            (unsigned long)header_opaque.next_blockette);
                    jprintf( 1, "Blockette length: %lu\n", 
                            (unsigned long)header_opaque.blk_lth);
                    jprintf( 1, "Data offset: %lu\n", 
                            (unsigned long)header_opaque.data_off);
                    jprintf( 1, "Record number: %lu\n", 
                            (unsigned long)header_opaque.recnum);
                    jprintf( 1, "Data word order: %s\n", 
                            header_opaque.word_order ? "Big Endian" : "Little Endian" );
                    jprintf( 1, "Opaque data flags: 0x%02lx\n", 
                            (unsigned long)header_opaque.opaque_flags );

                    // null terminate this, otherwise it gets ugly
                    for (i = 0; i < REC_TYPE_MAX_LEN; i++) {
                        rec_type_str[i] = header_opaque.rec_type[i];
                        if ( header_opaque.rec_type[i] == '\0' )
                            break;
                        if ( header_opaque.rec_type[i] == '~') {
                            break;
                        }
                    }
                    rec_type_str[i + 1] = '\0';
                    jprintf( 1, "Record type: %s\n", rec_type_str);
                    jprintf( 1, "========== RECORD DATA ========== \n" );
                }

                current_data = current_blockette + (size_t)header_opaque.data_off;
                data_length = (size_t)header_opaque.blk_lth - (size_t)header_opaque.data_off;

                msh_data = buffer_init();
                if ( (header_opaque.opaque_flags & 0x0c) == 0 )
                { // No spanning
                    msh_data = buffer_init();
                    data_complete = 1;
                }
                else if ( (header_opaque.opaque_flags & 0x0c) == 1 )
                { // First opaque blockette for spanning
                    msh_data = buffer_init();
                }
                else if ( (header_opaque.opaque_flags & 0x0c) == 3 )
                { // Continuation blockette for spanning
                    ;
                }
                else if ( (header_opaque.opaque_flags & 0x0c) == 2 )
                { // Final blockette for spanning
                    data_complete = 1;
                }
                buffer_write(msh_data, current_data, data_length);

                if (msh_data && data_complete) {
                    version_type = (uint16_t)ntohs((*(uint16_t*)(msh_data->content)));
                    if ((version_type & 0x7fff) > FALCON_VERSION) {
                        fprintf(stderr, "Falcon data is too new!\n");
                        continue;
                    }
                    if (version_type & 0x8000)
                    { // This is Alarm data
                        alarm = alarm_line_init();
                        alarm_data = msh_data->content;
                        if (!gReport)
                        {
                          fprintf(stdout, "\ncompacted alarms size is %lu bytes\n", (unsigned long)msh_data->length);
                          format_data(alarm_data, msh_data->length, 0, 0);
                        }

                        alarm_data += sizeof(uint16_t);
                        alarm_data += sizeof(uint32_t); // Skip start time
                        alarm_data += sizeof(uint32_t); // Skip end time
                        alarm_count = ntohs(*(uint16_t*)(alarm_data));
                        alarm_data += sizeof(uint16_t);
                        if (!gReport)
                          printf(
"=== ALARMS (%d) =========================================\n", alarm_count);

                        for (i = 0; i < (int)alarm_count; i++) {
                            alarm->channel = ntohs(*(uint16_t*)(alarm_data));
                            alarm_data += sizeof(uint16_t);
                            if (!gReport)
                              printf("current alarm_data pointer : 0x%08x\n",
                                 (uint32_t)alarm_data);
                            memcpy(&netuint32, alarm_data, 4);
                            if (!gReport)
                              printf("current alarm_time value   : 0x%08x\n",
                                 ntohl(netuint32));
                            alarm->timestamp = (time_t)ntohl(netuint32);
                            alarm_data += sizeof(uint32_t);
                            alarm->event = *(uint8_t*)(alarm_data);
                            alarm_data += sizeof(uint8_t);
                            desc_len = *(uint8_t*)(alarm_data);
                            alarm_data += sizeof(uint8_t);
                            strncpy(alarm->description, (char*)alarm_data, (size_t)desc_len);
                            alarm_data += desc_len + 1;
//                            alarm->description[8] = '\0';
                            alarm->description[desc_len] = '\0';

                            strftime(time_string, 31, "%Y/%m/%d %H:%M:%S", gmtime(&(alarm->timestamp)));

                            if (gReport)
                              printf( "%s <%s> (%u): Alarm event %s %s\n",
                                    time_string,
                                    alarm->description, alarm->channel,
                                    (alarm->event & 0x7f) == 0 ? "'On'"    :
                                    (alarm->event & 0x7f) == 1 ? "'High1'" :
                                    (alarm->event & 0x7f) == 2 ? "'Low1'"  :
                                    (alarm->event & 0x7f) == 3 ? "'High2'" :
                                    (alarm->event & 0x7f) == 4 ? "'Low2'"  :
                                                                 "'Unknown'",
                                    alarm->event & 0x80 ? "restored" : "triggered" );
                            else
                              printf( "    (%s)[%li]> %s (%u): Alarm event %s %s\n",
                                    time_string, (long)(alarm->timestamp),
                                    alarm->description, alarm->channel,
                                    (alarm->event & 0x7f) == 0 ? "'On'"    :
                                    (alarm->event & 0x7f) == 1 ? "'High1'" :
                                    (alarm->event & 0x7f) == 2 ? "'Low1'"  :
                                    (alarm->event & 0x7f) == 3 ? "'High2'" :
                                    (alarm->event & 0x7f) == 4 ? "'Low2'"  :
                                                                 "'Unknown'",
                                    alarm->event & 0x80 ? "restored" : "triggered" );
                        }
                        alarm = alarm_line_destroy(alarm);
                    }
                    else
                    { // This is FMash data
                        data_complete = 0;

                        fprintf(stdout, "\ncompressed (fmash) data size is %lu bytes\n", (unsigned long)msh_data->length);
                        format_data(msh_data->content, msh_data->length, 0, 0);

                        fmash_msh_to_csv(&csv_buffer, msh_data->content, msh_data->length);
                        printf("=== POST EXPANSION ======================================\n");
                        printf("    file:         %s\n", csv_buffer->file_name);
                        printf("    channel:      %d\n", csv_buffer->header->channel);
                        printf("    description:  %s\n", csv_buffer->header->description);
                        printf("    lines:        %d\n", list_size(csv_buffer->list));
                        list_iterator_stop(csv_buffer->list);
                        list_iterator_start(csv_buffer->list);
                            printf("        ---------------------- ----------- ----------- ----------- \n");
                            printf("       | Timestamp            | Average   | High      | Low       |\n");
                            printf("        ---------------------- ----------- ----------- ----------- \n");
                        while (list_iterator_hasnext(csv_buffer->list)) {
                            csv_row = (csv_row_t*)list_iterator_next(csv_buffer->list);
                            strftime(time_string, 31, "%Y/%m/%d %H:%M:%S", gmtime(&(csv_row->timestamp)));
                            printf("       | %s | % 9d | % 9d | % 9d |\n", time_string,
                                   csv_row->average, csv_row->high, csv_row->low );
                        }
                        list_iterator_stop(csv_buffer->list);
                        csv_buffer = csv_buffer_destroy(csv_buffer);
                    }
                    msh_data = buffer_destroy(msh_data);
                }
            }
        }
        if (record_limit) {
            if (record_limit == 1) {
                break;
            }
            record_limit--;
        }
    } 

 clean:
    if ( file_handle ) {
        fclose( file_handle );
        file_handle = NULL;
    }
    if ( buffer ) {
        free( buffer );
        buffer = NULL;
    }

    return 0;
}

void print_usage( char **args, char *msg, ... ) 
{
    va_list arglist;
    const char *exec_name = args[0];

    va_start(arglist, msg);
    if (msg) {
        vfprintf( stderr, msg, arglist );
        fprintf( stderr, "\n" );
    }
    if (args && (*args)) {
        exec_name = *args;
    }

    fprintf( stderr, "usage: %s [options] seed_file\n"
                     "      options:\n"
                     "        -h       -- print opaque blockette headers \n"
                     "        -i INDEX -- start with record at INDEX\n"
                     "        -l LIMIT -- display max of LIMIT records\n"
                     "        -r       -- display file contents as raw data\n"
                     "        -F COUNT -- display first COUNT records\n"
                     "        -L COUNT -- display last COUNT records\n"
                     "        -f       -- falcon report format\n"
                     "\n"
                     "        -I       -- display file info then quit\n",
                     exec_name );
}

int verify_int( char* arg_str, int* value ) {
    int result = 0;
    regex_t regex;
    regmatch_t match_list[MAX_MATCHES];

    if (!arg_str || !value) {
        goto unclean;
    }
    if (regcomp(&regex, "^[+]?[0-9]+$", REG_EXTENDED | REG_NEWLINE)) {
        goto unclean;
    }
    if (regexec(&regex, arg_str, (size_t)MAX_MATCHES, match_list, 0)) {
        goto unclean;
    }
    *value = atoi(arg_str);
    result = 1;

 unclean:
    regfree(&regex);
    return result;
}

