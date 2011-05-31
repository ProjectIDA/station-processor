/* ocfdump.c
 *
 * Author: Joel Edwards
 *         jdedwards@usgs.gov, joeledwards@gmail.com
 * 
 * Produces raw and formatted output of an file containing
 * opaque SEED records written by a Quanterra Q330.
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

#include <lib330.h>
#include <record.h>
#include <tokens.h>
#include <dump_hex.h>
#include <jio.h>

void print_usage( char **args, char *msg );
void print_memory_info();

int main ( int argc, char **argv )
{
    bool eof_reached = false;
    FILE *file_handle = NULL;
    char *file_name = NULL;
    long int file_position = 0;

    uint8_t *buffer = NULL;
    size_t records_read = 0;
    size_t header_len = 0;
    size_t i = 0, j = 0;
    uint8_t *current_record = NULL; /* the selected 512-byte record */
    uint8_t *current_position = NULL; /* position within selected record */
    uint8_t *header_ptr = NULL; /* temporary pointer for header parsing functions*/

    seed_header header_seed; /* SEED header struct*/
    topaque_hdr header_opaque; /* Opaque blockette header struct*/
    tglobal opaque_global;
    tfixed  opaque_fixed;
    tlog    opaque_log;
    token_context context_tokens;
    char rec_type_str[REC_TYPE_MAX_LEN + 1];

    int16_t blockette_type;
    int16_t next_blockette;
    int16_t blockette_length;

    int c;

    /* arguments */
    bool display_global = false;
    bool display_fixed  = false;
    bool display_log    = false;
    bool display_tokens = false;
    bool display_tokens_raw    = false;
    bool parse_tokens_manually = false;
    bool display_raw    = false;
    bool display_header = false;
    bool display_mem    = false;

    uint32_t print_flags = 0x00000000L;

    memset( &context_tokens, 0, sizeof(token_context) );

    /* ensure we received a file argument */
    if ( argc < 2 ) {
        print_usage( argv, NULL );
        goto clean;
    }

    file_name = argv[argc - 1];

    while ((c = getopt( argc, argv, "fghlmrCcTtw" )) != -1) {
        switch (c) {
            case 'f': 
                display_fixed = true;
                break;
            case 'g': 
                display_global = true;
                break;
            case 'l': 
                display_log = true;
                break;
            case 't':
                context_tokens.summary_only = true;
                display_tokens = true;
                break;
            case 'T': 
                display_tokens = true;
                break;
            case 'c': 
                display_tokens_raw = true;
                break;
            case 'C': 
                parse_tokens_manually = true;
                break;
            case 'r':
                display_raw = true;
                break;
            case 'h':
                display_header = true;
                break;
            case 'm':
                display_mem = true;
                break;
            case 'w':
                SET_PRINT_WILLARD(print_flags);
                break;
            default:
                fprintf( stderr, "Got Filename\n" );
                break;
        }
    }

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
        records_read = fread( buffer, (size_t)RECORD_SIZE, (size_t)MAX_RECORDS, file_handle );
        file_position = ftell( file_handle );

        fprintf( stdout, "Read %d record(s)\n", records_read );
        fprintf( stdout, "File position is %ld\n", file_position );
        if ( feof(file_handle) || ferror(file_handle) ) {
            fprintf( stdout, "EOF reached\n" );
            eof_reached = true;
        }
        if ( display_raw ) {
            fprintf( stdout, "Raw Data:\n" );
            dump_hex( buffer, records_read * (size_t)RECORD_SIZE, 
                      (size_t)DUMP_BYTES, 
                      (size_t)(RECORD_SIZE / DUMP_BYTES) );
        }

        /* populate the header structs */
        current_record = buffer;
        for (i = 0; i < records_read; i++ ) {
            header_ptr = current_record;
            loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );
            next_blockette = header_seed.first_blockette_byte;
            blockette_type = 0;
            current_position = current_record + next_blockette;

            while ( next_blockette ) {
                blockette_type   = ntohs(*(int16_t *)(current_position + 0));
                next_blockette   = ntohs(*(int16_t *)(current_position + 2));
                if (blockette_type == 2000) {
                    blockette_length = ntohs(*(int16_t *)(current_position + 4));
                    header_ptr = current_position;
                    loadopaquehdr( (pbyte *)&header_ptr, &header_opaque );
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

                    header_ptr = current_position + (size_t)header_opaque.data_off;
                    header_len = (size_t)header_opaque.blk_lth - (size_t)header_opaque.data_off;
                    if ( header_opaque.rec_type[0] == 'G' && 
                         header_opaque.rec_type[1] == 'L' ) {
                        loadglob( (pbyte *)&header_ptr, &opaque_global );
                        if ( display_global ) {
                            jprintf( 2, "Global Config:\n" );
                            print_global( &opaque_global, 3, print_flags );
                        }
                    }
                    else if ( header_opaque.rec_type[0] == 'F' && 
                              header_opaque.rec_type[1] == 'X' ) {
                        loadfix( (pbyte *)&header_ptr, &opaque_fixed ); 
                        if ( display_fixed ) {
                            jprintf( 2, "Fixed Config:\n" );
                            print_fixed( &opaque_fixed, 3, print_flags );
                        }
                    }
                    else if ( header_opaque.rec_type[0] == 'L' ) {
                        loadlog( (pbyte *)&header_ptr, &opaque_log );
                        if ( display_log ) {
                            jprintf( 2, "Log Config:\n" );
                            print_log( &opaque_log, 3, print_flags );
                        }
                    }
                    else if ( header_opaque.rec_type[0] == 'T' ) {
                        append_tokens( &context_tokens,
                                       header_ptr,
                                       header_len,
                                       header_opaque.opaque_flags );
                        if ( tokens_complete( &context_tokens ) ) {
                            if ( display_tokens ) {
                                jprintf( 2, "Token Config:\n" );
                                print_tokens( &context_tokens, 3, print_flags );
                            } else if ( parse_tokens_manually ) {
                                jprintf( 2, "Token Config (manual):\n" );
                                parse_tokens( &context_tokens, display_tokens_raw, 3);
                            } else if ( display_tokens_raw ) {
                                jprintf( 2, "Token Config (raw):\n" );
                                dump_tokens( &context_tokens );
                            }
                        }
                    }
                }
                else if (blockette_type == 1000) {
                    blockette_length = ntohs((uint16_t)(*(current_position + 6)));
                }
                current_position = current_record + next_blockette;
            }
            current_record += (size_t)RECORD_SIZE;
        }
    } 

    if ( display_mem ) {
        print_memory_info();
    }

    fclose( file_handle );

 clean:
    if ( buffer ) {
        free( buffer );
        buffer = NULL;
    }

    return 0;
}

