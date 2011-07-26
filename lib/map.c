/*
 *    map
 *
 *    Generic hash table with string based keys.
 *
 *    map.c
 *
 *    Derived from strmap version 2.0.0 (Copyright (c) 2009, 2011 Per Ola Kristensson)
 *      Per Ola Kristensson <pok21@cam.ac.uk>
 *
 *    Maintainer of map:
 *    Joel D. Edwards <jdedwards@usgs.gov>
 *
 *    map is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    map is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with strmap.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#include "include/map.h"

typedef struct Pair Pair;
typedef struct Bucket Bucket;

struct Pair {
    char *key;
    void *value;
};

struct Bucket {
    unsigned int count;
    Pair *pairs;
};

struct Map {
    unsigned int count;
    map_value_free_func free_func;
    map_value_dup_func dup_func;
    Bucket *buckets;
};

static Pair *get_pair(Bucket *bucket, const char *key);
static int   remove_pair(Bucket *bucket, const char *key);
static unsigned long hash(const char *str);

Map *map_new(unsigned int capacity, map_value_free_func free_func, map_value_dup_func dup_func)
{
    Map *map;

    map = malloc(sizeof(Map));
    if (map == NULL) {
        return NULL;
    }
    map->count = capacity;
    map->free_func = free_func;
    map->dup_func = dup_func;
    map->buckets = malloc(map->count * sizeof(Bucket));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }
    memset(map->buckets, 0, map->count * sizeof(Bucket));
    return map;
}

void map_delete(Map *map)
{
    unsigned int i, j, n, m;
    Bucket *bucket;
    Pair *pair;

    if (map == NULL) {
        return;
    }
    n = map->count;
    bucket = map->buckets;
    i = 0;
    while (i < n) {
        m = bucket->count;
        pair = bucket->pairs;
        j = 0;
        while(j < m) {
            free(pair->key);
            if (map->free_func != 0) {
                map->free_func(pair->value);
            }
            pair++;
            j++;
        }
        free(bucket->pairs);
        bucket++;
        i++;
    }
    free(map->buckets);
    free(map);
}

void *map_get(const Map *map, const char *key)
{
    unsigned int index;
    Bucket *bucket;
    Pair *pair;

    if (map == NULL) {
        return NULL;
    }
    if (key == NULL) {
        return NULL;
    }
    index = hash(key) % map->count;
    bucket = &(map->buckets[index]);
    pair = get_pair(bucket, key);
    if (pair == NULL) {
        return NULL;
    }
    return pair->value;
}

int map_exists(const Map *map, const char *key)
{
    return (map_get(map, key) != NULL);
}

int map_remove(const Map *map, const char *key)
{
    unsigned int index;
    Bucket *bucket;

    if (map == NULL) {
        return 0;
    }
    if (key == NULL) {
        return 0;
    }

    index = hash(key) % map->count;
    bucket = &(map->buckets[index]);
    return remove_pair(bucket, key);
}

int map_put(Map *map, const char *key, void *value)
{
    unsigned int key_len, index;
    Bucket *bucket;
    Pair *tmp_pairs, *pair;
    char *new_key;
    void *new_value;
    void *old_value;

    if (map == NULL) {
        return 0;
    }
    if (key == NULL) {
        return 0;
    }
    key_len = strlen(key);
    /* Get a pointer to the bucket to which the key string hashes */
    index = hash(key) % map->count;
    bucket = &(map->buckets[index]);
    /* Check if we can handle insertion by simply replacing
     * an existing value in a key-value pair in the bucket.
     */
    if ((pair = get_pair(bucket, key)) != NULL) {
        /* The bucket contains a pair that matches the provided key,
         * change the value for that pair to the new value.
         */
        old_value = pair->value;
        if (map->dup_func != NULL) {
            if (value != NULL) {
                new_value = map->dup_func(value);
                if (new_value == NULL) {
                    return 0;
                }
            } else {
                new_value = NULL;
            }
            pair->value = new_value;
        } else {
            pair->value = value;
        }
        if (map->free_func != NULL) {
            map->free_func(old_value);
        }
        return 1;
    }
    /* Allocate space for a new key and value */
    new_key = malloc((key_len + 1) * sizeof(char));
    if (new_key == NULL) {
        return 0;
    }
    if (map->dup_func != NULL) {
        new_value = map->dup_func(value);
        if (new_value == NULL) {
            free(new_key);
            return 0;
        }
    } else {
        new_value = value;
    }
    /* Create a key-value pair */
    if (bucket->count == 0) {
        /* The bucket is empty, lazily allocate space for a single
         * key-value pair.
         */
        bucket->pairs = malloc(sizeof(Pair));
        if (bucket->pairs == NULL) {
            free(new_key);
            if (map->free_func != NULL) {
                map->free_func(new_value);
            }
            return 0;
        }
        bucket->count = 1;
    }
    else {
        /* The bucket wasn't empty but no pair existed that matches the provided
         * key, so create a new key-value pair.
         */
        tmp_pairs = realloc(bucket->pairs, (bucket->count + 1) * sizeof(Pair));
        if (tmp_pairs == NULL) {
            free(new_key);
            if (map->free_func != NULL) {
                map->free_func(new_value);
            }
            return 0;
        }
        bucket->pairs = tmp_pairs;
        bucket->count++;
    }
    /* Get the last pair in the chain for the bucket */
    pair = &(bucket->pairs[bucket->count - 1]);
    pair->key = new_key;
    pair->value = new_value;
    /* Copy the key and its value into the key-value pair */
    strcpy(pair->key, key);
    return 1;
}

