#include <falcon_structures.h>

/* ===== Alarm Line Handlers ================================ */
alarm_line_t* alarm_line_init()
{
    alarm_line_t* alarm_line = NULL;
    alarm_line = (alarm_line_t*)calloc(1, sizeof(alarm_line_t));
    return alarm_line;
}

alarm_line_t* alarm_line_duplicate( alarm_line_t* alarm_line )
{
    alarm_line_t* new_line = NULL;
    if (alarm_line) {
        new_line = (alarm_line_t*)malloc(sizeof(alarm_line_t));
        if (new_line) {
            memcpy(new_line, alarm_line, sizeof(alarm_line_t));
        }
    }
    return new_line;
}

alarm_line_t* alarm_line_destroy( alarm_line_t* alarm_line )
{
    if (alarm_line) {
        if (alarm_line->text) {
            free(alarm_line->text);
        }
        free(alarm_line);
        alarm_line = NULL;
    }
    return alarm_line;
}

/* ===== Alarm Context Handlers =========================== */
alarm_context_t* alarm_context_init()
{
    alarm_context_t* alarm_list = NULL;
    alarm_list = (alarm_context_t*)calloc(1, sizeof(alarm_context_t));
    if (alarm_list) {
        if (list_init(alarm_list) == -1) {
            free(alarm_list);
            alarm_list = NULL;
        } else {
            list_attributes_comparator(alarm_list, _alarm_list_comparator);
            list_attributes_seeker(alarm_list, _alarm_list_seeker);
        }
    }
    return alarm_list;   
}

alarm_context_t* alarm_context_destroy( alarm_context_t* alarm_list )
{
    alarm_line_t* line = NULL;
    if (alarm_list) {
        while (!list_empty(alarm_list)) {
            line = (alarm_line_t*)list_fetch(alarm_list);
            alarm_line_destroy(line);
        }
        list_destroy(alarm_list);
        free(alarm_list);
        alarm_list = NULL;
    }
    return alarm_list;
}

int _alarm_list_comparator( const void* a, const void* b )
{
    if (a && b) {
        if (((alarm_line_t*)a)->timestamp != ((alarm_line_t*)b)->timestamp) {
            return (((alarm_line_t*)b)->timestamp - ((alarm_line_t*)a)->timestamp);
        } else if (((alarm_line_t*)a)->text && ((alarm_line_t*)b)->text) {
            return strcmp(((alarm_line_t*)b)->text, ((alarm_line_t*)a)->text);
        }
    } else if (!a && b) {
        return 1;
    } else if (a && !b) {
        return -1;
    }
    return 0;
}

int _alarm_list_seeker( const void* element, const void* indicator )
{
    if (!element || !indicator)
        return 0;
    if (((alarm_line_t*)element)->hash != ((alarm_line_t*)indicator)->hash)
        return 0;
    if (((alarm_line_t*)element)->channel != ((alarm_line_t*)indicator)->channel)
        return 0;
    if (((alarm_line_t*)element)->event != ((alarm_line_t*)indicator)->event)
        return 0;
    if (((alarm_line_t*)element)->timestamp != ((alarm_line_t*)indicator)->timestamp)
        return 0;
    if (!((alarm_line_t*)indicator)->text || !((alarm_line_t*)element)->text)
        return 0;
    if (strcmp(((alarm_line_t*)indicator)->text, ((alarm_line_t*)element)->text))
        return 0;
    return 1;
}



/* ===== CSV Context Handlers =========================== */
csv_context_t* csv_context_init()
{
    csv_context_t* csv_buffer_list = NULL;
    csv_buffer_list = (csv_context_t*)malloc(sizeof(csv_context_t));
    if (csv_buffer_list) {
        if (list_init(csv_buffer_list) == -1) {
            free(csv_buffer_list);
            csv_buffer_list = NULL;
        } else {
            list_attributes_seeker( csv_buffer_list,  _buffer_list_seeker );
            list_attributes_comparator( csv_buffer_list,  _buffer_list_comparator );
        }
    }
    return csv_buffer_list;   
}

