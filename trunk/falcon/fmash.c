#include <fmash.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

/*
 * OFA record internal format
 * 
 * Channel ID  : uint16_t ---- (2 bytes)
 * Num Rows    : uint16_t ---- (2 bytes)
 * Start Time  : time_t ------ (4 bytes)
 * End Time    : time_t ------ (4 bytes)
 * Data Bitmap : uint8_t[24] - (24 bytes)
 * Description : pascal str. - (3-10 bytes)
 * First Avg   : varint ------ (1 - 5 bytes)
 * First Hi    : varint ------ (1 - 5 bytes)
 * First Low   : varint ------ (1 - 5 bytes)
 * Data Bytes  : varint(s) --- (0+ bytes)
 * Last Avg    : varint ------ (1 - 5 bytes)
 * Last Hi     : varint ------ (1 - 5 bytes)
 * Last Low    : varint ------ (1 - 5 bytes)
 * 
 * Minimum size: 45 bytes
 * Maximum size: 976 bytes
 */

#define NO_CHANGE  0
#define CHANGES_1  1
#define CHANGES_2  2
#define CHANGES_3  3
#define NO_DATA    7

#define TYPE_AVERAGE  1
#define TYPE_HIGH     2
#define TYPE_LOW      3

#define MAX_DESCRIPTION 8
#define ROW_MAP_SIZE 24

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Compress the contents of a csv buffer into FMash format
 */