int map_get_count(const Map *map)
{
    unsigned int i, j, n, m;
    unsigned int count;
    Bucket *bucket;
    Pair *pair;

    if (map == NULL) {
        return 0;
    }
    bucket = map->buckets;
    n = map->count;
    i = 0;
    count = 0;
    while (i < n) {
        pair = bucket->pairs;
        m = bucket->count;
        j = 0;
        while (j < m) {
            count++;
            pair++;
            j++;
        }
        bucket++;
        i++;
    }
    return count;
}

int map_enum(const Map *map, map_enum_func enum_func, const void *obj)
{
    unsigned int i, j, n, m;
    Bucket *bucket;
    Pair *pair;

    if (map == NULL) {
        return 0;
    }
    if (enum_func == NULL) {
        return 0;
    }
    bucket = map->buckets;
    n = map->count;
    i = 0;
    while (i < n) {
        pair = bucket->pairs;
        m = bucket->count;
        j = 0;
        while (j < m) {
            enum_func(pair->key, pair->value, obj);
            pair++;
            j++;
        }
        bucket++;
        i++;
    }
    return 1;
}

/*
 * Returns a pair from the bucket that matches the provided key,
 * or null if no such pair exist.
 */
static Pair * get_pair(Bucket *bucket, const char *key)
{
    unsigned int i, n;
    Pair *pair;

    n = bucket->count;
    if (n == 0) {
        return NULL;
    }
    pair = bucket->pairs;
    i = 0;
    while (i < n) {
        if (pair->key != NULL) {
            if (strcmp(pair->key, key) == 0) {
                return pair;
            }
        }
        pair++;
        i++;
    }
    return NULL;
}

static int remove_pair(Bucket *bucket, const char *key)
{
    unsigned int i, n, moves, found;
    Pair *pair;
    Pair *new_pairs;

    n = bucket->count;
    if (n == 0) {
        return 0;
    }
    pair = bucket->pairs;
    i = 0;
    moves = 0;
    found = 0;
    while (i < n) {
        /* shuffle down for each NULL or match */
        if ((pair->key == NULL)) {
            memmove(pair, pair+sizeof(Pair), (n - i - 1) * sizeof(Pair));
            moves++;
        } else if (strcmp(pair->key, key) == 0) {
            memmove(pair, pair+sizeof(Pair), (n - i - 1) * sizeof(Pair));
            moves++;
            found = 1;
        }
        pair++;
        i++;
    }
    new_pairs = realloc(bucket->pairs, (bucket->count - moves) * sizeof(Pair));
    if (new_pairs != NULL) {
        bucket->pairs = new_pairs;
    }
    return found;
}

/*
 * Returns a hash code for the provided string.
 */
static unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

