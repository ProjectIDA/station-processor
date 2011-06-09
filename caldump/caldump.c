/* caldump.c
 *
 * Author: Joel Edwards
 *         jdedwards@usgs.gov, joeledwards@gmail.com
 * 
 * Produces formatted output of SEED calibration record headers,
 * and optionally prints out the header type of every record
 * in the SEED file.
 * 
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

#include <dump_hex.h>
#include <jio.h>
#include <lib330.h>
#include <record.h>

char* optarg = NULL;
int optind = 0, opterr = 0, optopt = 0;

void print_cal2( cal2 *cal2_info, size_t depth );
void print_usage( char **args, char *msg );
void print_memory_info();
void print_blockette_type( int16_t blockette_type, bool ignore_cals );

int main ( int argc, char **argv )
{
    bool eof_reached = false;
    FILE *file_handle = NULL;
    long int file_position = 0;

    uint8_t *buffer = NULL;
    size_t records_read = 0;

    uint8_t *current_record = NULL; /* the selected SEED record */
    uint8_t *current_blockette = NULL; /* the selected blockette */

    size_t record_size = 0;
    seed_header header_seed; /* SEED header struct*/
    seed_file_info file_info;
    step_calibration   header_cal_step;
    sine_calibration   header_cal_sine;
    random_calibration header_cal_random;
    abort_calibration  header_cal_abort;

    int16_t blockette_type;
    int16_t next_blockette;
    int16_t blockette_length;

    int c;

    /* arguments */
    bool display_mem  = false;
    bool display_raw  = false;
    bool succinct     = false;
    int verbosity     = 0;

    size_t record_count = 0;
    size_t blockette_count = 0;

    size_t file_count;
    char** files;
    char** files_end;

    /* ensure we received a file argument */
    if ( argc < 2 ) {
        print_usage( argv, NULL );
        goto stop;
    }

    while ((c = getopt( argc, argv, "mrsv" )) != -1) {
        switch (c) {
            case 'm':
                display_mem = true;
                break;
            case 'r':
                display_raw = true;
                break;
            case 's':
                succinct = true;
                break;
            case 'v':
                verbosity++;
                break;
            default:
                fprintf( stderr, "Got Filename\n" );
                break;
        }
    }

    files = argv + optind;
    file_count = argc - optind;
    files_end = files + file_count;

    if (succinct) {
        verbosity = 0;
        display_raw = false;
        display_mem = false;
    }

    for (; files < files_end; files++) {
        if ( file_handle ) {
            fclose( file_handle );
            file_handle = NULL;
        }
        if ( buffer ) {
            free( buffer );
            buffer = NULL;
        }

        get_seed_file_info( *files, &file_info, 0 );
        if (!file_info.good) {
            fprintf( stderr, "File '%s' is invalid.\n", *files );
            continue;
        }
        record_size = file_info.record_size;

        buffer = malloc( (size_t)(BUFFER_SIZE) );
        if ( !buffer ) {
            fprintf( stderr, "Could not allocate memory for read buffer.\n" );
            continue;
        }

        file_handle = fopen( *files, "rb" );
        if ( !file_handle ) {
            fprintf( stderr, "Unable to open file '%s' for reading.\n", *files );
            continue;
        }

        /* interpret header type, and break into opaque block types */
        while ( !eof_reached ) {
            records_read = fread( buffer, record_size, 1, file_handle );
            file_position = ftell( file_handle );
            record_count++;

            if ( verbosity > 1 ) {
                fprintf( stdout, "Read %d record(s)\n", records_read );
                fprintf( stdout, "File position is %ld\n", file_position );
            }
            if ( feof(file_handle) || ferror(file_handle) ) {
                if ( !succinct ) {
                    fprintf( stdout, "EOF reached\n" );
                }
                eof_reached = true;
            }
            if ( display_raw ) {
                fprintf( stdout, "Raw Data: %lu\n", (unsigned long)records_read );
                dump_hex( buffer, records_read * record_size, 
                          (size_t)DUMP_BYTES, 
                          (size_t)(record_size / DUMP_BYTES) );
            }

            /* populate the header structs */
            current_record = buffer;
            loadseedhdr( (pbyte *)&current_record, &header_seed, false );
            next_blockette = header_seed.first_blockette_byte;
            blockette_type = 0;

            while ( next_blockette ) {
                current_blockette = buffer + next_blockette;
                blockette_type = ntohs(*(int16_t *)(current_blockette + 0));
                next_blockette = ntohs(*(int16_t *)(current_blockette + 2));
                blockette_count++;
                if (blockette_type == 300) {
                    blockette_length = ntohs(*(int16_t *)(current_blockette + 4));
                    loadstep( (pbyte *)&current_blockette, &header_cal_step );

                    if ( succinct ) {
                        jprintf( 0, "[%c%c_%c%c%c%c%c %c%c-%c%c%c]> Step cal.   [%04u,%03u %02u:%02u:%02u.%04u] %lu seconds\n",
                                 header_seed.seednet[0],
                                 header_seed.seednet[1],
                                 header_seed.station_id_call_letters[0],
                                 header_seed.station_id_call_letters[1],
                                 header_seed.station_id_call_letters[2],
                                 header_seed.station_id_call_letters[3],
                                 header_seed.station_id_call_letters[4],
                                 header_seed.location_id[0],
                                 header_seed.location_id[1],
                                 header_seed.channel_id[0],
                                 header_seed.channel_id[1],
                                 header_seed.channel_id[2],
                                 (uint16_t)header_seed.starting_time.seed_yr,
                                 (uint16_t)header_seed.starting_time.seed_jday,
                                 (uint16_t)header_seed.starting_time.seed_hr,
                                 (uint16_t)header_seed.starting_time.seed_minute,
                                 (uint16_t)header_seed.starting_time.seed_seconds,
                                 (uint16_t)header_seed.starting_time.seed_tenth_millisec,
                                 (unsigned long)header_cal_step.calibration_duration/10000
                        );
                    } else {
                        printf( "Found a Step Calibration Blockette\n" );
                        if (verbosity > 0) {
                            jprintf( 1, "Blockette Type: %lu\n",
                                     (unsigned long)header_cal_step.blockette_type );
                            jprintf( 1, "Next Blockette: %lu\n",
                                     (unsigned long)header_cal_step.next_blockette );
                        }
                        jprintf( 1, "Starting Time:  %04u,%03u %02u:%02u:%02u.%04u\n",
                                (uint16_t)header_seed.starting_time.seed_yr,
                                (uint16_t)header_seed.starting_time.seed_jday,
                                (uint16_t)header_seed.starting_time.seed_hr,
                                (uint16_t)header_seed.starting_time.seed_minute,
                                (uint16_t)header_seed.starting_time.seed_seconds,
                                (uint16_t)header_seed.starting_time.seed_tenth_millisec );
                        jprintf( 1, "Calibration Duration: %lu seconds",
                                 (unsigned long)header_cal_step.calibration_duration / 10000);
                        if (header_cal_step.calibration_duration % 10000) {
                            jprintf( 0, " + %lu tenth-milliseconds",
                                     (unsigned long)header_cal_step.calibration_duration % 10000 );
                        }
                        jprintf( 0, "\n" );
                        jprintf( 1, "Interval Duration: %lu\n",
                                 (unsigned long)header_cal_step.interval_duration );
                        jprintf( 1, "Number of Steps: %lu\n",
                                 (unsigned long)header_cal_step.number_of_steps );
                        print_cal2( &header_cal_step.step2, 1 );
                        jprintf( 1, "Calibration Flags: %02lx\n",
                                 (unsigned long)header_cal_step.calibration_flags );
                    }

                } else if (blockette_type == 310) {
                    blockette_length = ntohs(*(int16_t *)(current_blockette + 4));
                    loadsine( (pbyte *)&current_blockette, &header_cal_sine );

                    if ( succinct ) {
                        jprintf( 0, "[%c%c_%c%c%c%c%c %c%c-%c%c%c]> Sine cal.   [%04u,%03u %02u:%02u:%02u.%04u] %lu seconds\n",
                                 header_seed.seednet[0],
                                 header_seed.seednet[1],
                                 header_seed.station_id_call_letters[0],
                                 header_seed.station_id_call_letters[1],
                                 header_seed.station_id_call_letters[2],
                                 header_seed.station_id_call_letters[3],
                                 header_seed.station_id_call_letters[4],
                                 header_seed.location_id[0],
                                 header_seed.location_id[1],
                                 header_seed.channel_id[0],
                                 header_seed.channel_id[1],
                                 header_seed.channel_id[2],
                                 (uint16_t)header_seed.starting_time.seed_yr,
                                 (uint16_t)header_seed.starting_time.seed_jday,
                                 (uint16_t)header_seed.starting_time.seed_hr,
                                 (uint16_t)header_seed.starting_time.seed_minute,
                                 (uint16_t)header_seed.starting_time.seed_seconds,
                                 (uint16_t)header_seed.starting_time.seed_tenth_millisec,
                                 (unsigned long)header_cal_sine.calibration_duration / 10000);
                    } else {
                        printf( "Found a Sine Calibration Blockette\n" );
                        if (verbosity > 0) {
                            jprintf( 1, "Blockette Type: %lu\n",
                                     (unsigned long)header_cal_sine.blockette_type );
                            jprintf( 1, "Next Blockette: %lu\n",
                                     (unsigned long)header_cal_sine.next_blockette );
                        }
                        jprintf( 1, "Starting Time:  %04u,%03u %02u:%02u:%02u.%04u\n",
                                (uint16_t)header_seed.starting_time.seed_yr,
                                (uint16_t)header_seed.starting_time.seed_jday,
                                (uint16_t)header_seed.starting_time.seed_hr,
                                (uint16_t)header_seed.starting_time.seed_minute,
                                (uint16_t)header_seed.starting_time.seed_seconds,
                                (uint16_t)header_seed.starting_time.seed_tenth_millisec );
                        jprintf( 1, "Calibration Duration: %lu seconds",
                                 (unsigned long)header_cal_sine.calibration_duration / 10000);
                        if (header_cal_sine.calibration_duration % 10000) {
                            jprintf( 0, " + %lu tenth-milliseconds",
                                     (unsigned long)header_cal_sine.calibration_duration % 10000 );
                        }
                        jprintf( 0, "\n" );
                        jprintf( 1, "Sine Period: %f\n",
                                 (float)header_cal_sine.sine_period );
                        print_cal2( &header_cal_sine.sine2, 1 );
                        jprintf( 1, "Calibration Flags: %02lx\n",
                                 (unsigned long)header_cal_sine.calibration_flags );
                    }

                } else if (blockette_type == 320) {
                    blockette_length = ntohs(*(int16_t *)(current_blockette + 4));
                    loadrandom( (pbyte *)&current_blockette, &header_cal_random );

                    if ( succinct ) {
                        jprintf( 0, "[%c%c_%c%c%c%c%c %c%c-%c%c%c]> Random cal. [%04u,%03u %02u:%02u:%02u.%04u] %lu seconds\n",
                                 header_seed.seednet[0],
                                 header_seed.seednet[1],
                                 header_seed.station_id_call_letters[0],
                                 header_seed.station_id_call_letters[1],
                                 header_seed.station_id_call_letters[2],
                                 header_seed.station_id_call_letters[3],
                                 header_seed.station_id_call_letters[4],
                                 header_seed.location_id[0],
                                 header_seed.location_id[1],
                                 header_seed.channel_id[0],
                                 header_seed.channel_id[1],
                                 header_seed.channel_id[2],
                                 (uint16_t)header_seed.starting_time.seed_yr,
                                 (uint16_t)header_seed.starting_time.seed_jday,
                                 (uint16_t)header_seed.starting_time.seed_hr,
                                 (uint16_t)header_seed.starting_time.seed_minute,
                                 (uint16_t)header_seed.starting_time.seed_seconds,
                                 (uint16_t)header_seed.starting_time.seed_tenth_millisec,
                                 (unsigned long)header_cal_random.calibration_duration/10000
                        );
                    } else {
                        printf( "Found a Random Calibration Blockette\n" );
                        if (verbosity > 0) {
                            jprintf( 1, "Blockette Type: %lu\n",
                                     (unsigned long)header_cal_random.blockette_type );
                            jprintf( 1, "Next Blockette: %lu\n",
                                     (unsigned long)header_cal_random.next_blockette );
                        }
                        jprintf( 1, "Starting Time:  %04u,%03u %02u:%02u:%02u.%04u\n",
                                (uint16_t)header_seed.starting_time.seed_yr,
                                (uint16_t)header_seed.starting_time.seed_jday,
                                (uint16_t)header_seed.starting_time.seed_hr,
                                (uint16_t)header_seed.starting_time.seed_minute,
                                (uint16_t)header_seed.starting_time.seed_seconds,
                                (uint16_t)header_seed.starting_time.seed_tenth_millisec );
                        jprintf( 1, "Calibration Duration: %lu seconds",
                                 (unsigned long)header_cal_random.calibration_duration / 10000);
                        if (header_cal_random.calibration_duration % 10000) {
                            jprintf( 0, " + %lu tenth-milliseconds",
                                     (unsigned long)header_cal_random.calibration_duration % 10000 );
                        }
                        jprintf( 0, "\n" );
                        jprintf( 1, "Noise Type: %s\n",
                                 header_cal_random.noise_type );
                        print_cal2( &header_cal_random.random2, 1 );
                        jprintf( 1, "Calibration Flags: %02lx\n",
                                 (unsigned long)header_cal_random.calibration_flags );
                    }

                } else if ( verbosity > 0 ) {
                    print_blockette_type( blockette_type, true );
                }
            }
        } 

        if (!succinct) {
            printf( "\n" );
            printf( "File '%s':\n", *files );
            printf( "  File size: %lu bytes\n", file_info.file_size );
            printf( "  Number of %lu-byte SEED records: %lu\n",
                    (unsigned long)record_size, (unsigned long)record_count );
            printf( "  Number of blockettes: %lu\n", (unsigned long)blockette_count );
        }

    }

 stop:
    if ( display_mem ) {
        print_memory_info();
    }

    return 0;
}

