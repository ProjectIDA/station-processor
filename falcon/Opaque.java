import java.util.ArrayList;

class Opaque {
    public static final byte STATE_UNSET    = 0xff;
    public static final byte STATE_SINGLE   = 0x00;
    public static final byte STATE_START    = 0x04;
    public static final byte STATE_CONTINUE = 0x0c;
    public static final byte STATE_END      = 0x08;

    private byte[] m_blockette = null;

    private boolean m_big_ending = true;
    private int     m_state = STATE_UNSET; 

    private int  m_blockette_type    = 0;
    private int  m_next_blockette    = 0;
    private int  m_total_length      = 0;
    private int  m_data_offset       = 0;
    private long m_record_number     = 0;
    private byte m_data_word_order   = 0;
    private byte m_data_flags        = 0;
    private byte m_num_header_fields = 0; 
    private ArrayList<String> m_header_fields = null;
    private byte[] m_data            = null;

    Opaque()
    {
        m_header_fields = new ArrayList<String>();
    }

    public boolean get_big_endian()   {return m_big_endian;}
    public int get_state()            {return m_state;}
    public int get_blockette_type()   {return m_blockette_type;}
    public int get_next_blockette()   {return m_next_blockette;}
    public int get_total_length()     {return m_total_length;}
    public int get_data_offset()      {return m_data_offset;}
    public long get_record_number()   {return m_record_number;}
    public byte get_data_word_order() {return m_data_word_order;}
    public byte get_data_flags()      {return m_data_flags;}
    public ArrayList<String> get_header_fields() {return m_header_fields;}
    public int get_data()             {return m_data;}


    public void set_big_endian(boolean big_endian) {m_big_endian = big_endian;}

    public int add_blockette(byte[] blockette)
    {
        m_header_fields.clear()
        m_blockette = blockette;
        if (m_blockette.length < 15) {
            return m_state = STATE_UNSET; // Header is too short
        }
        _get_header();
        if (m_num_header_fields > 0) {
            _get_fields();
        }
        if (m_data_offset >= m_total_length) {
            return m_state = STATE_UNSET; // Impossible data offset
        }

        int new_length  = 0;
        int start_index = 0;
        switch (m_data_flags & 0xc) {
            case STATE_CONTINUE:
            case STATE_END:
                if (m_data != null) {
                    new_length = start_index = m_data.length;
                }
            case STATE_SINGLE:
            case STATE_START:
                new_length += m_total_length - m_data_offset;
                for (int i=0; i < start_index; i++) {
                    temp[i] = m_data[i];
                }
                for (int i=0; i < (m_total_length - m_data_offset); i++) {
                    temp[start_index+i] = m_blockette[m_data_offset+i];
                }
                m_data = temp;
                m_state = m_data_flags & 0xc;
                break;
            default:
                return m_state = STATE_UNSET; // Invalid flag (should be impossible)
        }

        return m_state;
    }

    private void _get_header()
    {
        if (m_big_endian == true) {
            _get_header_big_endian();
        }
        else {
            _get_header_little_endian();
        }
        m_data_word_order   = m_blockette[12];
        m_data_flags        = m_blockette[13];
        m_num_header_fields = m_blockette[14];
    }

    private void _get_header_big_endian()
    {
        m_blockette_type    = (int)m_blockette[0] << 8 | (int)m_blockette[1];
        m_next_blockette    = (int)m_blockette[2] << 8 | (int)m_blockette[3];
        m_total_length      = (int)m_blockette[4] << 8 | (int)m_blockette[5];
        m_data_offset       = (int)m_blockette[6] << 8 | (int)m_blockette[7];
        m_record_number     = (long)m_blockette[8]  << 24 |
                              (long)m_blockette[9]  << 16 |
                              (long)m_blockette[10] <<  8 |
                              (long)m_blockette[11];
    }

    private void _get_header_little_endian()
    {
        m_blockette_type    = (int)m_blockette[0] | (int)m_blockette[1] << 8;
        m_next_blockette    = (int)m_blockette[2] | (int)m_blockette[3] << 8;
        m_total_length      = (int)m_blockette[4] | (int)m_blockette[5] << 8;
        m_data_offset       = (int)m_blockette[6] | (int)m_blockette[7] << 8;
        m_record_number     = (long)m_blockette[8]        |
                              (long)m_blockette[9]  <<  8 |
                              (long)m_blockette[10] << 16 |
                              (long)m_blockette[11] << 24;
    }

    private int _get_fields()
    {
        byte[] tmp   = null;
        int    start = 15;
        int    end   = 15;

        for (int i=0; i < m_num_header_fields; i++) {
            while (m_blockette[end] != 0x127) {end++;}
            if (end - start > 0) {
                tmp = new byte[end-start];
                for (int j=0; j < end-start; j++) {
                    tmp[j] = m_blockette[start+j];
                }
                m_header_fields.add(new String(tmp));
            }
            start = ++end;
        }
    }

}

