/*
 *  PComm
 *  Copyright (C) 2010
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
#include "pcomm.h"

int _list_aid_seeker( const void* element, const void* indicator )
{
    if ( element && indicator && 
         (((pcomm_fd*)element)->file_descriptor == *((int*)indicator)) ) {
        return 1;
    }
    return 0;
}

int _list_aid_comparator( const void* a, const void* b )
{
    if ( a && b ) {
        return ((pcomm_fd*)b)->file_descriptor - ((pcomm_fd*)a)->file_descriptor;
    }
    else if ( a && !b ) 
        return -1;
    else if ( !a && b )
        return 1;
    return 0;
}

pcomm_result _pcomm_make_fd_list( list_t* list, int** fd_list, size_t* length ) {
    pcomm_result result = PCOMM_SUCCESS;
    pcomm_fd* fd_context = NULL;
    size_t list_len = 0;
    int* fds = NULL;

    if ( !list ) {
        result = PCOMM_NULL_LIST;
    } else if ( !fd_list ) {
        result = PCOMM_NULL_FD;
    } else {
        list_len = list_size(list);
        if ( !(*fd_list = (int*)malloc(list_len * sizeof(int))) ) {
            result = PCOMM_OUT_OF_MEMORY;
        } else {
            fds = *fd_list;
            list_iterator_stop(list);
            list_iterator_start(list);
            *length = 0;
            while ( list_iterator_hasnext(list) ) {
                fd_context = list_iterator_next(list);
                if (fd_context) {
                    if ( list_len <= *length ) {
                        result = PCOMM_LIST_BAD_SIZE;
                        free(*fd_list);
                        break;
                    } else {
                        *fds = fd_context->file_descriptor;
                        fds++;
                        (*length)++;
                    }
                }
            }
            list_iterator_stop(list);
        }
    }

    return result;
}

pcomm_result _pcomm_add_input_fd( list_t* list, int fd, 
                                  pcomm_callback_io io_callback,
                                  pcomm_callback_close close_callback ) 
{
    pcomm_fd* fd_context = NULL;
    pcomm_result result = PCOMM_SUCCESS;
    
    if ( fd < 0 ) {
        result = PCOMM_FD_NEGATIVE;
    } else if ( !list ) {
        result = PCOMM_NULL_LIST;
    } else if ( !(fd_context = calloc( sizeof(pcomm_fd), 1 )) ) {
        result = PCOMM_OUT_OF_MEMORY;
    } else {
        fd_context->file_descriptor = fd;
        fd_context->io_callback = io_callback;
        fd_context->close_callback = close_callback;
        fd_context->buffer   = NULL;
        fd_context->length   = 0;
        fd_context->used     = 0;
        fd_context->offset   = 0;
        fd_context->last_read_empty = 0;

        list_append( list, fd_context );
    }
    
    return result;
}

pcomm_result _pcomm_add_output_fd( list_t* list, int fd, uint8_t* data, 
                                   size_t length, 
                                   pcomm_callback_io io_callback,
                                   pcomm_callback_close close_callback ) 
{
    pcomm_fd* fd_context = NULL;
    pcomm_result result = PCOMM_SUCCESS;
    uint8_t* new_buffer = NULL;

    if ( fd < 0 ) {
        result = PCOMM_FD_NEGATIVE;
    } else if ( !list ) {
        result = PCOMM_NULL_LIST;
    } else {
        if ( (fd_context = (pcomm_fd*)list_seek( list, &fd )) ) {
            if ( !fd_context ) {
                result = PCOMM_NULL_FD;
            } else if ( !fd_context->buffer ) {
                result = PCOMM_NULL_BUFFER;
            } else {
                if ( !data || !length ) {
                    result = PCOMM_NO_DATA_FOR_WRITE;
                } else if ( !(new_buffer = (uint8_t*)realloc(fd_context->buffer, fd_context->used + length)) ) {
                    result = PCOMM_OUT_OF_MEMORY;
                } else {
                    fd_context->buffer = new_buffer;
                    memcpy( fd_context->buffer + fd_context->used, data, length );
                    fd_context->length = fd_context->used + length;
                    fd_context->used = fd_context->used + length;
                }
            }
        } else if ( !(fd_context = calloc( sizeof(pcomm_fd), 1 )) ) {
            result = PCOMM_OUT_OF_MEMORY;
        } else {
            fd_context->file_descriptor = fd;
            fd_context->io_callback = io_callback;
            fd_context->close_callback = close_callback;
            fd_context->buffer   = NULL;
            fd_context->length   = 0;
            fd_context->used     = 0;
            fd_context->offset   = 0;

            if ( !data || !length ) {
                result = PCOMM_NO_DATA_FOR_WRITE;
            } else if ( !(fd_context->buffer = (uint8_t*)malloc(length)) ) {
                result = PCOMM_OUT_OF_MEMORY;
                free(fd_context);
                fd_context = NULL;
            } else {
                memcpy( fd_context->buffer, data, length );
                fd_context->length = length;
                fd_context->used = length;
                if ( list_append( list, fd_context ) < 0 ) {
                    fprintf(stderr, "Failed to add output descriptor to list.\n");
                }
            }
        }
    }
    
    return result;
}

pcomm_fd* _pcomm_get_fd( list_t* list, int fd ) 
{
    pcomm_fd* fd_context = NULL;

    if ( (fd >= 0) && (list) ) {
        fd_context = (pcomm_fd*)list_seek( list, &fd );
    } 

    return fd_context;
}

pcomm_result _pcomm_remove_fd( list_t* list, int fd ) 
{
    pcomm_result result = PCOMM_SUCCESS;
    int index = -1;
    int delete_result = -1;
    pcomm_fd* fd_context;

    if ( !list ) {
        result = PCOMM_NULL_LIST;
    } else if ( fd < 0 ) {
        result = PCOMM_FD_NEGATIVE;
    } else if ( ! (fd_context = (pcomm_fd*)list_seek(list, &fd)) ) {
        result = PCOMM_FD_NOT_FOUND;
    } else if ( (index = list_locate(list, fd_context)) < 0 ) {
        result = PCOMM_INDEX_NOT_FOUND;
    } else if ( (delete_result = list_delete_at(list, (unsigned int)index)) < 0 ) {
        result = PCOMM_LIST_REMOVE_FAILED;
    } 
    
    return result;
}

pcomm_result _pcomm_remove_fd_type( pcomm_context* context, int fd, pcomm_stream type )
{
    pcomm_result result = PCOMM_SUCCESS;
    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else {
        if (type == PCOMM_STREAM_WRITE) {
            result = _pcomm_remove_fd(&context->write_fds, fd);
        } else if (type == PCOMM_STREAM_READ) {
            result = _pcomm_remove_fd(&context->read_fds, fd);
        } else if (type == PCOMM_STREAM_ERROR) {
            result = _pcomm_remove_fd(&context->error_fds, fd);
        } else {
            result = PCOMM_INVALID_STREAM_TYPE;
        }
    }
    return result;
}

pcomm_result _pcomm_empty_list( list_t* list ) {
    pcomm_result result = PCOMM_SUCCESS;
    pcomm_fd* fd_context = NULL;

    if ( !list ) {
        result = PCOMM_NULL_LIST;
    } else {
        list_iterator_stop(list); 
        list_iterator_start(list); 
        while ( list_iterator_hasnext(list) ) {
            fd_context = list_iterator_next(list);
            if ( fd_context ) {
                if ( fd_context->buffer )
                    free( fd_context->buffer );
                free( fd_context );
            }
        }
        list_iterator_stop(list); 
        list_clear(list);
    }

    return result;
}

int _pcomm_writes_buffered( pcomm_context* context ) { 
    int result = 0;
    /*
    pcomm_fd* fd_context = NULL;
    */

    if ( context ) {
        /* This assumes that all writes are being handled correctly */
        if ( !list_empty(&context->write_fds) ) {
            result = 1;
        }
        /*
        list_iterator_stop(&context->write_fds); 
        list_iterator_start(&context->write_fds); 
        while ( list_iterator_hasnext(&context->write_fds) ) {
            fd_context = list_iterator_next(&context->write_fds);
            if ( fd_context ) {
                if ( fd_context->buffer && 
                     fd_context->used   &&
                    (fd_context->offset <= fd_used) ) {
                    result = 1;
                    break;
                }
            }
        }
        list_iterator_stop(&context->write_fds); 
        */
    }

    return result;
}

