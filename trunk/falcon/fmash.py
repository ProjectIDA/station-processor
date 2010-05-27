import struct
try:
    import java.util.ArrayList as ArrayList
    HAS_JAVA = True
except:
    HAS_JAVA = False

NO_CHANGE = 0x00 # No deltas
CHANGES_1 = 0x01 # 1 delta
CHANGES_2 = 0x02 # 2 deltas
CHANGES_3 = 0x03 # 3 deltas
NO_DATA   = 0x07 # No data

TYPE_AVERAGE = 1
TYPE_HIGH    = 2
TYPE_LOW     = 3

MAX_DESCRIPTION =  8
ROW_MAP_SIZE = 24

FALCON_VERSION = 1
TM_MINUTE = 60

# Version/Type : uint16_t ---- (2 bytes)        : 0
# Start Time   : time_t ------ (4 bytes)        : 2
# End Time     : time_t ------ (4 bytes)        : 6
# Channel ID   : uint16_t ---- (2 bytes)        : 10
# Num Rows     : uint16_t ---- (2 bytes)        : 12
# Data Bitmap  : uint8_t[24] - (24 bytes)       : 14
# Description  : pascal str. - (3-10 bytes)     : 38
# First Avg    : varint ------ (1 - 5 bytes)
# First Hi     : varint ------ (1 - 5 bytes)
# First Low    : varint ------ (1 - 5 bytes)
# Data Bytes   : varint(s) --- (0+ bytes)
# Last Avg     : varint ------ (1 - 5 bytes)
# Last Hi      : varint ------ (1 - 5 bytes)
# Last Low     : varint ------ (1 - 5 bytes)

class EFmashVersion(Exception): pass
class EFmashDescLen(Exception): pass
class EFmashOverrun(Exception): pass

class FmashRow:
    def __init__(self, timestamp, average, high, low):
        self.timestamp = timestamp
        self.average   = average
        self.hight     = hight
        self.low       = low

class Fmash:
    def __init__(self, raw_msh):
        self.raw_msh       = raw_msh
        self.total_lengh   = len(self.raw_msh)
        self.offset        = 0
        
        self.version       = 0
        self.start_time    = 0xffffffff
        self.end_time      = 0x7fffffff
        self.num_rows      = 0
        self.data_bitmap   = None
        self.desc_length   = 0
        self.first_average = 0
        self.first_high    = 0
        self.first_low     = 0
        self.rows          = []
        self.last_average  = 0
        self.last_high     = 0
        self.last_low      = 0

        self._check_version()
        self._populate_metadata()
        self._populate_rows()
        self._msh_to_dict()
        if HAS_JAVA:
            self._generate_array_list()

    def _check_version(self):
        version = struct.unpack('>H', self.raw_msh[0:2])[0]
        if version & 0x7fff > FALCON_VERSION:
            raise EFmashVersion()

    def _populate_metadata(self):
        values = struct.unpack('>HLLHH24B', self.raw_msh[0:25])

        self.version     = values[0]
        self.start_time  = values[1]
        self.end_time    = values[2]
        self.channel_id  = values[3]
        self.num_rows    = values[4]
        self.data_bitmap = values[5:29]
        self.desc_length = values[29]

        if 1 > desc_length > 8:
            raise EFmashDescLen()

        # desc_offset + length_byte + description + null_byte
        desc_end = 38 + 1 + desc_length + 1
        self.description = stuct.unpack('>%dp' % desc_length, self.raw_msh[38:desc_end])[0]

        self.offset = desc_end

    def _populate_rows(self, raw_msh):
        self.rows = {}
        self.first_average = self._get_varint()
        self.first_high    = self._get_varint()
        self.first_low     = self._get_varint()
        last_average = self.first_average
        last_high    = self.first_high
        last_low     = self.first_low

        self._get_maps()
        current_time = self.start_time
        for map in self.maps:
            if map in (CHANGES_1,CHANGES_2,CHANGES_3):
                for j in range(0,map):
                    byte = struct.unpack('>B', self.raw_msh[self.offset,self.offset+1])[0]
                    if (byte & 0x3) == TYPE_AVERAGE:
                        last_average += self._get_varint()
                    elif (byte & 0x3) == TYPE_HIGH:
                        last_high += self._get_varint()
                    elif (byte & 0x3) == TYPE_LOW:
                        last_low += self._get_varint()
            self.rows.append((current_time,last_average,last_high,last_low))
            current_time += TM_MINUTE

        self.last_average = self._get_varint()
        self.last_high    = self._get_varint()
        self.last_low     = self._get_varint()

        return dict

    def _generate_array_list():
        self.array_list = ArrayList()
        for t,a,h,l in self.rows:
            self.array_list.add(FmashRow(t,a,h,l))

    def _get_varint(self):
        varint_max = 5
        if (self.total_length - self.offset) < 5:
            varint_max = self.total_length - offset
        if varint_max < 1:
            raise FMashOverrun()
        result,count = varint_to_int32(struct.unpack('>%dB', self.raw_msh[offset:offset-varin_max]))
        self.offset += count

    def _get_maps(self):
        self.maps = []
        for index in range(0,self.num_rows):
            pos = (index / 8) * 3;
            rotation = index % 8
            if rotation == 0:
                maps.append(((self.data_bitmap[map+pos]       ) & 0x07))
            elif rotation == 1:
                maps.append(((self.data_bitmap[map+pos]   >> 3) & 0x07))
            elif rotation == 2:
                maps.append(((self.data_bitmap[map+pos]   >> 6) & 0x03) | ((self.data_bitmap[map+pos+1] << 2) & 0x04))
            elif rotation == 3:
                maps.append(((self.data_bitmap[map+pos+1] >> 1) & 0x07))
            elif rotation == 4:
                maps.append(((self.data_bitmap[map+pos+1] >> 4) & 0x07))
            elif rotation == 5:
                maps.append(((self.data_bitmap[map+pos+1] >> 7) & 0x01) | ((self.data_bitmap[map+pos+2] << 1) & 0x06))
            elif rotation == 6:
                maps.append(((self.data_bitmap[map+pos+2] >> 2) & 0x07))
            elif rotation == 7:
                maps.append(((self.data_bitmap[map+pos+2] >> 5) & 0x07))


def varint_to_int32(bytes):
    sign_map = {
        1 : 0xffffffe0,
        2 : 0xfffff000,
        3 : 0xfff80000,
        4 : 0xfc000000,
    }

    result = 0
    count  = 0
    for byte in bytes:
        count += 1
        if count == 1:
            result |= (byte >> 2) & 0x0000001f;
        elif count == 5:
            result |= (byte & 0x3f) << 26;
            break
        else:
            result |= (byte & 0x0000007f) << (count * 7 - 2);
            
        if (0x80 & byte) == 0:
            if byte & 0x40:
                result |= sign_map[count]
            break

    return (result,count)

