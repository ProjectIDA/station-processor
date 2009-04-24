#ifndef MURMUR_H
#define MURMUR_H

#include <stdio.h>
#include <stdint.h>

uint32_t murmur_32   ( const void *key, size_t length, uint32_t seed );
uint64_t murmur_64_a ( const void *key, size_t length, uint64_t seed );
uint64_t murmur_64_b ( const void *key, size_t length, uint64_t seed );

#endif

