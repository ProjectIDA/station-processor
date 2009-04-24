#ifndef __RECORD_H__
#define __RECORD_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <simclist.h>

#define DUMP_BYTES 16
#define BLOCK_SIZE 4096
#define MIN_FILE_SIZE 56

#define MAX_RECORDS 16
#define BUFFER_SIZE (BLOCK_SIZE * MAX_RECORDS)
#define REC_TYPE_MAX_LEN 5

#define FLAG_PR_WILLARD 0x00000001L

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
    long record_size;
    bool good;
} seed_file_info;

void get_seed_file_info( char *file_name, 
                         seed_file_info* file_info, 
                         bool print_file_info );

#endif