int _pcomm_populate_set( list_t* list, fd_set* set ) 
{
    int max_fd = -1;
    int tmp_fd = -1;
    pcomm_fd* fd_context = NULL; 

    FD_ZERO( set );
    if ( list && set ) {
        list_iterator_stop(list); 
        list_iterator_start(list); 
        while ( list_iterator_hasnext(list) ) {
            fd_context = list_iterator_next(list);
            if ( fd_context ) {
                tmp_fd = fd_context->file_descriptor;
                if ( tmp_fd >= 0 ) {
                    if (tmp_fd > max_fd) {
                        max_fd = tmp_fd;
                    }
                    FD_SET( tmp_fd, set );
                }
            }
        }
        list_iterator_stop(list); 
    }
    return max_fd;
}

pcomm_result _pcomm_clean_read_buffer( pcomm_fd* fd_context )
{
    pcomm_result result = PCOMM_SUCCESS;

    if ( !fd_context ) {
        result = PCOMM_NULL_CONTEXT;
    } else {
        if ( fd_context->buffer )
            free( fd_context->buffer );
        fd_context->buffer = NULL;
        fd_context->length = 0;
        fd_context->used   = 0;
    }

    return result;
}

pcomm_result _read_fd( pcomm_fd* fd_context )
{
    pcomm_result result = PCOMM_SUCCESS;
    uint8_t* new_buffer = NULL;
    uint8_t buffer[PCOMM_PAGE_SIZE];
    size_t read_count = 0;

    if ( !fd_context) {
        result = PCOMM_NULL_CONTEXT;
    } else if ( (read_count = read(fd_context->file_descriptor, buffer, PCOMM_PAGE_SIZE)) <= 0 ) {
        result = PCOMM_NO_DATA_FROM_READ;
    } else if ( !(new_buffer = realloc( fd_context->buffer, fd_context->used + read_count )) ) {
        result = PCOMM_OUT_OF_MEMORY;
    } else {
        fd_context->buffer = new_buffer;
        memcpy( fd_context->buffer + fd_context->used, buffer, read_count );
        fd_context->length = fd_context->used + read_count;
        fd_context->used = fd_context->used + read_count;
    }

    return result;
}