int fmash_csv_to_msh( csv_buffer_t* csv, uint8_t** raw_msh, size_t* length )
{
    int result = 0;
    int i = 0;
    int first_time = 1;

    time_t start_time;
    time_t end_time;
    time_t current_time;

    int32_t first_average = 0;
    int32_t first_high = 0;
    int32_t first_low = 0;
    
    int32_t last_average = 0;
    int32_t last_high = 0;
    int32_t last_low = 0;

    int32_t delta_average = 0;
    int32_t delta_high = 0;
    int32_t delta_low = 0;

    varint vint;

    uint16_t element_count = 0;
    uint8_t updated_rows = 0;
    uint8_t description_length = 0;

    uint8_t rowmap_buffer[ROW_MAP_SIZE];
    buffer_t* buf = NULL;
    csv_row_t* buffered_row = NULL;

    // Confirm that all required data structures are populated
    if ((!csv) || (!csv->list) || (!csv->header) || (!raw_msh) || (!length))
    {
        goto unclean;
    }

    memset(rowmap_buffer, 0, sizeof(rowmap_buffer));

    // Initialize the buffer that will store the compressed data
    buf = buffer_init();

    // Reserve space for elements that will be written during/after
    // the compression loop
    buffer_write(buf, rowmap_buffer, sizeof(csv->header->channel));
    buffer_write(buf, rowmap_buffer, sizeof(element_count));
    buffer_write(buf, rowmap_buffer, sizeof(start_time));
    buffer_write(buf, rowmap_buffer, sizeof(end_time));
    buffer_write(buf, rowmap_buffer, sizeof(rowmap_buffer));

    // Add the description string in Pascal string format
    description_length = strlen(csv->header->description);
    description_length = description_length < MAX_DESCRIPTION ?
                         description_length : MAX_DESCRIPTION;
    buffer_write(buf, &description_length, sizeof(description_length));
    buffer_write(buf, (uint8_t*)csv->header->description, description_length);
    buffer_terminate(buf);

    // Generate the compressed buffer
    for (i = 0; i < MAX_ROWS; i++)
    {
        // We don't have a row buffered
        if (!buffered_row)
        {
            // If the list is empty we are done
            if (list_empty(csv->list))
            {
                break;
            }

            // Fetch item from the head of the list
            buffered_row = list_fetch(csv->list);

            // Is this our first record
            if (first_time)
            {
                start_time = buffered_row->timestamp;
                last_average = first_average = buffered_row->average;
                last_high = first_high = buffered_row->high;
                last_low = first_low = buffered_row->low;
                current_time = buffered_row->timestamp;

                // Insert the channel
                buf->content[0] = (csv->header->channel     ) & 0xff;
                buf->content[1] = (csv->header->channel >> 8) & 0xff;

                // Insert the start time
                buf->content[4] = (start_time      ) & 0xff;
                buf->content[5] = (start_time >>  8) & 0xff;
                buf->content[6] = (start_time >> 16) & 0xff;
                buf->content[7] = (start_time >> 24) & 0xff;

                // Insert the first average
                vint = int32_to_varint(first_average);
                vint.num[0] = (vint.num[0] & 0xfc) | TYPE_AVERAGE;
                buffer_write(buf, vint.num, vint.bytes);

                // Insert the first high
                vint = int32_to_varint(first_high);
                vint.num[0] = (vint.num[0] & 0xfc) | TYPE_HIGH;
                buffer_write(buf, vint.num, vint.bytes);

                // Insert the first low
                vint = int32_to_varint(first_low);
                vint.num[0] = (vint.num[0] & 0xfc) | TYPE_LOW;
                buffer_write(buf, vint.num, vint.bytes);

                first_time = 0;
            }
        }

        // If the timestamps match, add the current row
        if (current_time == buffered_row->timestamp)
        {
            updated_rows  = NO_CHANGE;

            // Calculate our delta values
            delta_average = buffered_row->average - last_average;
            delta_high    = buffered_row->high    - last_high;
            delta_low     = buffered_row->low     - last_low;

            // Only write the delta values to the compression buffer
            // if they are different from the previous deltas
            if (delta_average)
            {
                vint = int32_to_varint(delta_average);
                vint.num[0] = (vint.num[0] & 0xfc) | TYPE_AVERAGE;
                buffer_write(buf, vint.num, vint.bytes);
                updated_rows++;
                last_average = buffered_row->average;
            }
            if (delta_high)
            {
                vint = int32_to_varint(delta_high);
                vint.num[0] = (vint.num[0] & 0xfc) | TYPE_HIGH;
                buffer_write(buf, vint.num, vint.bytes);
                updated_rows++;
                last_high = buffered_row->high;
            }
            if (delta_low)
            {
                vint = int32_to_varint(delta_low);
                vint.num[0] = (vint.num[0] & 0xfc) | TYPE_LOW;
                buffer_write(buf, vint.num, vint.bytes);
                updated_rows++;
                last_low = buffered_row->low;
            }
            
            // Update the bitmap with the number of deltas written
            map_set(buf->content + 12, i, updated_rows);
            buffered_row = csv_row_destroy(buffered_row);
        }
        // If the timestamps do NOT match, note in the bitmap that 
        // we are missing a row at this minute
        else
        {
            map_set(buf->content + 12, i, NO_DATA);
        }
        current_time += TM_MINUTE;
        element_count++;
    }
    end_time = current_time - TM_MINUTE;

    // If we didn't use this element, restore it to the list
    if (buffered_row)
    {
        list_prepend(csv->list, buffered_row);
    }

    // Update the starting timestamp for the csv buffer
    buffered_row = list_get_at(csv->list, (unsigned int)0);
    if (buffered_row)
    {
        csv->start_time = buffered_row->timestamp;
    }
    else
    {
        csv->start_time = end_time;
    }
    buffered_row = NULL;

    // Store the element count
    buf->content[2] = (element_count     ) & 0xff;
    buf->content[3] = (element_count >> 8) & 0xff;

    // Store the end time
    buf->content[8]  = (end_time      ) & 0xff;
    buf->content[9]  = (end_time >>  8) & 0xff;
    buf->content[10] = (end_time >> 16) & 0xff;
    buf->content[11] = (end_time >> 24) & 0xff;

    // Add the final average
    vint = int32_to_varint(last_average);
    vint.num[0] = (vint.num[0] & 0xfc) | TYPE_AVERAGE;
    buffer_write(buf, vint.num, vint.bytes);

    // Add the final high
    vint = int32_to_varint(last_high);
    vint.num[0] = (vint.num[0] & 0xfc) | TYPE_HIGH;
    buffer_write(buf, vint.num, vint.bytes);

    // Add the final low
    vint = int32_to_varint(last_low);
    vint.num[0] = (vint.num[0] & 0xfc) | TYPE_LOW;
    buffer_write(buf, vint.num, vint.bytes);

    // Populated the elements whose pointers the user supplied
    *length  = buffer_size(buf);
    *raw_msh = buffer_detach(buf);

    result = 1;
    goto clean;

 unclean:
 clean:
    buf = buffer_destroy(buf);
    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Decompress FMash formatted data into a csv buffer
 */
int fmash_msh_to_csv( csv_buffer_t** csv, uint8_t* raw_msh, size_t length )
{
    int result = 0;
    int i = 0;
    int j = 0;

    time_t start_time;
    time_t end_time;
    time_t current_time;

    int32_t first_average = 0;
    int32_t first_high = 0;
    int32_t first_low = 0;
    
    int32_t last_average = 0;
    int32_t last_high = 0;
    int32_t last_low = 0;

    uint8_t final_average = 0;
    uint8_t final_high = 0;
    uint8_t final_low = 0;

    size_t vint_bytes = 0;

    uint8_t map;
    uint8_t* p = NULL;
    uint8_t* rowmap = NULL;

    csv_row_t* row = NULL;
    size_t description_length = 0;
    csv_buffer_t* p_csv = NULL;
    uint16_t element_count = 0;

    p = raw_msh;
    if (csv && raw_msh && length) {
        // Allocate csv_buffer and child structures
        p_csv = csv_buffer_init();
        if (!p_csv)
            goto unclean;

        // Allocated space for the description
        p_csv->header->description = (char*)calloc(MAX_DESCRIPTION + 1, sizeof(char));
        if (!p_csv->header->description)
            goto unclean;

        // Extract the header information
        p_csv->header->channel = *p | *(p+1) << 8;
        p += sizeof(uint16_t);
        element_count = *p | *(p+1) << 8;
        p += sizeof(uint16_t);
        start_time = *p | *(p+1) << 8 | *(p+2) << 16 | *(p+3) << 24;
        p += sizeof(time_t);
        end_time = *p | *(p+1) << 8 | *(p+2) << 16 | *(p+3) << 24;
        p += sizeof(time_t);
        rowmap = p;
        p += ROW_MAP_SIZE;
        description_length = *p;
        p += sizeof(uint8_t);
        strncpy(p_csv->header->description, (char*)p, description_length);
        p += description_length + 1;
        first_average = last_average = varint_to_int32(p, &vint_bytes);
        p += vint_bytes;
        first_high = last_high = varint_to_int32(p, &vint_bytes);
        p += vint_bytes;
        first_low = last_low = varint_to_int32(p, &vint_bytes);
        p += vint_bytes;

        // Loop through the compressed buffer and re-construct 
        // the csv list
        current_time = start_time;
        for (i = 0; i < (int)element_count; i++) {
            map = map_get(rowmap, i);
            // If this is our first time through the loop or we have just
            // added a row to the list, then 'row' will be NULL, so
            // create a new row element
            if (!row) {
                row = csv_row_init();
                if (!row) {
                    goto unclean;
                }
            }
            // Handle the type of data we find in the map
            switch (map) {
                // If there are no deltas, the current row is the same
                // as the previous row
                case NO_CHANGE : 
                    row->timestamp = current_time;
                    row->average = last_average;
                    row->high = last_high;
                    row->low = last_low;
                    list_append(p_csv->list, row);
                    row = NULL;
                    break;
                // If there are deltas, extract the correct number of deltas
                // and generate a new row
                case CHANGES_1 :
                case CHANGES_2 :
                case CHANGES_3 :
                    row->timestamp = current_time;
                    row->average = last_average;
                    row->high = last_high;
                    row->low = last_low;
                    for (j = 0; j < map; j++) {
                        switch(*p & 0x3) {
                            case TYPE_AVERAGE :
                                last_average += varint_to_int32(p, &vint_bytes);
                                row->average = last_average;
                                p += vint_bytes; break;
                            case TYPE_HIGH :
                                last_high += varint_to_int32(p, &vint_bytes);
                                row->high = last_high;
                                p += vint_bytes; break;
                            case TYPE_LOW :
                                last_low += varint_to_int32(p, &vint_bytes);
                                row->low = last_low;
                                p += vint_bytes; break;
                        }
                    }
                    list_append(p_csv->list, row);
                    row = NULL;
                    break;
            }
            current_time += TM_MINUTE;
        }
        row = csv_row_destroy(row);
    }

    // Recover the final values
    final_average = varint_to_int32(p, &vint_bytes);
    p += vint_bytes;
    final_high = varint_to_int32(p, &vint_bytes);
    p += vint_bytes;
    final_low = varint_to_int32(p, &vint_bytes);
    p += vint_bytes;

    // Verify the final values
    if (final_average != last_average) {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end averages\n");
        else
            syslog(LOG_ERR, "falcon: mismatched start and end averages\n");
        goto unclean;
    }
    if (final_high != last_high) {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end highs\n");
        else
            syslog(LOG_ERR, "falcon: mismatched start and end highs\n");
        goto unclean;
    }
    if (final_low != last_low) {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end lows\n");
        else
            syslog(LOG_ERR, "falcon: mismatched start and end lows\n");
        goto unclean;
    }

    (p_csv)->start_time = start_time;
    (p_csv)->end_time = end_time;

    result = 1;
    goto clean;

 unclean:
    p_csv = csv_buffer_destroy(p_csv);
 clean:
    *csv = p_csv;
    return result;
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
 *  The map_set() and map_get() functions store/recall 3-bit values
 *  to/from a buffer. The index determines where in the buffer the
 *  3-bit value will be stored. The index is on 3-bit boundaries
 *  within the buffer: 
 *    index 0 points to bits 0-2, index 1 points to bits 3-5, etc.
 * 
 *  map_set() - stores the 3 least significant bits of 'value'
 *              into 'map' at 'index'.
 *  map_get() - returns the 3-bit value stored in 'map' at 'index'.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void map_set( uint8_t* map, int index, uint8_t value )
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

uint8_t map_get( const uint8_t* map, int index )
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