void print_usage( char **args, char *msg ) 
{
    char *exec_name = "ocfdump";

    if (msg) {
        fprintf( stderr, "%s\n", msg );
    }
    if (args && (*args)) {
        exec_name = *args;
    }

    fprintf( stderr, "usage: %s [options] seed_file\n"
                     "      options:\n"
                     "        -h  print opaque blockette headers \n"
                     "        -r  print file contents as raw data\n"
                     "        - - - - -\n"
                     "        -f  show fixed configuration\n"
                     "        -g  show global configuration\n"
                     "        -l  show log configuration\n"
                     "        -c  print token configuration raw data\n"
                     "        -C  print raw data for each token\n"
                     "        -T  show token configuration\n"
                     "        -t  show token configuration headers\n"
                     "        -w  show where to find these values in Willard\n",
                     exec_name );
}

void print_memory_info() {
    struct rusage mem_usage;

    getrusage( RUSAGE_SELF, &mem_usage );

    printf( "Memory Usage Statistics:\n" );
    printf( "  User time used ---------------- %li.%06lli sec.\n",
            mem_usage.ru_utime.tv_sec, 
            mem_usage.ru_utime.tv_usec % 1000000LL );
    printf( "  System time used -------------- %li.%06lli sec.\n",
            mem_usage.ru_stime.tv_sec, 
            mem_usage.ru_stime.tv_usec % 1000000LL );
    printf( "  Maximum resident set size ----- %li\n", mem_usage.ru_maxrss );
    printf( "  Integral shared memory size --- %li\n", mem_usage.ru_ixrss );
    printf( "  Integral unshared data size --- %li\n", mem_usage.ru_idrss );
    printf( "  Integral unshared stack size -- %li\n", mem_usage.ru_isrss );
    printf( "  Page reclaims ----------------- %li\n", mem_usage.ru_minflt );
    printf( "  Page faults ------------------- %li\n", mem_usage.ru_majflt );
    printf( "  Swaps ------------------------- %li\n", mem_usage.ru_nswap );
    printf( "  Block input operations -------- %li\n", mem_usage.ru_inblock );
    printf( "  Block output operations ------- %li\n", mem_usage.ru_oublock );
    printf( "  Messages sent ----------------- %li\n", mem_usage.ru_msgsnd );
    printf( "  Messages received ------------- %li\n", mem_usage.ru_msgrcv );
    printf( "  Signals received -------------- %li\n", mem_usage.ru_nsignals );
    printf( "  Voluntary context switches ---- %li\n", mem_usage.ru_nvcsw );
    printf( "  Involuntary context switches -- %li\n", mem_usage.ru_nivcsw );

}

