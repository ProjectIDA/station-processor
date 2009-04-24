/* ofadump.c
 *
 * Author: Joel Edwards
 *         jdedwards@usgs.gov, joeledwards@gmail.com
 * 
 * Produces raw and formatted output of a file containing
 * opaque SEED records containing compressed statistical
 * data from an RLE Technologies (R)  Falcon (C)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <fmash.h>
#include <format_data.h>
#include <lib330.h>
#include <jio.h>
#include <record.h>
#include <tokens.h>

void print_usage( char **args, char *msg );

int gDebug = 1;

int main ( int argc, char **argv )
{
    bool eof_reached = false;
    FILE *file_handle = NULL;
    char *file_name = NULL;
    long int file_position = 0;

    uint8_t *buffer = NULL;
    size_t records_read = 0;
    size_t data_length = 0;
    size_t i = 0, j = 0;
    uint8_t *current_record = NULL; /* the selected SEED record */
    uint8_t *current_header = NULL; /* position of current SEED record header */
    uint8_t *current_blockette = NULL; /* the selected blockette */
    uint8_t *current_data = NULL; /* location of data within this blockette */

    buffer_t* msh_data = NULL;
    seed_file_info file_info;
    seed_header header_seed; /* SEED header struct*/
    topaque_hdr header_opaque; /* Opaque blockette header struct*/
    char rec_type_str[REC_TYPE_MAX_LEN + 1];

    size_t record_size = 0;
    int16_t next_blockette;
    int16_t blockette_type;
    int16_t blockette_length;
    bool spanning_blockette = false;

    int c;
    bool data_complete = false;

    /* arguments */
    bool display_raw    = false;
    bool display_header = false;

    uint32_t print_flags = 0x00000000L;

    csv_buffer_t* csv_buffer = NULL;
    csv_row_t* csv_row = NULL;

    /* ensure we received a file argument */
    if ( argc < 2 ) {
        print_usage( argv, NULL );
        goto clean;
    }

    file_name = argv[argc - 1];

    while ((c = getopt( argc, argv, "hr" )) != -1) {
        switch (c) {
            case 'h':
                display_header = true;
                break;
            case 'r':
                display_raw = true;
                break;
            default:
                fprintf( stderr, "Got Filename\n" );
                break;
        }
    }

    get_seed_file_info( file_name, &file_info, 1 );
    if (!file_info.good) {
        fprintf( stderr, "File '%s' is invalid.\n", file_name );
        goto clean;
    }
    record_size = file_info.record_size;

    buffer = malloc( (size_t)(BUFFER_SIZE) );
    if ( !buffer ) {
        fprintf( stderr, "Could not allocate memory for read buffer.\n" );
        goto clean;
    }

    file_handle = fopen( file_name, "rb" );
    if ( !file_handle ) {
        fprintf( stderr, "Unable to open file '%s' for reading.\n", file_name );
        goto clean;
    }

    /* interpret header type, and break into opaque block types */
    while ( !eof_reached ) {
        records_read = fread( buffer, record_size, 1, file_handle );
        file_position = ftell( file_handle );

        fprintf( stdout, "Read %d record(s)\n", records_read );
        fprintf( stdout, "File position is %ld\n", file_position );
        if ( feof(file_handle) || ferror(file_handle) ) {
            fprintf( stdout, "EOF reached\n" );
            eof_reached = true;
        }
        if ( display_raw ) {
            fprintf( stdout, "Raw Data:\n");
            format_data( buffer, records_read * record_size, 
                         (size_t)DUMP_BYTES, 
                         record_size / (size_t)DUMP_BYTES );
        }

        /* populate the header structs */
        current_header = buffer;
        loadseedhdr( (pbyte *)&current_header, &header_seed, false );
        next_blockette = header_seed.first_blockette_byte;
        blockette_type = 0;

        while ( next_blockette ) {
            current_blockette = buffer + next_blockette;
            blockette_type   = ntohs(*(int16_t *)(current_blockette + 0));
            next_blockette   = ntohs(*(int16_t *)(current_blockette + 2));
            if (blockette_type == 2000) {
                blockette_length = ntohs(*(int16_t *)(current_blockette + 4));
                loadopaquehdr( (pbyte *)&current_blockette, &header_opaque );

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

                    /* Null terminate this, otherwise it gets ugly */
                    for (j = 0; j < REC_TYPE_MAX_LEN; j++) {
                        rec_type_str[j] = header_opaque.rec_type[j];
                        if ( header_opaque.rec_type[j] == '\0' )
                            break;
                        if ( header_opaque.rec_type[j] == '~') {
                            break;
                        }
                    }
                    rec_type_str[j + 1] = '\0';
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
                    data_complete = 0;
                    format_data(msh_data->content, msh_data->length, 0, 0);

                    fmash_msh_to_csv(&csv_buffer, msh_data->content, msh_data->length);
                    printf("=== POST EXPANSION ======================================\n");
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
                    csv_buffer = csv_buffer_destroy(csv_buffer);
                    msh_data = buffer_destroy(msh_data);
                }
            }
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

void print_usage( char **args, char *msg ) 
{
    const char *exec_name = "ofadump";

    if (msg) {
        fprintf( stderr, "%s\n", msg );
    }
    if (args && (*args)) {
        exec_name = *args;
    }

    fprintf( stderr, "usage: %s [options] seed_file\n"
                     "      options:\n"
                     "        -h  print opaque blockette headers \n"
                     "        -r  print file contents as raw data\n",
                     exec_name );
}