void print_cal2( cal2 *cal2_info, size_t depth )
{
    jprintf( depth, "Calibration Amplitude: %f\n",
             (float)cal2_info->calibration_amplitude );
    jprintf( depth, "Calibration Input Channel: %c%c%c\n",
             ((char *)cal2_info->calibration_input_channel)[0],
             ((char *)cal2_info->calibration_input_channel)[1],
             ((char *)cal2_info->calibration_input_channel)[2] );
    jprintf( depth, "Reference Amplitude: %f\n",
             (float)cal2_info->ref_amp );
    jprintf( depth, "Coupling Method: %s\n",
             cal2_info->coupling );
    jprintf( depth, "Filtering Type: %s\n",
             cal2_info->rolloff );
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
                     "        -m  memory usage\n"
                     "        -r  print file contents as raw data\n"
                     "        -s  succinct; places cal summaries on a single line\n"
                     "            and disables verbosity, memory and raw output\n"
                     "        -v  increase verbosity for each use\n",
                     exec_name );
}

void print_memory_info() 
{
    char buf[30];
    FILE* fh = NULL;

    unsigned size;
    unsigned resident;
    unsigned share;
    unsigned text;
    unsigned lib;
    unsigned data;

    snprintf(buf, 30, "/proc/%u/statm", (unsigned)getpid());
    fh = fopen(buf, "r");

    if (fh) {
        fscanf(fh, "%u %u %u %u %u %u", &size, &resident, &share, &text, &lib, &data);
    }

    struct rusage mem_usage;

    getrusage( RUSAGE_SELF, &mem_usage );

    printf("Memory Usage Statistics:\n");
    printf("  size ------ %u\n", size);
    printf("  resident -- %u\n", resident);
    printf("  share ----- %u\n", share);
    printf("  text ------ %u\n", text);
    printf("  lib ------- %u\n", lib);
    printf("  data ------ %u\n", data);

}

