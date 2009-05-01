#ifndef MURMUR_H
#define MURMUR_H

#include <stdio.h>
#include <stdint.h>

#define HASH_SEED_32 0xe2588c89L
#define HASH_SEED_64 0xde62d51808ecb849LL

/* optimized 32-bit hash */
uint32_t murmur_32   ( const void *key, size_t length, uint32_t seed );

/* 64-bit hash optimized for 64-bit architecture */
uint64_t murmur_64_a ( const void *key, size_t length, uint64_t seed );

/* 64-bit hash optimized for 32-bit architecture */
uint64_t murmur_64_b ( const void *key, size_t length, uint64_t seed );

#endif