pcomm_result _write_fd( pcomm_fd* fd_context )
{
    pcomm_result result = PCOMM_SUCCESS;
    uint8_t* new_buffer = NULL;
    size_t write_count = 0;

    if ( !fd_context ) { 
        result = PCOMM_NULL_CONTEXT;
    } else if ( !fd_context->buffer ) { 
        result = PCOMM_NULL_BUFFER;
    } else if ( !fd_context->used ) {
        result = PCOMM_NO_DATA_FOR_WRITE;
    } else if ( !(write_count = write(fd_context->file_descriptor, fd_context->buffer, fd_context->used)) ) {
        result = PCOMM_FD_WRITE_FAILED;
    } else {
        /* Update the number of bytes left, if empty, null out buffer */
        if ( write_count == fd_context->used ) {
            free(fd_context->buffer);
            fd_context->buffer = NULL;
            fd_context->length = 0;
            fd_context->used = 0;
        } else {
            fd_context->used -= write_count;
            memmove(fd_context->buffer, fd_context->buffer + write_count, fd_context->used);
            if ( (new_buffer = realloc(fd_context->buffer, fd_context->used)) ) {
                fd_context->buffer = new_buffer;
                fd_context->length = fd_context->used;
            }
        }
    }

    return result;
}

/* The real magic happens here */
pcomm_result _pcomm_loop( pcomm_context* context ) {
    pcomm_result result = PCOMM_SUCCESS;
    pcomm_result io_result;
    int read_max, write_max, error_max;
    int max_fd;
    int num_fds;
    pcomm_fd* fd_context;

    int i = 0;
    int* fds = NULL;
    size_t fds_len = 0;

    fd_set read_set;
    fd_set write_set;
    fd_set error_set;

    fd_set* read_set_ptr;
    fd_set* write_set_ptr;
    fd_set* error_set_ptr;

    if ( !context ) {
        fprintf( stderr, "Context null!\n" );
        return PCOMM_NULL_CONTEXT;
    }

    while ( !context->exit_now ) {
        if ( context->exit_request && !_pcomm_writes_buffered( context ) ) {
            break;
        }

        max_fd = -1;
        write_max = _pcomm_populate_set( &context->write_fds, &write_set );
        if ( write_max > max_fd ) max_fd = write_max;
        read_max = _pcomm_populate_set( &context->read_fds, &read_set );
        if ( read_max > max_fd ) max_fd = read_max;
        error_max = _pcomm_populate_set( &context->error_fds, &error_set );
        if ( error_max > max_fd ) max_fd = error_max;

        if ( max_fd < 0 ) {
            result = PCOMM_FD_NOT_FOUND;
            break;
        }

        write_set_ptr = (write_max >= 0) ? &write_set : NULL;
        read_set_ptr  = (read_max  >= 0) ? &read_set  : NULL;
        error_set_ptr = (error_max >= 0) ? &error_set : NULL;

        num_fds = select( max_fd + 1, read_set_ptr, write_set_ptr, error_set_ptr, context->timeout_ptr );
        if ( num_fds < 0 ) {
            context->exit_now = 1;
            switch (errno) {
                case EBADF:
                    fprintf( stderr, "Bad file descriptor\n" );
                    break;
                case EINTR:
                    fprintf( stderr, "Caught interrupt signal\n" );
                    break;
                case EINVAL:
                    fprintf( stderr, "Invalid timeout or max fd\n" );
                    break;
                case ENOMEM:
                    fprintf( stderr, "Out of memory\n" );
                    break;
                default:
                    fprintf( stderr, "Unknown error with select\n" );
                    context->exit_now = 0;
            }
        } else if ( num_fds == 0 ) {
            if (context->timeout_callback) {
                context->timeout_callback(context);
            }
        } else {
            /* ERROR */
            fd_context = NULL;
            if ( !_pcomm_make_fd_list(&context->error_fds, &fds, &fds_len) ) {
                for ( i=0; i<fds_len; i++ ) {
                    if ( FD_ISSET(fds[i], error_set_ptr) ) {
                        if ( (fd_context = (pcomm_fd*)list_seek(&context->error_fds, &fds[i])) ) {
                            io_result = _read_fd( fd_context );
                            if ( !io_result ) {
                                fd_context->last_read_empty = 0;
                                if ( fd_context->io_callback ) {
                                    fd_context->io_callback( context, 
                                                             fd_context->file_descriptor, 
                                                             fd_context->buffer, 
                                                             fd_context->used );
                                    _pcomm_clean_read_buffer( fd_context );
                                }
                            } else if (io_result == PCOMM_NO_DATA_FROM_READ) {
                                if (fd_context->last_read_empty) {
                                    pcomm_remove_read_fd(context, fd_context->file_descriptor);
                                    if (fd_context->close_callback) {
                                        fd_context->close_callback(context, fd_context->file_descriptor);
                                    }
                                } else {
                                    fd_context->last_read_empty = 1;
                                }
                            }
                        }
                    }
                }
                free(fds);
                fds = NULL;
                fds_len = 0;
            }

            /* WRITE */
            fd_context = NULL;
            if ( !_pcomm_make_fd_list(&context->write_fds, &fds, &fds_len) ) {
                for ( i=0; i<fds_len; i++ ) {
                    if ( FD_ISSET(fds[i], write_set_ptr) ) {
                        if ( (fd_context = (pcomm_fd*)list_seek(&context->write_fds, &fds[i])) ) {
                            io_result = _write_fd( fd_context );
                            if ( fd_context->io_callback ) {
                                fd_context->io_callback( context, 
                                                      fd_context->file_descriptor,
                                                      NULL, 0);
                            }
                            if ( !fd_context->buffer || !fd_context->used ) {
                                _pcomm_remove_fd( &context->write_fds, 
                                                  fd_context->file_descriptor );
                                if (fd_context->close_callback) {
                                    fd_context->close_callback(context, fd_context->file_descriptor);
                                }
                                free(fd_context);
                            }
                        }
                    }
                }
                free(fds);
                fds = NULL;
                fds_len = 0;
            }

            /* READ */
            fd_context = NULL;
            if ( !_pcomm_make_fd_list(&context->read_fds, &fds, &fds_len) ) {
                for ( i=0; i<fds_len; i++ ) {
                    if ( FD_ISSET(fds[i], read_set_ptr) ) {
                        if ( (fd_context = (pcomm_fd*)list_seek(&context->read_fds, &fds[i])) ) {
                            io_result = _read_fd( fd_context );
                            if (!io_result) {
                                fd_context->last_read_empty = 0;
                                if (fd_context->io_callback ) {
                                    fd_context->io_callback( context, 
                                                          fd_context->file_descriptor, 
                                                          fd_context->buffer, 
                                                          fd_context->used );
                                    _pcomm_clean_read_buffer( fd_context );
                                }
                            } else if (io_result == PCOMM_NO_DATA_FROM_READ) {
                                if (fd_context->last_read_empty) {
                                    pcomm_remove_read_fd(context, fd_context->file_descriptor);
                                    if (fd_context->close_callback) {
                                        fd_context->close_callback(context, fd_context->file_descriptor);
                                    }
                                } else {
                                    fd_context->last_read_empty = 1;
                                }
                            }
                        }
                    }
                }
                free(fds);
                fds = NULL;
                fds_len = 0;
            }
        }
    }

    return result;
}

