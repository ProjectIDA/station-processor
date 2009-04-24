#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"

buffer_t* buffer_init()
{
    buffer_t* buf = NULL;
    buf = (buffer_t*)malloc(sizeof(buffer_t));
    if (buf) {
        buf->content = NULL;
        buf->capacity = 0;
        buf->length = 0;
        buf->offset = 0;
    }
    return buf;
}

buffer_t* buffer_destroy(buffer_t* buf)
{
    if (buf) {
        buffer_reset(buf);
        free(buf);
        buf = NULL;
    }
    return buf;
}

void buffer_reset(buffer_t* buf)
{
    if (buf) {
        if (buf->content) {
            free(buf->content);
            buf->content = NULL;
        }
        buf->capacity = 0;
        buf->length = 0;
        buf->offset = 0;
    }
}

int buffer_good(const buffer_t* buf)
{
    if (!buf) return 0;
    if (buf->length > buf->capacity) return 0;
    if (buf->offset > buf->length) return 0;
    return 1;
}

size_t buffer_erase(buffer_t* buf, size_t length)
{
    size_t bytes_erased = 0;
    uint8_t* tmp_buf = NULL;
    uint8_t* src = NULL;
    uint8_t* dst = NULL;
    size_t copy_length = 0;
    size_t new_length = 0;

    if ( buffer_good(buf) && length && buf->content &&
         ((buf->length - buf->offset) >= length) ) {
        dst = buf->content + buf->offset;
        src = buf->content + buf->offset + length;
        copy_length = buf->length - (buf->offset + length);
        new_length = buf->length - length;
        memmove(dst, src, copy_length);
        tmp_buf = (uint8_t*)realloc(buf->content, new_length);
        if (tmp_buf) {
            buf->content = tmp_buf;
            buf->capacity = new_length;
            buf->length = new_length;
            buffer_seek_end(buf);
        }
    }
    return bytes_erased;
}

/* seek to 'offset' in buffer
   returns the new offset (unchanged if offset is out of range) */
size_t buffer_seek(buffer_t* buf, size_t offset)
{
    size_t new_offset = 0;
    if (buffer_good(buf)) {
        if (offset <= buf->length) {
            new_offset = buf->offset = offset;
        }
    }
    return new_offset;
}

size_t buffer_seek_start(buffer_t* buf)
{
    return buffer_seek(buf, 0);
}

size_t buffer_seek_end(buffer_t* buf)
{
    size_t new_offset = 0;
    if (buffer_good(buf)) {
        new_offset = buf->offset = buf->length;
    }
    return new_offset;
}

/* read data from the buffer,
   'dst' should be an existing block of memory large enough 
   to accomodate 'length' bytes
   returns the number of bytes read into 'dst' */
size_t buffer_read(buffer_t* buf, uint8_t* dst, size_t length) 
{
    size_t bytes_read = 0;
    uint8_t* src = NULL;
    size_t src_len = 0;

    if (buffer_good(buf) && dst && length) {
        if (buf->length > buf->offset) {
            src_len = buf->length - buf->offset;
        }
        if (buf->content && src_len) {
            src = buf->content + buf->offset;
            memcpy(dst, src, src_len);
            bytes_read = src_len;
        }
    }
    return bytes_read;
}

/* write data to the buffer re-sizing the buffer as needed
   returns the number of bytes written */
size_t buffer_write(buffer_t* buf, const uint8_t* src, size_t length) 
{
    size_t bytes_written = 0;
    uint8_t* tmp_buf = NULL;
    uint8_t* dst = NULL;
    size_t new_length = 0;
    size_t new_capacity = 0;

    if (buffer_good(buf) && src && length) {
        new_capacity = buf->capacity;
        new_length = buf->offset + length;
        if (new_length < buf->length) {
            new_length = buf->length;
        }
        if (buf->content && (new_length <= buf->capacity)) {
            tmp_buf = buf->content;
        } else {
            new_capacity = new_length;
            tmp_buf = (uint8_t*)realloc(buf->content, new_capacity);
        }
        if (tmp_buf) {
            buf->content = tmp_buf;
            dst = buf->content + buf->offset;
            memcpy(dst, src, length);
            bytes_written = length;
            buf->length = new_length;
            buf->offset += bytes_written;
            buf->capacity = new_capacity;
        }
    }
    return bytes_written;
}

size_t buffer_size(const buffer_t* buf)
{
    size_t size = 0;
    if (buffer_good(buf)) {
        size = buf->length;
    }
    return size;
}

int buffer_terminate(buffer_t* buf) {
    int result = 0;
    uint8_t terminator = 0;
    result = (int)buffer_write(buf, &terminator, sizeof(terminator));
    return result;
}

uint8_t* buffer_detach(buffer_t* buf)
{
    uint8_t* buf_content = NULL;
    if (buffer_good(buf)) {
        buf_content = buf->content;
        buf->content = NULL;
        buffer_reset(buf);
    }
    return buf_content;
}