csv_context_t* csv_context_destroy( csv_context_t* csv_buffer_list )
{
    csv_buffer_t* csv_buffer = NULL;
    if (csv_buffer_list) {
        while (!list_empty(csv_buffer_list)) {
            csv_buffer = list_fetch(csv_buffer_list);
            csv_buffer = csv_buffer_destroy(csv_buffer);
        }
        list_destroy(csv_buffer_list);
        free(csv_buffer_list);
        csv_buffer_list = NULL;
    }
    return csv_buffer_list;
}

int _buffer_list_seeker( const void* element, const void* indicator )
{
    if ( element && indicator &&
         !strcmp(((csv_buffer_t*)element)->file_name,
                 ((csv_buffer_t*)indicator)->file_name) )
        return 1;
    return 0;
}

int _buffer_list_comparator( const void* a, const void* b )
{
    if ( !a && !b )
        return 0;
    else if ( a && !b ) 
        return -1;
    else if ( !a && b )
        return 1;
    return strcmp( ((csv_buffer_t*)b)->file_name, ((csv_buffer_t*)a)->file_name );
}

/* ===== CSV Buffer Handlers ================================= */
csv_buffer_t* csv_buffer_init()
{
    csv_buffer_t* csv_buffer = NULL;
    csv_buffer = (csv_buffer_t*)calloc(1, sizeof(csv_buffer_t));
    if (csv_buffer) {
        csv_buffer->header = csv_header_init();
        csv_buffer->list = csv_list_init();
    }
    return csv_buffer;
}

csv_buffer_t* csv_buffer_destroy(csv_buffer_t* csv_buffer)
{
    if (csv_buffer) {
        if (csv_buffer->file_name) {
            free(csv_buffer->file_name);
            csv_buffer->file_name = NULL;
        }
        csv_buffer->header = csv_header_destroy(csv_buffer->header);
        csv_buffer->list = csv_list_destroy(csv_buffer->list);
        free(csv_buffer);
        csv_buffer = NULL;
    }
    return csv_buffer;
}

/* ===== CSV Header Handlers ================================= */
csv_header_t* csv_header_init()
{
    csv_header_t* csv_header = NULL;
    csv_header = (csv_header_t*)calloc(1, sizeof(csv_header_t));
    return csv_header;
}

csv_header_t* csv_header_duplicate(csv_header_t* csv_header)
{
    csv_header_t* new_header = NULL;
    if (csv_header) {
        new_header = (csv_header_t*)malloc(sizeof(csv_header_t));
        if (new_header) {
            memcpy(new_header, csv_header, sizeof(csv_header_t));
        }
    }
    return new_header;
}

csv_header_t* csv_header_destroy(csv_header_t* csv_header)
{
    if (csv_header) {
        if (csv_header->name) {
            free(csv_header->name);
            csv_header->name = NULL;
        }
        if (csv_header->description) {
            free(csv_header->description);
            csv_header->description = NULL;
        }
        if (csv_header->unit_of_measure) {
            free(csv_header->unit_of_measure);
            csv_header->unit_of_measure = NULL;
        }
        free(csv_header);
        csv_header = NULL;
    }
    return csv_header;
}

/* ===== CSV List Handlers ================================= */
csv_list_t* csv_list_init()
{
    csv_list_t* csv_list = NULL;
    csv_list = (csv_list_t*)calloc(1, sizeof(csv_list_t));
    if (csv_list && (list_init(csv_list) == -1)) {
        free(csv_list);
        csv_list = NULL;
    }
    return csv_list;
}

csv_list_t* csv_list_destroy(csv_list_t* csv_list)
{
    csv_row_t* csv_row = NULL;
    if (csv_list) {
        while(!list_empty(csv_list)) {
            csv_row = list_fetch(csv_list);
            csv_row = csv_row_destroy(csv_row);
        }
        list_destroy(csv_list);
        free(csv_list);
        csv_list = NULL;
    }
    return csv_list;
}