pcomm_result pcomm_init( pcomm_context* context )
{
    pcomm_result result = PCOMM_SUCCESS;

    if ( !context ) {
        result = PCOMM_NULL_CONTEXT;
    }
    else if ( list_init( &context->read_fds  ) || 
              list_init( &context->write_fds ) || 
              list_init( &context->error_fds ) ) {
        result = PCOMM_INIT_FAILED;
    } else {
        list_attributes_seeker( &context->read_fds,  _list_aid_seeker );
        list_attributes_seeker( &context->write_fds, _list_aid_seeker );
        list_attributes_seeker( &context->error_fds, _list_aid_seeker );
        list_attributes_comparator( &context->read_fds,  _list_aid_comparator );
        list_attributes_comparator( &context->write_fds, _list_aid_comparator );
        list_attributes_comparator( &context->error_fds, _list_aid_comparator );
        context->page_size = PCOMM_PAGE_SIZE;
        context->timeout_callback = NULL;
        context->timeout_ptr = NULL;
        context->initialized = 1;
        context->exit_request = 0;
        context->exit_now = 0;
        context->user_context = NULL;
    }

    return result;
}

pcomm_result pcomm_destroy( pcomm_context* context )
{
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else {
        if (context->initialized) {
            context->initialized = 0;
            context->user_context = NULL;
            _pcomm_empty_list( &context->read_fds );
            _pcomm_empty_list( &context->write_fds );
            _pcomm_empty_list( &context->error_fds );
            list_destroy( &context->read_fds );
            list_destroy( &context->write_fds );
            list_destroy( &context->error_fds );
        }
    }
    return result;
}

