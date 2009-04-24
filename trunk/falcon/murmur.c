/* 
 * MurmurHash2 by Austin Appleby
 *
 * These hash functions are intended for in-memory use only!
 * They will return different results depending on the byte
 * ordering of your CPU.
 *
 */

#include <murmur.h>

/* optimized 32-bit hash */
uint32_t murmur_32 ( const void *key, size_t length, uint32_t seed )
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t h;
    uint32_t k;
    const unsigned char* data;

    h = seed ^ length;

    data = (const unsigned char*)key;

    while (length >= 4)
    {
        k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;
        
        h *= m;
        h ^= k;

        data += 4;
        length -= 4;
    }

    switch (length) 
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

/* 64-bit hash optimized for 64-bit architecture */
uint64_t murmur_64_a ( const void *key, size_t length, uint64_t seed )
{
    const uint64_t m = 0xc6a4a7935bd1e995LL;
    const int r = 47;
    uint64_t h;
    uint64_t k;
    const uint64_t* data;
    const uint64_t* end;
    const unsigned char* data2;

    h = seed ^ length;
    data = (const uint64_t*)key;
    end = data + (length / 8);

    while (data != end)
    {
        k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    data2 = (const unsigned char*)data;
    switch (length & 7)
    {
        case 7: h ^= (uint64_t)(data[6]) << 48;
        case 6: h ^= (uint64_t)(data[5]) << 40;
        case 5: h ^= (uint64_t)(data[4]) << 32;
        case 4: h ^= (uint64_t)(data[3]) << 24;
        case 3: h ^= (uint64_t)(data[2]) << 16;
        case 2: h ^= (uint64_t)(data[1]) << 8;
        case 1: h ^= (uint64_t)(data[0]);
                h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

/* 64-bit hash optimized for 32-bit architecture */
uint64_t murmur_64_b ( const void *key, size_t length, uint64_t seed )
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint64_t h;
    uint32_t h1;
    uint32_t h2;
    uint32_t k1;
    uint32_t k2;
    const uint32_t* data;

    h1 = seed ^ length;
    h2 = 0;

    data = (const uint32_t*)key;

    while (length >= 8)
    {
        k1 = *data++;
        k1 *= m;
        k1 ^= k1 >> r;
        k1 *= m;
        h1 *= m;
        h1 ^= k1;
        length -= 4;

        k2 = *data++;
        k2 *= m;
        k2 ^= k2 >> r;
        k2 *= m;
        h2 *= m;
        h2 ^= k2;
        length -= 4;
    }

    if (length >= 4)
    {
        k1 = *data++;
        k1 *= m;
        k1 ^= k1 >> r;
        k1 *= m;
        h1 *= m;
        h1 ^= k1;
        length -= 4;
    }


    switch (length)
    {
        case 3: h ^= ((unsigned char*)data)[2] << 16;
        case 2: h ^= ((unsigned char*)data)[1] << 8;
        case 1: h ^= ((unsigned char*)data)[0];
                h *= m;
    };

    h1 ^= h2 >> 18; h1 *=m;
    h2 ^= h1 >> 22; h2 *=m;
    h1 ^= h2 >> 17; h1 *=m;
    h2 ^= h1 >> 19; h2 *=m;

    h = h1;
    h = (h << 32) | h2;

    return h;
}

