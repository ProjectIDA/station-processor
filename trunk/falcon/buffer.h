#include <stdint.h>
#include <string.h>

#ifndef BUFFER_H
#define BUFFER_H

typedef struct _buffer {
    uint8_t* content;
    size_t length;   /* amount of buffer used */
    size_t capacity; /* total capacity of the buffer */
    size_t offset;   /* start next read/write here */
} buffer_t;

buffer_t* buffer_init();

/* free the buffer and all contents
   return new buffer address */
buffer_t* buffer_destroy(buffer_t* buf);

/* delete data and buffer 
   return new buffer address */
void buffer_reset(buffer_t* buf);

/* delete the requested content 
   returns the number of bytes deleted */
size_t buffer_erase(buffer_t* buf, size_t length);

/* seek to 'offset' in buffer
   returns the new offset (unchanged if offset is out of range) */
size_t buffer_seek(buffer_t* buf, size_t offset);
size_t buffer_seek_start(buffer_t* buf);
size_t buffer_seek_end(buffer_t* buf);

/* read data from the buffer,
   'data' should be an existing block of memory large enough 
   to accomodate 'length' bytes
   returns the number of bytes read into 'data' */
size_t buffer_read(buffer_t* buf, uint8_t* data, size_t length); 

/* write data to the buffer, re-sizing the buffer as needed
   returns the number of bytes written */
size_t buffer_write(buffer_t* buf, const uint8_t* data, size_t length);

size_t buffer_size(const buffer_t* buf);
int buffer_terminate(buffer_t* buf);
uint8_t* buffer_detach(buffer_t* buf);

#endif
