import java.util.ArrayList;

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

class EFmash extends Exception {;}
class EFmashDescLen extends EFmash {;}
class EFmashRows    extends EFmash {;}
class EFmashVarint  extends EFmash {;}
class EFmashVersion extends EFmash {;}

class FmashRow 
{
    public long m_timestamp = 0;
    public long m_average   = 0;
    public long m_high      = 0;
    public long m_low       = 0;

    FmashRow(long timestamp, long average, long high, long low)
    {
        m_timestamp = timestamp;
        m_average   = average;
        m_high      = high;
        m_low       = low;
    }

}

class Fmash
{

    public static final byte NO_CHANGE = 0x00; // No deltas
    public static final byte CHANGES_1 = 0x01; // 1 delta
    public static final byte CHANGES_2 = 0x02; // 2 deltas
    public static final byte CHANGES_3 = 0x03; // 3 deltas
    public static final byte NO_DATA   = 0x07; // No data

    public static final byte TYPE_AVERAGE = 1;
    public static final byte TYPE_HIGH    = 2;
    public static final byte TYPE_LOW     = 3;

    public static final byte MAX_DESCRIPTION =  8;
    public static final byte ROW_MAP_SIZE = 24;

    public static final byte FALCON_VERSION = 1;
    public static final byte TM_MINUTE = 60;

    public int        m_version       = 0;
    public boolean    m_populated     = false;
    public long       m_start_time    = 0xffffffff;
    public long       m_end_time      = 0x7fffffff;
    public int        m_num_rows      = 0;
    public int        m_channel_id    = 0;
    public String     m_description   = new String();
    public ArrayList<FmashRow> m_rows = new ArrayList<FmashRow>();

    private byte[]    m_data_bitmap   = new byte[24];
    private byte[]    m_maps          = null;
    private byte[]    m_raw_msh       = null;
    private byte[]    m_desc_bytes    = null;
    private int       m_total_length  = 0;
    private int       m_offset        = 0;
    private long      m_first_average = 0;
    private long      m_first_high    = 0;
    private long      m_first_low     = 0;
    private long      m_last_average  = 0;
    private long      m_last_high     = 0;
    private long      m_last_low      = 0;

    Fmash(byte[] raw_msh)
    {
        m_raw_msh      = raw_msh;
        m_total_length = m_raw_msh.length;

        try { 
            _check_version();
            _populate_metadata();
            m_maps = new byte[m_num_rows];
            _populate_rows(); 
            m_populated = true;
        } catch (EFmash e) {;}
    }

    private void _check_version() throws EFmashVersion
    {
        m_version = (int)(m_raw_msh[0] | (m_raw_msh[1] << 8));
        if ((m_version & 0x7fff) > FALCON_VERSION) {
            throw new EFmashVersion();
        }
    }

    private void _populate_metadata() throws EFmashDescLen
    {
        int desc_len;

        m_start_time  = (long)(m_raw_msh[2] | (m_raw_msh[3] <<  8) | (m_raw_msh[4] << 16) | (m_raw_msh[5] << 24));
        m_end_time    = (long)(m_raw_msh[6] | (m_raw_msh[7] <<  8) | (m_raw_msh[8] << 16) | (m_raw_msh[9] << 24));
        m_channel_id  = (int)(m_raw_msh[10] | (m_raw_msh[11] << 8));
        m_num_rows    = (int)(m_raw_msh[12] | (m_raw_msh[13] << 8));
        for (int i=0; i < 24; i++) {
            m_data_bitmap[i] = m_raw_msh[14+i];
        }
        desc_len = (int)(m_raw_msh[38]);

        if ((desc_len < 1) || (desc_len > 8)) {
            throw new EFmashDescLen();
        }

        m_desc_bytes = new byte[desc_len];

        for (int i=0; i < desc_len; i++) {
            m_desc_bytes[i] = m_raw_msh[39+i];
        }
        m_description = new String(m_desc_bytes);
        
        // desc_offset + length_byte + description + null_byte
        m_offset = 38 + 1 + desc_len + 1;
    }

    private void _populate_rows() throws EFmashRows
    {
        long current_time;
        byte item;
        byte map;
        FmashRow row = null;

        try {
            m_first_average = _get_varint();
            m_first_high    = _get_varint();
            m_first_low     = _get_varint();
            m_last_average  = m_first_average;
            m_last_high     = m_first_high;
            m_last_low      = m_first_low;

            _get_maps();

            current_time = m_start_time;

            for (int i=0; i < m_maps.length; i++) {
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
                } // switch statement
                row = new FmashRow(current_time, m_last_average, m_last_high, m_last_low);
                m_rows.add(row);
            } // for loop
            m_last_average = _get_varint();
            m_last_high    = _get_varint();
            m_last_low     = _get_varint();
        } catch (EFmashVarint e) {
            throw new EFmashRows();
        }
    }

    private long _get_varint() throws EFmashVarint
    {
        int       varint_max = 5;
        int       count      = 0;
        byte      value      = 0;
        ArrayList bytes      = null;

        if ((m_total_length - m_offset) < 5) {
            varint_max = m_total_length - m_offset;
        }
        if (varint_max < 1) {
            throw new EFmashVarint();
        }

        for (int i=0; i < varint_max; i++) {
            count++;
            value = m_raw_msh[m_offset+i];

            switch (count) {
                case 1:  value |= (long)((value >> 2) & 0x0000001f); break;
                case 5:  value |= (long)((value & 0x3f) << 26); break;
                default: value |= (long)((value & 0x000007f) << (count * 7 - 2)); 
            }

            if ((0x80 & value) == 0) {
                if ((value & 0x40) != 0) {
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
            pos = (i / 8) * 3;
            rotation = i % 8;
            switch (rotation) {
                case 0: m_maps[i] =  (byte)((m_data_bitmap[pos]       ) & 0x07); break;
                case 1: m_maps[i] =  (byte)((m_data_bitmap[pos]   >> 3) & 0x07); break;
                case 2: m_maps[i] = (byte)(((m_data_bitmap[pos]   >> 6) & 0x03)|
                                     (byte)((m_data_bitmap[pos+1] << 2) & 0x04)); break;
                case 3: m_maps[i] =  (byte)((m_data_bitmap[pos+1] >> 1) & 0x07); break;
                case 4: m_maps[i] =  (byte)((m_data_bitmap[pos+1] >> 4) & 0x07); break;
                case 5: m_maps[i] = (byte)(((m_data_bitmap[pos+1] >> 7) & 0x01)| 
                                     (byte)((m_data_bitmap[pos+2] << 1) & 0x06)); break;
                case 6: m_maps[i] =  (byte)((m_data_bitmap[pos+2] >> 2) & 0x07); break;
                case 7: m_maps[i] =  (byte)((m_data_bitmap[pos+2] >> 5) & 0x07); break;
            }
        } // for loop
    }
}

