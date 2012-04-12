/*
 *    map
 *
 *    Generic hash table with string based keys.
 *
 *    map.h
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

#ifndef _MAP_H_
#define _MAP_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>

typedef struct Map Map;
typedef void(*map_enum_func)(const char *key, const void *value, const void *obj);
typedef void(*map_value_free_func)(const void *value);
typedef void*(*map_value_dup_func)(const void *value);

Map    *map_new(unsigned int capacity, map_value_free_func free_func, map_value_dup_func dup_func);
void    map_delete(Map *map);

int     map_put(Map *map, const char *key, void *value);
void   *map_get(const Map *map, const char *key);
int     map_contains(const Map *map, const char *key);
int     map_remove(Map *map, const char *key);
int     map_get_count(const Map *map);
int     map_enum(const Map *map, map_enum_func enum_func, const void *obj);

#ifdef __cplusplus
}
#endif
#endif