pcomm_result pcomm_main( pcomm_context* context ) 
{
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else {
        result = _pcomm_loop( context );
    }

    return result;
}

pcomm_result pcomm_stop( pcomm_context* context, int immediately ) 
{
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else {
        if ( immediately ) {
            context->exit_now = 1;
        } else {
            context->exit_request = 1;
        }
    }

    return result;
}

pcomm_result pcomm_set_user_context( pcomm_context* context, void* user_context ) {
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else {
        context->user_context = user_context;
    }

    return result;
}

void* pcomm_get_user_context( pcomm_context* context ) {
    void* user_context = NULL;
    if (context && context->initialized)
        user_context = context->user_context;
    return user_context;
}

pcomm_result pcomm_set_page_size( pcomm_context* context, size_t page_size )
{
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else {
        context->page_size = page_size;
    }

    return result;
}

pcomm_result pcomm_set_timeout( pcomm_context* context, struct timeval *timeout_ptr )
{
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else {
        context->timeout_ptr = timeout_ptr;
    }

    return result;
}

pcomm_result pcomm_set_timeout_callback( pcomm_context* context, 
                                         pcomm_callback_timeout timeout_callback )
{
    pcomm_result result = PCOMM_SUCCESS;

    if (!context) {
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else {
        context->timeout_callback = timeout_callback;
    }

    return result;
}

pcomm_result pcomm_add_write_fd( pcomm_context* context, int fd, 
                                 uint8_t* data, size_t length, 
                                 pcomm_callback_io io_callback, 
                                 pcomm_callback_close close_callback )
{ 
    pcomm_result result = PCOMM_SUCCESS;

    if ( !context ) { 
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else if (!data || !length) {
        result = PCOMM_NO_DATA_FOR_WRITE;
    } else {
        result = _pcomm_add_output_fd( &context->write_fds, fd, data, length, io_callback, close_callback ); 
    }

    return result;
}

pcomm_result pcomm_add_read_fd( pcomm_context* context, int fd, 
                                pcomm_callback_io io_callback, 
                                pcomm_callback_close close_callback ) 
{ 
    pcomm_result result = PCOMM_SUCCESS;

    if ( !context ) { 
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else if ( !io_callback ) {
        result = PCOMM_NULL_CALLBACK;
    } else {
        result = _pcomm_add_input_fd( &context->read_fds, fd, io_callback, close_callback ); 
    }

    return result;
}

pcomm_result pcomm_add_error_fd( pcomm_context* context, int fd, 
                                 pcomm_callback_io io_callback, 
                                 pcomm_callback_close close_callback )
{ 
    pcomm_result result = PCOMM_SUCCESS;

    if ( !context ) { 
        result = PCOMM_NULL_CONTEXT;
    } else if (!context->initialized) {
        result = PCOMM_UNINITIALIZED_CONTEXT;
    } else if ( !io_callback ) {
        result = PCOMM_NULL_CALLBACK;
    } else {
        result = _pcomm_add_input_fd( &context->error_fds, fd, io_callback, close_callback ); 
    }

    return result;
}

pcomm_result pcomm_remove_write_fd( pcomm_context* context, int fd )
{ 
    pcomm_result result = PCOMM_SUCCESS;
    result = _pcomm_remove_fd_type(context, fd, PCOMM_STREAM_WRITE);
    return result;
}

pcomm_result pcomm_remove_read_fd( pcomm_context* context, int fd )
{ 
    pcomm_result result = PCOMM_SUCCESS;
    result = _pcomm_remove_fd_type(context, fd, PCOMM_STREAM_READ);
    return result;
}

pcomm_result pcomm_remove_error_fd( pcomm_context* context, int fd )
{ 
    pcomm_result result = PCOMM_SUCCESS;
    result = _pcomm_remove_fd_type(context, fd, PCOMM_STREAM_ERROR);
    return result;
}