void print_blockette_type( int16_t blockette_type, bool ignore_cals )
{
    switch (blockette_type) {
        case 5 :
            printf( "Found a Field Volume Identifier Blockette\n" );
            break;
        case 8 :
            printf( "Found a Telemetry Volume Identifier Blockette\n" );
            break;
        case 10 :
            printf( "Found a Volume Identifier Blockette\n" );
            break;
        case 11 :
            printf( "Found a Volume Station Header Index Blockette\n" );
            break;
        case 12 :
            printf( "Found a Volume Time Span Index Blockette\n" );
            break;
        case 30 :
            printf( "Found a Data Format Dictionary Blockette\n" );
            break;
        case 31 :
            printf( "Found a Comment Description Blockette\n" );
            break;
        case 32 :
            printf( "Found a Cited Source Dictionary Blockette\n" );
            break;
        case 33 :
            printf( "Found a Generic Abberviation Blockette\n" );
            break;
        case 34 :
            printf( "Found a Units Abberviations Blockette\n" );
            break;
        case 35 :
            printf( "Found a Beam Configuration Blockette\n" );
            break;
        case 41 :
            printf( "Found a FIR Dictionary Blockette\n" );
            break;
        case 42 :
            printf( "Found a Response (Polynomial) Dictionary Blockette\n" );
            break;
        case 43 :
            printf( "Found a Response (Poles & Zeros) Dictionary Blockette\n" );
            break;
        case 44 :
            printf( "Found a Response (Coefficients) Dictionary Blockette\n" );
            break;
        case 45 :
            printf( "Found a Response List Dictionary Blockette\n" );
            break;
        case 46 :
            printf( "Found a Generic Response Dictionary Blockette\n" );
            break;
        case 47 :
            printf( "Found a Decimation Dictionary Blockette\n" );
            break;
        case 48 :
            printf( "Found a Channel Sensitivity/Gain Dictionary Blockette\n" );
            break;
        case 49 :
            printf( "Found a Response (Polynomial) Dictionary Blockette\n" );
            break;
        case 50 :
            printf( "Found a Station Identifier Blockette\n" );
            break;
        case 51 :
            printf( "Found a Station Comment Blockette\n" );
            break;
        case 52 :
            printf( "Found a Channel Identifier Blockette\n" );
            break;
        case 53 :
            printf( "Found a Response (Poles & Zeros) Blockette\n" );
            break;
        case 54 :
            printf( "Found a Response (Coefficients) Blockette\n" );
            break;
        case 55 :
            printf( "Found a Response List Blockette\n" );
            break;
        case 56 :
            printf( "Found a Generic Response Blockette\n" );
            break;
        case 57 :
            printf( "Found a Decimation Blockette\n" );
            break;
        case 58 :
            printf( "Found a Channel Sensitivity/Gain Blockette\n" );
            break;
        case 59 :
            printf( "Found a Channel Comment Blockette\n" );
            break;
        case 60 :
            printf( "Found a Response Reference Blockette\n" );
            break;
        case 61 :
            printf( "Found a FIR Response Blockette\n" );
            break;
        case 62 :
            printf( "Found a Response [Polynomial] Blockette\n" );
            break;
        case 70 :
            printf( "Found a Time Span Identifier Blockette\n" );
            break;
        case 71 :
            printf( "Found a Hypocenter Information Blockette\n" );
            break;
        case 72 :
            printf( "Found an Event Phase Blockette\n" );
            break;
        case 73 :
            printf( "Found a Time Span Data Start Index Blockette\n" );
            break;
        case 74 :
            printf( "Found a Time Series Index Blockette\n" );
            break;
        case 100 :
            printf( "Found a Sample Rate Blockette\n" );
            break;
        case 200 :
            printf( "Found a Generic Event Detection Blockette\n" );
            break;
        case 201 :
            printf( "Found a Murdock Event Detection Blockette\n" );
            break;
        case 202 :
            printf( "Found a Log-Z Event Detection Blockette\n" );
            break;
        case 300 :
            if (!ignore_cals) {
                printf( "Found a Step Calibration Blockette\n" );
            }
            break;
        case 310 :
            if (!ignore_cals) {
                printf( "Found a Sine Calibration Blockette\n" );
            }
            break;
        case 320 :
            if (!ignore_cals) {
                printf( "Found a Psuedo-random Calibration Blockette\n" );
            }
            break;
        case 390 :
            printf( "Found a Generic Calibration Blockette\n" );
            break;
        case 395 :
            printf( "Found an Abort Calibration Blockette\n" );
            break;
        case 400 :
            printf( "Found a Beam Blockette\n" );
            break;
        case 405 :
            printf( "Found a Beam Delay Blockette\n" );
            break;
        case 500 :
            printf( "Found a Timing Blockette\n" );
            break;
        case 1000 :
            printf( "Found a Data Only SEED Blockette\n" );
            break;
        case 1001 :
            printf( "Found a Data Extension Blockette\n" );
            break;
        case 2000 :
            printf( "Found an Opaque Blockette\n" );
            break;
        default :
            printf( "Encountered an Unrecognized Blockette [%d]\n",
                blockette_type );
            break;
    }
}

