/* ocfdiff.c
 *
 * Author: Joel Edwards
 *         jdedwards@usgs.gov, joeledwards@gmail.com
 * 
 * Checks for OCF record modifications within a file, or between two files.
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
#include <simclist.h>
#include <record.h>
#include <tokens.h>
#include <dump_hex.h>
#include <jio.h>

#define LIST_ORDER_S2L 1
#define LIST_ORDER_L2S -1

void print_usage( char **args, char *msg );
void print_memory_info();

void diff_files( char* file1, char* file2, int verbosity );
void diff_in_file( char* file, int verbosity );
void diff_records( token_mod_link** differences,
                   tglobal* global_original, tglobal* global_updated,
                   tfixed* fixed_original, tfixed* fixed_updated,
                   tlog* log_original, tlog* log_updated,
                   bool print_diff_info );

void swap_pointers( void** ptr1, void** ptr2 );

int main ( int argc, char **argv )
{
    int file_times;

    int c = 0;

    bool single_file = false;
    int verbosity = 0;

    /* ensure we received a file argument */
    if ( argc < 3 ) {
        print_usage( argv, NULL );
        goto clean;
    }

    while ((c = getopt( argc, argv, "sv" )) != -1) {
        switch (c) {
            case 's': 
                single_file = true;
                break;
            case 'v': 
                verbosity++;
                break;
            default:
                fprintf( stderr, "Got Filename\n" );
                break;
        }
    }

    if ( single_file ) {
        diff_in_file( argv[argc - 1], verbosity );
    } else {
        diff_files( argv[argc - 1], argv[argc - 2], verbosity );
    }

clean:
    return 0;
}

void diff_in_file( char* file, int verbosity ) {
    bool finished = false;
    bool print_diff_info = verbosity > 0 ? true : false;
    bool print_file_info = verbosity > 1 ? true : false;

    list_t list_global;
    list_t list_fixed;
    list_t list_log;
    list_t list_token_context;

    seed_file_info file_info;

    printf( "Initializing lists\n" );
    if ( list_init( &list_global ) ||
         list_init( &list_fixed  ) ||
         list_init( &list_log    ) ||
         list_init( &list_token_context ) ) {
        goto cleanup;
    }

    get_seed_file_info( file, &file_info, print_file_info );

    list_attributes_comparator( &list_global, metadata_compare );
    list_attributes_comparator( &list_fixed, metadata_compare );
    list_attributes_comparator( &list_log, metadata_compare );
    list_attributes_comparator( &list_token_context, metadata_compare );

    printf( "Populating records\n" );
    populate_records( file, &list_global, &list_fixed, 
                      &list_log, &list_token_context, print_file_info );

    printf( "Sorting lists\n" );
    list_sort( &list_global, LIST_ORDER_S2L );
    list_sort( &list_fixed, LIST_ORDER_S2L );
    list_sort( &list_log, LIST_ORDER_S2L );
    list_sort( &list_token_context, LIST_ORDER_S2L );

    printf( "Comparing lists\n" );
    diff_list_global( &list_global, print_diff_info );
    diff_list_fixed( &list_fixed, print_diff_info );
    diff_list_log( &list_log, print_diff_info );
    diff_list_tokens( &list_token_context, print_diff_info );

goto finish;
cleanup:
    fprintf( stderr, "A list initialization failed.\n" );
    list_init(&list_global);
    list_init(&list_fixed);
    list_init(&list_log);
    list_init(&list_token_context);
finish:
    return;
}

