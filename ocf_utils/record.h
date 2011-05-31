#ifndef __RECORD_H__
#define __RECORD_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <simclist.h>
#include <lib330.h>
#include <tokens.h>

#define DUMP_BYTES 16
#define RECORD_SIZE 512

#define MAX_RECORDS 16
#define BUFFER_SIZE (RECORD_SIZE * MAX_RECORDS)
#define REC_TYPE_MAX_LEN 5

#define PF_GLOBAL   0x01
#define PF_FIXED    0x02
#define PF_LOG      0x04
#define PF_TOKENS   0x08
#define PF_COMPLETE 0x0f

#define FLAG_PR_WILLARD 0x00000001L

#define PRINT_WILLARD(x) (x & FLAG_PR_WILLARD)
#define SET_PRINT_WILLARD(x) (x |= FLAG_PR_WILLARD)

typedef struct {
    size_t offset;
    bool eof_reached;
    uint8_t flags;
    tseed_time timestamp;
} file_context;

typedef struct {
    tseed_time time;
    void *data;
} list_metadata;

typedef struct {
    tseed_time start_time;
    tseed_time end_time;
    long length;
    bool good;
} seed_file_info;

void get_seed_file_info( char *file_name, 
                         seed_file_info* file_info, 
                         bool print_file_info );

void diff_list_global( list_t* list, bool print_diff );
void diff_list_fixed( list_t* list, bool print_diff );
void diff_list_log( list_t* list, bool print_diff );
void diff_list_tokens( list_t* list, bool print_diff );

int diff_global( tglobal* original, tglobal* updated, bool print_diff );
int diff_fixed( tfixed* original, tfixed* updated, bool print_diff );
int diff_log( tlog* original, tlog* updated, bool print_diff );

void populate_records( char* file_name, 
                       list_t* list_global, 
                       list_t* list_fixed,
                       list_t* list_log, 
                       list_t* list_token_context,
                       bool print_file_info );
bool done_reading( file_context* context );
bool data_complete( file_context* context );
int cmp_times( tseed_time* time1, tseed_time* time2 );

/* List support functions */
int metadata_compare( const void *data1, const void *data2 );
/*size_t metadata_metric( const void *data );*/

void print_seed_header( seed_header* header );
void print_seed_time( const char* prefix_msg, tseed_time* time, const char* postfix_msg );
void print_global( tglobal *opaque_global, size_t depth, uint32_t pr_flags );
void print_fixed( tfixed *opaque_fixed, size_t depth, uint32_t pr_flags );
void print_log( tlog *opaque_log, size_t depth, uint32_t pr_flags );
void print_tokens( token_context *context, size_t depth, uint32_t pr_flags );
void print_dss( tdss *dss, size_t depth );
void print_tdetload( tdetload *detector, size_t depth );
void print_tdispatch( tdispatch *disp, size_t depth );
void print_tmdispatch( tmdispatch *disp, size_t depth );

/* Chain printing functions */
void print_lcq_list( tlcq *lcq, bool dispatch, size_t depth );
void print_cdetector_list( tcontrol_detector *head, size_t depth );
void print_detector_list( tdetector *head, size_t depth );
void print_tiirdef_list( tiirdef *head, size_t depth );
void print_tdop_list( tdop *head, size_t depth );
void print_tdetector_operation_list ( tdetector_operation *detopt, size_t depth);
void print_mholdqtype_list( mholdqtype *head, size_t depth );
void print_tfilter_list( tfilter *head, size_t depth );
void print_tcompressed_buffer_ring_list( tcompressed_buffer_ring *head, size_t depth );

#endif

