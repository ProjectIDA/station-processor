#include <lib330.h>
#include <format_data.h>
#include <simclist.h>
#include <record.h>
#include <jio.h>

void print_seed_time( const char* prefix_msg, tseed_time* time, 
                      const char* postfix_msg ) {
    if ( time ) {
        printf( "%s[%04d-%03d %02d:%02d:%02d.%04d]%s",
                prefix_msg,
                time->seed_yr, 
                time->seed_jday, 
                time->seed_hr, 
                time->seed_minute, 
                time->seed_seconds,
                time->seed_tenth_millisec,
                postfix_msg );
    }
}

void print_seed_header( seed_header* header ) {
    printf( "SEED Record:" );
    printf( " %c%c_%c%c%c%c%c %c%c-%c%c%c",
            header->seednet[0],
            header->seednet[1], 
            header->station_id_call_letters[0],
            header->station_id_call_letters[1],
            header->station_id_call_letters[2],
            header->station_id_call_letters[3],
            header->station_id_call_letters[4],
            header->location_id[0],
            header->location_id[1],
            header->channel_id[0],
            header->channel_id[1],
            header->channel_id[2] );
    print_seed_time( " ", &header->starting_time, "\n" );
}

void get_seed_file_info( char *file_name, seed_file_info* file_info, 
                         bool print_file_info ) {
    FILE *file_handle = NULL;
    uint8_t *buffer = NULL;
    uint8_t size_exponent;
    size_t end_offset;
    long file_length;
    long records_read;
    size_t record_size = 0;
    seed_header header_seed; /* SEED header struct*/
    uint8_t* header_ptr = NULL;
    uint8_t *data_blockette = NULL;

    file_info->good = false;

    if ( print_file_info ) {
        printf( "Getting file info for %s\n", file_name );
    }

    if ( !file_name ) {
        fprintf( stderr, "No file name supplied.\n" );
        goto clean;
    }

    buffer = malloc( (size_t)(BLOCK_SIZE) );
    if ( !buffer ) {
        fprintf( stderr, "Could not allocate memory for read buffer.\n" );
        goto clean;
    }

    file_handle = fopen( file_name, "rb" );
    if ( !file_handle ) {
        fprintf( stderr, "Unable to open file '%s' for reading.\n", file_name );
        goto clean;
    }

    fseek( file_handle, 0, SEEK_END );
    file_length = ftell( file_handle );
    if ( file_length < (long)MIN_FILE_SIZE ) {
        fprintf( stderr, "File is too small to contain SEED data.\n" );
        goto clean;
    }

    fseek( file_handle, 0, SEEK_SET ); 
    records_read = fread( buffer, (size_t)BLOCK_SIZE, 1, file_handle );
    header_ptr = buffer;
    loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );

    data_blockette = buffer + (size_t)(header_seed.first_blockette_byte);

    if ( ntohs(*((uint16_t*)(data_blockette))) != 1000 ) {
        fprintf( stderr, "Require blockette 1000 after header, found blockette %d\n", ntohs(*((uint16_t*)(data_blockette))) );
        goto clean;
    }

    size_exponent = *(data_blockette + 6);
    record_size = 1 << size_exponent;
    file_info->record_size = record_size;

    if ( print_file_info ) {
        printf( "record size is %d\n", record_size );
    }

    end_offset = file_length % record_size
                    ? file_length / record_size 
                        ? file_length - (file_length % record_size) - record_size
                        : (long)0
                    : file_length - record_size;

    if ( print_file_info ) {
        printf( "File size is %li bytes.\n", file_length );
        printf( "Reading at position [%li]\n", (long)0 );
    }

    fseek( file_handle, 0, SEEK_SET ); 
    records_read = fread( buffer, record_size, 1, file_handle );
    header_ptr = buffer;
    loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );
    if ( print_file_info ) {
        print_seed_header( &header_seed );
    }
    if ( !records_read ) {
        goto clean;
    }
    file_info->start_time = header_seed.starting_time;

    if ( print_file_info ) {
        printf( "Reading at position [%lu]\n", (unsigned long)end_offset );
    }
    fseek( file_handle, end_offset, SEEK_SET ); 
    records_read = fread( buffer, record_size, 1, file_handle );
    header_ptr = buffer;
    loadseedhdr( (pbyte *)&header_ptr, &header_seed, false );
    if ( print_file_info ) {
        print_seed_header( &header_seed );
    }
    if ( !records_read ) {
        goto clean;
    }
    file_info->end_time = header_seed.starting_time;

    fclose( file_handle );

    file_info->length = file_length;
    file_info->good = true;
clean:
    if ( buffer ) {
        free( buffer );
    }
}

