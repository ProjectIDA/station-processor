/*
 *  PComm
 *  Copyright (C) 2010, 2012
 *
 *  Contributor(s):
 *      Joel Edwards <joel.edwards@gmail.com>
 *
 * This software is licensed as "freeware."  Permission to distribute
 * this software in source and binary forms is hereby granted without a
 * fee.  THIS SOFTWARE IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY DAMAGES RESULTING FROM
 * THE USE OF THIS SOFTWARE, EITHER DIRECTLY OR INDIRECTLY, INCLUDING,
 * BUT NOT LIMITED TO, LOSS OF DATA OR DATA BEING RENDERED INACCURATE.
 *
 */
#ifndef __PCOMM_H__
#define __PCOMM_H__

#include <time.h>
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/resource.h>

#include "simclist.h"

#define PCOMM_PAGE_SIZE 4096

#define PCOMM_STDIN  0
#define PCOMM_STDOUT 1
#define PCOMM_STDERR 2

/* Result Codes */
enum PCOMM_RESULT {
    /* 0 */
    PCOMM_SUCCESS = 0,
    PCOMM_INIT_FAILED,
    PCOMM_DESTROY_FAILED,
    PCOMM_FD_OPEN_FAILED,
    PCOMM_FD_READ_FAILED,
    /* 5 */
    PCOMM_FD_WRITE_FAILED,
    PCOMM_FD_CLOSE_FAILED,
    PCOMM_FD_NOT_FOUND,
    PCOMM_FD_NEGATIVE,
    PCOMM_FD_EOF_REACHED,
    /* 10 */
    PCOMM_INDEX_NOT_FOUND,
    PCOMM_LIST_REMOVE_FAILED,
    PCOMM_LIST_BAD_SIZE,
    PCOMM_NO_DATA_FROM_READ,
    PCOMM_NO_DATA_FOR_WRITE,
    /* 15 */
    PCOMM_OUT_OF_MEMORY,
    PCOMM_TERMINATED_BY_ERROR,
    PCOMM_UNINITIALIZED_CONTEXT,
    PCOMM_NULL_CONTEXT,
    PCOMM_NULL_CALLBACK,
    /* 20 */
    PCOMM_NULL_LIST,
    PCOMM_NULL_FD,
    PCOMM_NULL_BUFFER,
    PCOMM_DUPLICATE_FD,
    PCOMM_INVALID_STREAM_TYPE
};
typedef enum PCOMM_RESULT pcomm_result;

/* Default stream identifiers */
enum PCOMM_STREAM {
    PCOMM_STREAM_WRITE,
    PCOMM_STREAM_READ,
    PCOMM_STREAM_ERROR
};
typedef enum PCOMM_STREAM pcomm_stream;

struct pcomm_context_t;

/* pcomm_callback_ready is called when a descriptor is ready for I/O */
typedef void (* pcomm_callback_ready)(struct pcomm_context_t* context, int fd);

/* pcomm_callback_io is called for every I/O event on associated descriptor */
typedef void (* pcomm_callback_io)(struct pcomm_context_t* context, int fd, uint8_t* data, size_t length);

/* pcomm_callback_close is called when the associated descriptor is closed */
typedef void (* pcomm_callback_close)(struct pcomm_context_t* context, int fd);

/* pcomm_callback_timeout is whenever the select operation times out.
 * This allows certain operations to be performed at regular intervals while
 * pcomm retains controll of the overall program flow.
 */
typedef void (* pcomm_callback_timeout)(struct pcomm_context_t* context);

/* The pcomm context object used for managing all file descriptors and program
 * state information.
 */
typedef struct pcomm_context_t {
    list_t read_fds;
    list_t write_fds;
    list_t error_fds;

    int initialized;
    size_t page_size;
    void* user_context;

    pcomm_callback_timeout timeout_callback;
    struct timeval *timeout_ptr;

    int exit_now;
    int exit_request;
} pcomm_context;

/* The file descriptor context object, used for tracking each individual
 * file descriptor.
 */
typedef struct {
    int file_descriptor;
    pcomm_callback_ready ready_callback;
    pcomm_callback_io io_callback;
    pcomm_callback_close close_callback;

    uint8_t* buffer;
    size_t length;
    size_t used;
    size_t offset;
    int last_read_empty;
    int check_only;
} pcomm_fd;


/* Public function definitions  */

/* context creation and destruction */
pcomm_result pcomm_init( pcomm_context* context );
pcomm_result pcomm_destroy( pcomm_context* context );

/* main loop control */
pcomm_result pcomm_main( pcomm_context* context );
pcomm_result pcomm_stop( pcomm_context* context, int immediately );

/* used to maintain the user's context if any */
pcomm_result pcomm_set_user_context( pcomm_context* context, void* user_context );
void* pcomm_get_user_context( pcomm_context* context );

/* sets the function to be called every time the select operation
 * times out
 */
pcomm_result pcomm_set_timeout_callback( pcomm_context* context, 
                                         pcomm_callback_timeout timeout_callback );

/* changes the timeout for the select operation */
pcomm_result pcomm_set_timeout( pcomm_context* context, struct timeval *timeout_ptr );

/* sets the maximum number of bytes to read before returning control to 
 * the select
 */
pcomm_result pcomm_set_page_size( pcomm_context* context, size_t page_size );

/* add a file descriptor to the WRITE list for automatic I/O */
pcomm_result pcomm_add_write_fd( pcomm_context* context, int fd, 
                                 uint8_t* data, size_t length, 
                                 pcomm_callback_io io_callback, 
                                 pcomm_callback_close close_callback );
/* add a file descriptor to the WRITE list for alert */
pcomm_result pcomm_monitor_write_fd( pcomm_context* context, int fd,
                                     pcomm_callback_ready ready_callback,
                                     pcomm_callback_close close_callback );

/* add a file descriptor to the READ list for automatic I/O */
pcomm_result pcomm_add_read_fd(  pcomm_context* context, int fd, 
                                 pcomm_callback_io io_callback, 
                                 pcomm_callback_close close_callback );
/* add a file descriptor to the READ list for alert */
pcomm_result pcomm_monitor_read_fd( pcomm_context* context, int fd,
                                    pcomm_callback_ready ready_callback,
                                    pcomm_callback_close close_callback );

/* add a file descriptor to the ERROR list for automatic I/O */
pcomm_result pcomm_add_error_fd( pcomm_context* context, int fd, 
                                 pcomm_callback_io io_callback, 
                                 pcomm_callback_close close_callback );
/* add a file descriptor to the ERROR list for alert */
pcomm_result pcomm_monitor_error_fd( pcomm_context* context, int fd,
                                    pcomm_callback_ready ready_callback,
                                    pcomm_callback_close close_callback );

pcomm_result pcomm_remove_read_fd(  pcomm_context* context, int fd );
pcomm_result pcomm_remove_write_fd( pcomm_context* context, int fd );
pcomm_result pcomm_remove_error_fd( pcomm_context* context, int fd );

#endif