/* ===== CSV Row Handlers ================================= */
csv_row_t* csv_row_init()
{
    csv_row_t* csv_row = NULL;
    csv_row = (csv_row_t*)calloc(1, sizeof(csv_row_t));
    return csv_row;
}

csv_row_t* csv_row_duplicate(csv_row_t* csv_row)
{
    csv_row_t* new_row = NULL;
    
    if (csv_row) {
        new_row = (csv_row_t*)malloc(sizeof(csv_row_t));
        if (new_row) {
            memcpy(new_row, csv_row, sizeof(csv_row_t));
        }
    }
    return new_row;
}

csv_row_t* csv_row_destroy(csv_row_t* csv_row)
{
    if (csv_row) {
        free(csv_row);
        csv_row = NULL;
    }
    return csv_row;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Convert a signed 32-bit integer into a varint
 */
varint int32_to_varint( int32_t integer )
{
    uint32_t* p_integer = NULL;
    varint result;

    memset(&result, 0, sizeof(varint));
    p_integer = (uint32_t*)&integer;

    if ((integer < 16) && (integer > -17)) {
        result.num[0] = (*p_integer & 0x0000001f) << 2;
        result.bytes = 1;
    }
    else if ((integer < 2048) && (integer > -2049)) {
        result.num[0] = ((*p_integer & 0x0000001f) << 2) | 0x80;
        result.num[1] = ((*p_integer >> 5) & 0x0000007f);
        result.bytes = 2;
    }
    else if ((integer < 262144) && (integer > -262145)) {
        result.num[0] = ((*p_integer & 0x0000001f) << 2)  | 0x80;
        result.num[1] = ((*p_integer >>  5) & 0x0000007f) | 0x80;
        result.num[2] = ((*p_integer >> 12) & 0x0000007f);
        result.bytes = 3;
    }
    else if ((integer < 33554432) && (integer > -33554432)) {
        result.num[0] = ((*p_integer & 0x0000001f) << 2)  | 0x80;
        result.num[1] = ((*p_integer >>  5) & 0x0000007f) | 0x80;
        result.num[2] = ((*p_integer >> 12) & 0x0000007f) | 0x80;
        result.num[3] = ((*p_integer >> 19) & 0x0000007f);
        result.bytes = 4;
    }
    else {
        result.num[0] = ((*p_integer & 0x0000001f) << 2)  | 0x80;
        result.num[1] = ((*p_integer >>  5) & 0x0000007f) | 0x80;
        result.num[2] = ((*p_integer >> 12) & 0x0000007f) | 0x80;
        result.num[3] = ((*p_integer >> 19) & 0x0000007f) | 0x80;
        result.num[4] = ((*p_integer >> 26) & 0x0000003f);
        result.bytes = 5;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Re-construct a signed 32-bit integer from a varint
 */
int32_t varint_to_int32( uint8_t* integer, size_t* bytes )
{
    int32_t result = 0;
    uint32_t* p_result = NULL;
    uint8_t* p_num = NULL;

    p_num = integer;
    p_result = (uint32_t*)(&result);
    *bytes = 0;
    
    *p_result |= (*p_num >> 2) & 0x0000001f;
    *bytes += 1;
    if (!(*p_num & 0x80)) {
        if (*p_num & 0x40) {
            *p_result |= 0xffffffe0;
        }
        goto complete;
    } p_num++;

    *p_result |= (*p_num & 0x0000007f) << 5;
    *bytes += 1;
    if (!(*p_num & 0x80)) {
        if (*p_num & 0x40) {
            *p_result |= 0xfffff000;
        }
        goto complete;
    } p_num++;

    *p_result |= (*p_num & 0x0000007f) << 12;
    *bytes += 1;
    if (!(*p_num & 0x80)) {
        if (*p_num & 0x40) {
            *p_result |= 0xfff80000;
        }
        goto complete;
    } p_num++;

    *p_result |= (*p_num & 0x0000007f) << 19;
    *bytes += 1;
    if (!(*p_num & 0x80)) {
        if (*p_num & 0x40) {
            *p_result |= 0xfc000000;
        }
        goto complete;
    } p_num++;

    *p_result |= (*p_num & 0x3f) << 26;
    *bytes += 1;

 complete:
    return result;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  The map_4_set() and map_4_get() functions store/recall 4-bit values
 *  to/from a buffer. The index determines where in the buffer the
 *  4-bit value will be stored. The index is on 4-bit boundaries
 *  within the buffer: 
 *    index 0 points to bits 0-3, index 1 points to bits 4-7, etc.
 * 
 *  map_4_set() - stores the 4 least significant bits of 'value'
 *              into 'map' at 'index'.
 *  map_4_get() - returns the 4-bit value stored in 'map' at 'index'.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void map_4_set( uint8_t* map, int index, uint8_t value )
{
    map += index / 2;
    if (index % 2)
        *(map) = (*(map) & 0x0f) | ((value & 0x0f) << 4);
    else
        *(map) = (*(map) & 0xf0) | (value & 0x0f);
}

uint8_t map_4_get( const uint8_t* map, int index )
{
    map += index / 2;
    if (index % 2)
        return ((*(map) >> 4) & 0x08);
    else
        return (*(map) & 0x08);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  The map_3_set() and map_3_get() functions store/recall 3-bit values
 *  to/from a buffer. The index determines where in the buffer the
 *  3-bit value will be stored. The index is on 3-bit boundaries
 *  within the buffer: 
 *    index 0 points to bits 0-2, index 1 points to bits 3-5, etc.
 * 
 *  map_3_set() - stores the 3 least significant bits of 'value'
 *              into 'map' at 'index'.
 *  map_3_get() - returns the 3-bit value stored in 'map' at 'index'.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void map_3_set( uint8_t* map, int index, uint8_t value )
{
    int pos = 0;
    pos = (index / 8) * 3;
    switch (index % 8) {
        case 0 :
            *(map+pos)   = (*(map+pos)   & 0xf8) | ((value & 0x07)     ); break;
        case 1 :
            *(map+pos)   = (*(map+pos)   & 0xc7) | ((value & 0x07) << 3); break;
        case 2 :
            *(map+pos)   = (*(map+pos)   & 0x3f) | ((value & 0x03) << 6);
            *(map+pos+1) = (*(map+pos+1) & 0xfe) | ((value & 0x04) >> 2); break;
        case 3 :
            *(map+pos+1) = (*(map+pos+1) & 0xf1) | ((value & 0x07) << 1); break;
        case 4 :
            *(map+pos+1) = (*(map+pos+1) & 0x8f) | ((value & 0x07) << 4); break;
        case 5 :
            *(map+pos+1) = (*(map+pos+1) & 0x7f) | ((value & 0x01) << 7);
            *(map+pos+2) = (*(map+pos+2) & 0xfc) | ((value & 0x06) >> 1); break;
        case 6 :
            *(map+pos+2) = (*(map+pos+2) & 0xe3) | ((value & 0x07) << 2); break;
        case 7 :
            *(map+pos+2) = (*(map+pos+2) & 0x1f) | ((value & 0x07) << 5); break;
    }
}

uint8_t map_3_get( const uint8_t* map, int index )
{
    int pos = 0;
    pos = (index / 8) * 3;
    switch (index % 8) {
        case 0 :
            return ((*(map+pos)        ) & 0x07); break;
        case 1 :
            return ((*(map+pos)    >> 3) & 0x07); break;
        case 2 :
            return ((*(map+pos)   >> 6) & 0x03) |
                   ((*(map+pos+1) << 2) & 0x04); break;
        case 3 :
            return ((*(map+pos+1) >> 1) & 0x07); break;
        case 4 :
            return ((*(map+pos+1) >> 4) & 0x07); break;
        case 5 :
            return ((*(map+pos+1) >> 7) & 0x01) |
                   ((*(map+pos+2) << 1) & 0x06); break;
        case 6 :
            return ((*(map+pos+2) >> 2) & 0x07); break;
        case 7 :
            return ((*(map+pos+2) >> 5) & 0x07); break;
        default:
            return 0;
    };
}


