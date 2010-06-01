import java.io
import java.util.ArrayList

NO_CHANGE = 0x00 // No deltas
CHANGES_1 = 0x01 // 1 delta
CHANGES_2 = 0x02 // 2 deltas
CHANGES_3 = 0x03 // 3 deltas
NO_DATA   = 0x07 // No data

TYPE_AVERAGE = 1
TYPE_HIGH    = 2
TYPE_LOW     = 3

MAX_DESCRIPTION =  8
ROW_MAP_SIZE = 24

FALCON_VERSION = 1
TM_MINUTE = 60

// Version/Type : uint16_t ---- (2 bytes)        : 0
// Start Time   : time_t ------ (4 bytes)        : 2
// End Time     : time_t ------ (4 bytes)        : 6
// Channel ID   : uint16_t ---- (2 bytes)        : 10
// Num Rows     : uint16_t ---- (2 bytes)        : 12
// Data Bitmap  : uint8_t[24] - (24 bytes)       : 14
// Description  : pascal str. - (3-10 bytes)     : 38
// First Avg    : varint ------ (1 - 5 bytes)
// First Hi     : varint ------ (1 - 5 bytes)
// First Low    : varint ------ (1 - 5 bytes)
// Data Bytes   : varint(s) --- (0+ bytes)
// Last Avg     : varint ------ (1 - 5 bytes)
// Last Hi      : varint ------ (1 - 5 bytes)
// Last Low     : varint ------ (1 - 5 bytes)

public class EFmashVersion extends Exception {;}
public class EFmashDescLen extends Exception {;}
public class EFmashVarint  extends Exception {;}

public class FmashRow 
{
    public m_timestamp = 0;
    public m_average   = 0;
    public m_high      = 0;
    public m_low       = 0;

    FmashRwo(timestamp, average, high, low)
    {
        m_timestamp = timestamp;
        m_average   = average;
        m_hight     = high;
        m_low       = low;
    }

}

public class Varint
{
    public m_count = 0;
    public m_value = 0;

    Varint(value, count)
    {
        m_value = value;
        m_count = count;
    }
}