void diff_files( char* file_name_a, char* file_name_b, int verbosity ) {
    bool finished = false;
    bool print_diff_info = verbosity > 0 ? true : false;
    bool print_file_info = verbosity > 1 ? true : false;

    seed_file_info file_info_a;
    seed_file_info file_info_b;

    char* file_name_original;
    char* file_name_updated;

    list_t list_global_original;
    list_t list_global_updated;
    list_t list_fixed_original;
    list_t list_fixed_updated;
    list_t list_log_original;
    list_t list_log_updated;
    list_t list_token_context_original;
    list_t list_token_context_updated;

    list_t list_global;
    list_t list_fixed;
    list_t list_log;
    list_t list_token_context;

    printf( "Retrieving file info\n" );
    get_seed_file_info( file_name_a, &file_info_a, print_file_info );
    get_seed_file_info( file_name_b, &file_info_b, print_file_info );
    if ( print_file_info ) {
        print_seed_time( "File A Start Time ", &file_info_a.start_time, "\n" );
        print_seed_time( "File A End Time   ", &file_info_a.end_time,   "\n" );
        print_seed_time( "File B Start Time ", &file_info_b.start_time, "\n" );
        print_seed_time( "File B End Time   ", &file_info_b.end_time,   "\n" );
    }

    printf( "Comparing file times\n" );
    if ( cmp_times( &file_info_a.start_time, &file_info_a.end_time   )  > 0 ||
         cmp_times( &file_info_b.start_time, &file_info_b.end_time   )  > 0 ||
         cmp_times( &file_info_a.start_time, &file_info_b.start_time ) == 0 ||
         cmp_times( &file_info_a.start_time, &file_info_b.end_time   ) == 0 ||
         cmp_times( &file_info_a.end_time, &file_info_b.start_time   ) == 0 ||
         cmp_times( &file_info_a.end_time, &file_info_b.end_time     ) == 0 ) {
        printf( "File times too close\n" );
        goto cleanup;
    } 
    else if ( cmp_times( &file_info_a.end_time, &file_info_b.start_time ) < 0 ) {
        file_name_original = file_name_a;
        file_name_updated  = file_name_b;
    }
    else if ( cmp_times( &file_info_b.end_time, &file_info_a.start_time ) < 0 ) {
        file_name_original = file_name_b;
        file_name_updated  = file_name_a;
    }
    else {
        printf( "File times not as expected\n" );
        goto cleanup;
    }

    printf( "Initializing lists\n" );
    if ( list_init( &list_global_original ) ||
         list_init( &list_fixed_original  ) ||
         list_init( &list_log_original    ) ||
         list_init( &list_token_context_original ) ||

         list_init( &list_global_updated  ) ||
         list_init( &list_fixed_updated   ) ||
         list_init( &list_log_updated     ) ||
         list_init( &list_token_context_updated ) ||

         list_init( &list_global          ) ||
         list_init( &list_fixed           ) ||
         list_init( &list_log             ) ||
         list_init( &list_token_context   ) ) {
        goto cleanup;
    }

    list_attributes_comparator( &list_global_original, metadata_compare );
    list_attributes_comparator( &list_fixed_original, metadata_compare );
    list_attributes_comparator( &list_log_original, metadata_compare );
    list_attributes_comparator( &list_token_context_original, metadata_compare );

    list_attributes_comparator( &list_global_updated, metadata_compare );
    list_attributes_comparator( &list_fixed_updated, metadata_compare );
    list_attributes_comparator( &list_log_updated, metadata_compare );
    list_attributes_comparator( &list_token_context_updated, metadata_compare );

    list_attributes_comparator( &list_global, metadata_compare );
    list_attributes_comparator( &list_fixed, metadata_compare );
    list_attributes_comparator( &list_log, metadata_compare );
    list_attributes_comparator( &list_token_context, metadata_compare );

    printf( "Populating lists\n" );
    /* Loop through file, comparing records as we go  */
    populate_records( file_name_original, 
                      &list_global_original, 
                      &list_fixed_original, 
                      &list_log_original, 
                      &list_token_context_original,
                      print_file_info );
    populate_records( file_name_updated, 
                      &list_global_updated, 
                      &list_fixed_updated, 
                      &list_log_updated, 
                      &list_token_context_updated,
                      print_file_info );

    printf( "Sorting lists\n" );
    list_sort( &list_global_original,        LIST_ORDER_S2L );
    list_sort( &list_fixed_original,         LIST_ORDER_S2L );
    list_sort( &list_log_original,           LIST_ORDER_S2L );
    list_sort( &list_token_context_original, LIST_ORDER_S2L );

    list_sort( &list_global_updated,         LIST_ORDER_S2L );
    list_sort( &list_fixed_updated,          LIST_ORDER_S2L );
    list_sort( &list_log_updated,            LIST_ORDER_S2L );
    list_sort( &list_token_context_updated,  LIST_ORDER_S2L );

    printf( "Populating test lists\n" );
    list_prepend( &list_global, list_get_max(&list_global_original) );
    list_append(  &list_global, list_get_min(&list_global_updated)  );

    list_prepend( &list_fixed, list_get_max(&list_fixed_original) );
    list_append(  &list_fixed, list_get_min(&list_fixed_updated)  );

    list_prepend( &list_log, list_get_max(&list_log_original) );
    list_append(  &list_log, list_get_min(&list_log_updated)  );

    list_prepend( &list_token_context, list_get_max(&list_token_context_original) );
    list_append(  &list_token_context, list_get_min(&list_token_context_updated)  );

    list_sort( &list_global,         LIST_ORDER_S2L );
    list_sort( &list_fixed,          LIST_ORDER_S2L );
    list_sort( &list_log,            LIST_ORDER_S2L );
    list_sort( &list_token_context,  LIST_ORDER_S2L );

    printf( "Comparing lists\n" );
    diff_list_global( &list_global, print_diff_info );
    diff_list_fixed( &list_fixed, print_diff_info );
    diff_list_log( &list_log, print_diff_info );
    diff_list_tokens( &list_token_context, print_diff_info );

goto finish;
cleanup:
    fprintf( stderr, "Either a list initialization failed, "
                     "or the files were incompatable.\n" );
    list_destroy(&list_global_original);
    list_destroy(&list_fixed_original);
    list_destroy(&list_log_original);
    list_destroy(&list_token_context_original);

    list_destroy(&list_global_updated);
    list_destroy(&list_fixed_updated);
    list_destroy(&list_log_updated);
    list_destroy(&list_token_context_updated);

    list_destroy(&list_global);
    list_destroy(&list_fixed);
    list_destroy(&list_log);
    list_destroy(&list_token_context);
finish:
    return;
}

