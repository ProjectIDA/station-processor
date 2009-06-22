#include <fmash.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

/*
 * OFA record internal format
 * 
 * Version/Type : uint16_t ---- (2 bytes)
 * Start Time   : time_t ------ (4 bytes)
 * End Time     : time_t ------ (4 bytes)
 * Channel ID   : uint16_t ---- (2 bytes)
 * Num Rows     : uint16_t ---- (2 bytes)
 * Data Bitmap  : uint8_t[24] - (24 bytes)
 * Description  : pascal str. - (3-10 bytes)
 * First Avg    : varint ------ (1 - 5 bytes)
 * First Hi     : varint ------ (1 - 5 bytes)
 * First Low    : varint ------ (1 - 5 bytes)
 * Data Bytes   : varint(s) --- (0+ bytes)
 * Last Avg     : varint ------ (1 - 5 bytes)
 * Last Hi      : varint ------ (1 - 5 bytes)
 * Last Low     : varint ------ (1 - 5 bytes)
 * 
 * Minimum size : 45 bytes
 * Maximum size : 976 bytes
 */

#define NO_CHANGE   0x00  /* No deltas  */
#define CHANGES_1   0x01  /* 1 delta    */
#define CHANGES_2   0x02  /* 2 deltas   */
#define CHANGES_3   0x03  /* 3 deltas   */
#define NO_DATA     0x07  /* No data    */

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

    uint8_t  buf_byte  = 0;
    uint16_t buf_word  = 0;
    uint32_t buf_dword = 0L;
    //uint64_t buf_qword = 0LL;

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

    uint16_t version_type = FALCON_VERSION;
    uint16_t row_count = 0;
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

    // Add the version/type
    buf_word = htons(version_type);
    buffer_write(buf, (uint8_t*)(&buf_word), sizeof(buf_word)); // Version + Type

    // Reserve space for elements that will be written during/after
    // the compression loop
    buffer_write(buf, (uint8_t*)(&buf_dword), sizeof(buf_dword)); // Start Time
    buffer_write(buf, (uint8_t*)(&buf_dword), sizeof(buf_dword)); // End Time
    buffer_write(buf, (uint8_t*)(&buf_word), sizeof(buf_word)); // Channel ID
    buffer_write(buf, (uint8_t*)(&buf_word), sizeof(buf_word)); // Row Count
    buffer_write(buf, rowmap_buffer, sizeof(rowmap_buffer)); // Row Map

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

                printf("first average : %d\n", first_average);
                printf("first high    : %d\n", first_high);
                printf("first low     : %d\n", first_low);
            }
        }

        // If the timestamps match, add the current row
        if (current_time == buffered_row->timestamp)
        {
            updated_rows  = NO_CHANGE;

            if (buffered_row->empty)
            {
                updated_rows = NO_DATA;
            }
            else
            {
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
            }
            
            // Update the bitmap
            map_3_set(buf->content + 12, i, updated_rows);
            buffered_row = csv_row_destroy(buffered_row);
        }
        // If the timestamps do NOT match, note in the bitmap that 
        // we are missing a row at this minute
        else
        {
            // Update the bitmap
            map_3_set(buf->content + 12, i, (uint8_t)NO_DATA);
        }
        current_time += TM_MINUTE;
        row_count++;
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
    printf("last average : %d\n", last_average);
    printf("last high    : %d\n", last_high);
    printf("last low     : %d\n", last_low);

    buffer_seek(buf, 2);
    buf_dword = htonl(start_time);
    buffer_write(buf, (uint8_t*)(&buf_dword), sizeof(buf_dword));
    buf_dword = htonl(end_time);
    buffer_write(buf, (uint8_t*)(&buf_dword), sizeof(buf_dword));
    buf_word = htons(csv->header->channel);
    buffer_write(buf, (uint8_t*)(&buf_word), sizeof(buf_word));
    buf_word = htons(row_count);
    buffer_write(buf, (uint8_t*)(&buf_word), sizeof(buf_word));

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

    uint8_t map = 0;
    uint8_t* p = NULL;
    uint8_t* rowmap = NULL;

    csv_row_t* row = NULL;
    size_t description_length = 0;
    csv_buffer_t* p_csv = NULL;
    uint16_t row_count = 0;
    uint16_t version_type = 0;

    p = raw_msh;
    if (csv && raw_msh && length)
    {
        // Allocate csv_buffer and child structures
        p_csv = csv_buffer_init();
        if (!p_csv)
            goto unclean;

        // Allocated space for the description
        p_csv->header->description = (char*)calloc(MAX_DESCRIPTION + 1, sizeof(char));
        if (!p_csv->header->description)
            goto unclean;

        // Extract the header information
        version_type = ntohs(*((uint16_t*)p)); // Version + Type
        p += sizeof(uint16_t);
        if ((version_type & 0x7fff) > FALCON_VERSION) {
            fprintf(stderr, "Falcon data version is too new.\n");
            exit(1);
        }
        start_time = (time_t)ntohl(*((uint32_t*)p)); // Start Time
        p += sizeof(uint32_t);
        end_time = (time_t)ntohl(*((uint32_t*)p)); // End Time
        p += sizeof(uint32_t);
        p_csv->header->channel = ntohs(*((uint16_t*)p)); // Channel ID
        p += sizeof(uint16_t);
        row_count = ntohs(*((uint16_t*)p)); // Row Count
        p += sizeof(uint16_t);
        rowmap = p; // Row Map
        p += ROW_MAP_SIZE;

        // Description
        description_length = *p;
        p += sizeof(uint8_t);
        strncpy(p_csv->header->description, (char*)p, description_length);
        p += description_length + 1;

        // Initial Values
        first_average = last_average = varint_to_int32(p, &vint_bytes);
        p += vint_bytes;
        first_high = last_high = varint_to_int32(p, &vint_bytes);
        p += vint_bytes;
        first_low = last_low = varint_to_int32(p, &vint_bytes);
        p += vint_bytes;
        printf("first average : %d\n", first_average);
        printf("first high    : %d\n", first_high);
        printf("first low     : %d\n", first_low);

        // Loop through the compressed buffer and re-construct 
        // the csv list
        current_time = start_time;
        for (i = 0; i < (int)row_count; i++)
        {
            map = map_3_get(rowmap, i);
            // If this is our first time through the loop or we have just
            // added a row to the list, then 'row' will be NULL, so
            // create a new row element
            if (!row)
            {
                row = csv_row_init();
                if (!row)
                    goto unclean;
            }
            // Handle the type of data we find in the map
            switch (map)
            {
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
                    for (j = 0; j < map; j++)
                    {
                        switch(*p & 0x3)
                        {
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

    printf("last average : %d\n", last_average);
    printf("last high    : %d\n", last_high);
    printf("last low     : %d\n", last_low);

    printf("final average : %d\n", final_average);
    printf("final high    : %d\n", final_high);
    printf("final low     : %d\n", final_low);

    // Verify the final values
    if (final_average != last_average)
    {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end averages (%d != %d)\n", final_average, last_average);
        else
            syslog(LOG_ERR, "falcon: mismatched start and end averages (%d != %d)\n", final_average, last_average);
        goto unclean;
    }
    if (final_high != last_high)
    {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end highs (%d != %d)\n", final_high, last_high);
        else
            syslog(LOG_ERR, "falcon: mismatched start and end highs (%d != %d)\n", final_high, last_high);
        goto unclean;
    }
    if (final_low != last_low)
    {
        if (gDebug)
            fprintf(stderr, "falcon: mismatched start and end lows (%d != %d)\n", final_low, last_low);
        else
            syslog(LOG_ERR, "falcon: mismatched start and end lows (%d != %d)\n", final_low, last_low);
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