class Fmash:
    private ArrayList m_raw_msh       = null;
    private ArrayList m_data_bitmap   = null;
    private int       m_total_length  = 0;
    private int       m_offset        = 0;
    private int       m_desc_length   = 0;
    private int       m_first_average = 0;
    private int       m_first_high    = 0;
    private int       m_first_low     = 0;
    private int       m_last_average  = 0;
    private int       m_last_high     = 0;
    private int       m_last_low      = 0;

    public int        m_version       = 0;
    public long       m_start_time    = 0xffffffff;
    public long       m_end_time      = 0x7fffffff;
    public int        m_num_rows      = 0;
    public ArrayList  m_row_list      = null;
    public ArrayList  m_description   = null;

    Fmash(ArrayList raw_msh)
    {
        m_raw_msh      = raw_msh;
        m_total_length = m_raw_msh.size();
        m_rows         = new ArrayList();
        m_maps         = new ArrayList();

        _check_version();
        _populate_metadata();
        _populate_rows();
        _msh_to_arraylist();
    }

    private void _check_version()
    {
        m_version = int(m_raw_msh[0] | m_raw_msh[1] << 8)
        if ((m_version & 0x7fff) > FALCON_VERSION) {
            raise new EFmashVersion();
        }
    }

    private void _populate_metadata()
    {
        int desc_len;
        int desc_end;

        m_start_time  = int(m_raw_msh[2] | (m_raw_msh[3] <<  8) | (m_raw_msh[4] << 16) | (m_raw_msh[5] << 24) );
        m_end_time    = int(m_raw_msh[6] | (m_raw_msh[7] <<  8) | (m_raw_msh[8] << 16) | (m_raw_msh[9] << 24) );
        m_channel_id  = int(m_raw_msh[10] | (m_raw_msh[11] << 8));
        m_num_rows    = int(m_raw_msh[12] | (m_raw_msh[13] << 8));
        m_data_bitmap = m_raw_msh.subList(14, 38);
        desc_length   = int(m_raw_msh[38]);

        if ((desc_length < 1) || (desc_length > 8)) {
        m_count = count;
            raise new EFmashDescLen();
        }

        // desc_offset + length_byte + description + null_byte
        desc_end = 38 + 1 + desc_length + 1;
        m_description = m_raw_msh.subList(38, desc_end);
        
        m_offset = desc_end;
    }

    private void _populate_rows()
    {
        long current_time;
        byte item;

        m_rows = new ArrayList();
        m_first_average = _get_varint();
        m_first_high    = _get_varint();
        m_first_low     = _get_varint();
        m_last_average  = m_first_average;
        m_last_high     = m_first_high;
        m_last_low      = m_first_low;

        _get_maps();

        current_time = m_start_time;

        for (int i=0; i < m_maps.size(); i++) {
            map = m_maps[i];
            switch (map) {
                case CHANGES_1:
                case CHANGES_2:
                case CHANGES_3:
                    item = m_raw_msh[m_offset];
                    if ((item & 0x3) == TYPE_AVERAGE) {
                        m_last_average += _get_varint();
                    }
                    else if ((item & 0x3) == TYPE_HIGH) {
                        m_last_high += _get_varint();
                    }
                    else if ((item & 0x3) == TYPE_LOW) {
                        m_last_low += _get_varint();
                    }
                    break;
                m_rows.add(new FmashRow(current_time, m_last_average, m_last_high, m_last_low));
            }
        } // for loop
        m_last_average = _get_varint();
        m_last_high    = _get_varint();
        m_last_low     = _get_varint();
    }

    private void _get_varint()
    {
        int       varint_max = 5;
        int       value      = 0;
        int       count      = 0;
        Varint    vint       = new Varint(0,0);
        ArrayList bytes      = null;

        if ((m_total_length - m_offset) < 5) {
            varint_max = m_total_length - offset;
        }
        if (varint_max < 1) {
            raise new FMashVarint();
        }

        bytes = m_raw_msh.subList(m_offset, m_offset+varint_max);

        for (int i=0; i < bytes.size(); i++) {
            count++;
            value = bytes[i];

            switch (count) {
                case 1:  value |= int((value >> 2) & 0x0000001f); break;
                case 5:  value |= int((value & 0x3f) << 26); break;
                default: value |= int((value & 0x000007f) << (count * 7 - 2)); 
            }

            if ((0x80 & value) == 0) {
                if (value & 0x40) {
                    switch (count) {
                        case 1: value |= 0xffffffe0; break;
                        case 2: value |= 0xfffff000; break;
                        case 3: value |= 0xfff80000; break;
                        case 4: value |= 0xfc000000; break;
                    }
                }
            }
        } // for loop
        
        m_offset += count;
        return value;
    }

    private void _get_maps()
    {
        int pos = 0;
        int rotation = 0;
        for (int i=0; i < m_num_rows; i++) {
            pos = (index / 8) * 3;
            rotation = index % 8;
            switch (rotation) {
            case 0: m_maps.add(((m_data_bitmap[map+pos]       ) & 0x07)); break;
            case 1: m_maps.add(((m_data_bitmap[map+pos]   >> 3) & 0x07)); break;
            case 2: m_maps.add(((m_data_bitmap[map+pos]   >> 6) & 0x03)|
                               ((m_data_bitmap[map+pos+1] << 2) & 0x04)); break;
            case 3: m_maps.add(((m_data_bitmap[map+pos+1] >> 1) & 0x07)); break;
            case 4: m_maps.add(((m_data_bitmap[map+pos+1] >> 4) & 0x07)); break;
            case 5: m_maps.add(((m_data_bitmap[map+pos+1] >> 7) & 0x01)| 
                               ((m_data_bitmap[map+pos+2] << 1) & 0x06)); break;
            case 6: m_maps.add(((m_data_bitmap[map+pos+2] >> 2) & 0x07)); break;
            case 7: m_maps.add(((m_data_bitmap[map+pos+2] >> 5) & 0x07)); break;
            }
        } // for loop
    }