void diff_records( token_mod_link** differences,
                   tglobal* global_original, tglobal* global_updated,
                   tfixed* fixed_original, tfixed* fixed_updated,
                   tlog* log_original, tlog* log_updated,
                   bool print_diff_info ) {

    token_mod_link* diff_pair = NULL;

    /* We may want to print the timestamps of the records
     * being compared in order to differentiate them, in fact
     * this is an essential part of the process...
     */

    while( diff_pair = pop_tm_link( differences ) ) {
        printf( "Difference found on token [%d]\n",
                diff_pair->original ?
                (unsigned short)diff_pair->original->token :
                diff_pair->modified ?
                (unsigned short)diff_pair->modified->token :
                (unsigned short)-1 );
        printf( "Original:\n" );
        if ( diff_pair->original ) {
            dump_hex( diff_pair->original->buffer,
                      diff_pair->original->length,
                      (size_t)DUMP_BYTES, 
                      (size_t)16 );
        } else {
            printf( "NULL\n" ); 
        }
        printf( "Modified:\n" );
        if ( diff_pair->modified ) {
            dump_hex( diff_pair->modified->buffer,
                      diff_pair->modified->length,
                      (size_t)DUMP_BYTES, 
                      (size_t)16 );
        } else {
            printf( "NULL\n" ); 
        }
    }

    printf( "Global configuration modifications:\n" );
    if ( !diff_global( global_original, global_updated, print_diff_info ) ) {
        printf( "  None.\n" );
    }

    printf( "Fixed configuration modifications:\n" );
    if ( !diff_fixed( fixed_original, fixed_updated, print_diff_info ) ) {
        printf( "  None.\n" );
    }

    printf( "Log configuration modifications:\n" );
    if ( !diff_log( log_original, log_updated, print_diff_info ) ) {
        printf( "  None.\n" );
    }
}

void swap_pointers( void** ptr1, void** ptr2 ) {
    void* temp;

     temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 =  temp;
}

void print_usage( char **args, char *msg ) 
{
    char *exec_name = "ocfdiff";

    if (msg) {
        fprintf( stderr, "%s\n", msg );
    }
    if (args && (*args)) {
        exec_name = *args;
    }

    fprintf( stderr, "usage: %s [options] original_file modified_file\n"
                     "       %s [options] -s file_name\n"
                     "       options:\n"
                     "        -v  Verbose output. Include multiple times\n"
                     "            to increase verbosity.\n",
                     exec_name, exec_name );
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

