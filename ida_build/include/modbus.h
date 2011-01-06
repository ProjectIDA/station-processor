/*
 * Copyright 2001-2008 Stephane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MODBUS_H_
#define _MODBUS_H_

//CHAVEZ_MOD#include <stdint.h>
//CHAVEZ_MOD#include <termios.h>
//CHAVEZ_MOD#include <arpa/inet.h>
#include "platform.h" //CHAVEZ_MOD

/* version.c */                   //CHAVEZ_MOD
char *modbusVersionString(void);  //CHAVEZ_MOD
VERSION *modbusVersion(void);     //CHAVEZ_MOD

#ifdef __cplusplus
extern "C" {
#endif 

#define MODBUS_TCP_DEFAULT_PORT 502

#define MODBUS_HEADER_LENGTH_RTU           0
#define MODBUS_PRESET_QUERY_LENGTH_RTU     6
#define MODBUS_PRESET_RESPONSE_LENGTH_RTU  2

#define MODBUS_HEADER_LENGTH_TCP           6
#define MODBUS_PRESET_QUERY_LENGTH_TCP    12
#define MODBUS_PRESET_RESPONSE_LENGTH_TCP  8

#define MODBUS_CHECKSUM_LENGTH_RTU      2
#define MODBUS_CHECKSUM_LENGTH_TCP      0        

/* It's not really the minimal length (the real one is report slave ID
 * in RTU (4 bytes)) but it's a convenient size to use in RTU or TCP
 * communications to read many values or write a single one.
 * Maximum between :
 * - HEADER_LENGTH_TCP (6) + slave (1) + function (1) + address (2) +
 *   number (2)
 * - HEADER_LENGTH_RTU (0) + slave (1) + function (1) + address (2) +
 *   number (2) + CRC (2)
*/
#define MODBUS_MIN_QUERY_LENGTH        12

/* Page 102, Application Notes of PI–MBUS–300: 
 *  The maximum length of the entire message must not exceed 256
 *  bytes.
 */
#define MODBUS_MAX_MESSAGE_LENGTH     256

#define MODBUS_MAX_STATUS             800
#define MODBUS_MAX_REGISTERS          100

#define MODBUS_REPORT_SLAVE_ID_LENGTH 75

/* Time out between trames in microsecond */
#define MODBUS_TIME_OUT_BEGIN_OF_TRAME 500000
#define MODBUS_TIME_OUT_END_OF_TRAME   500000

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

/* Function codes */
#define MODBUS_FC_READ_COIL_STATUS          0x01  /* discretes inputs */
#define MODBUS_FC_READ_INPUT_STATUS         0x02  /* discretes outputs */
#define MODBUS_FC_READ_HOLDING_REGISTERS    0x03  
#define MODBUS_FC_READ_INPUT_REGISTERS      0x04
#define MODBUS_FC_FORCE_SINGLE_COIL         0x05
#define MODBUS_FC_PRESET_SINGLE_REGISTER    0x06
#define MODBUS_FC_READ_EXCEPTION_STATUS     0x07
#define MODBUS_FC_FORCE_MULTIPLE_COILS      0x0F
#define MODBUS_FC_PRESET_MULTIPLE_REGISTERS 0x10
#define MODBUS_FC_REPORT_SLAVE_ID           0x11

/* Protocol exceptions */
#define MODBUS_ILLEGAL_FUNCTION        -0x01
#define MODBUS_ILLEGAL_DATA_ADDRESS    -0x02
#define MODBUS_ILLEGAL_DATA_VALUE      -0x03
#define MODBUS_SLAVE_DEVICE_FAILURE    -0x04
#define MODBUS_SERVER_FAILURE          -0x04
#define MODBUS_ACKNOWLEDGE             -0x05
#define MODBUS_SLAVE_DEVICE_BUSY       -0x06
#define MODBUS_SERVER_BUSY             -0x06
#define MODBUS_NEGATIVE_ACKNOWLEDGE    -0x07
#define MODBUS_MEMORY_PARITY_ERROR     -0x08
#define MODBUS_GATEWAY_PROBLEM_PATH    -0x0A
#define MODBUS_GATEWAY_PROBLEM_TARGET  -0x0B

/* Local */
#define MODBUS_COMM_TIME_OUT           -0x0C
#define MODBUS_PORT_SOCKET_FAILURE     -0x0D
#define MODBUS_SELECT_FAILURE          -0x0E
#define MODBUS_TOO_MANY_DATA           -0x0F
#define MODBUS_INVALID_CRC             -0x10
#define MODBUS_INVALID_EXCEPTION_CODE  -0x11
#define MODBUS_CONNECTION_CLOSED       -0x12

/* Internal using */
#define MODBUS_MSG_LENGTH_UNDEFINED -1

typedef enum { MODBUS_RTU, MODBUS_TCP } type_com_t;
typedef enum { MODBUS_FLUSH_OR_RECONNECT_ON_ERROR, MODBUS_NOP_ON_ERROR } error_handling_t;

/* This structure is byte-aligned */
typedef struct {
        /* Descriptor (tty or socket) */
        int fd;
        /* Communication mode: RTU or TCP */
        type_com_t type_com;
        /* Flag debug */
        int debug;
        /* Header length used for offset */
        int header_length;
        /* Checksum length RTU = 2 and TCP = 0 */
        int checksum_length;
        /* TCP port */
        int port;
        /* Device: "/dev/ttyS0", "/dev/ttyUSB0" or "/dev/tty.USA19*"
           on Mac OS X for KeySpan USB<->Serial adapters this string
           had to be made bigger on OS X as the directory+file name
           was bigger than 19 bytes. Making it 67 bytes for now, but
           OS X does support 256 byte file names. May become a problem
           in the future. */
#ifdef __APPLE_CC__
        char device[64];
#else
        char device[16];
#endif
        /* Bauds: 9600, 19200, 57600, 115200, etc */
        int baud;
        /* Data bit */
        uint8_t data_bit;
        /* Stop bit */
        uint8_t stop_bit;
        /* Parity: "even", "odd", "none" */
        char parity[5];
        /* In error_treat with TCP, do a reconnect or just dump the error */
        uint8_t error_handling;
        /* IP address */
        char ip[16];
        /* Save old termios settings */
        struct termios old_tios;
} modbus_param_t;

typedef struct {
        int nb_coil_status;
        int nb_input_status;
        int nb_input_registers;
        int nb_holding_registers;
        uint8_t *tab_coil_status;
        uint8_t *tab_input_status;
        uint16_t *tab_input_registers;
        uint16_t *tab_holding_registers;
} modbus_mapping_t;


/* All functions used for sending or receiving data return:
   - the numbers of values (bits or word) if success (0 or more)
   - less than 0 for exceptions errors
*/

/* Reads the boolean status of coils and sets the array elements in
   the destination to TRUE or FALSE */
int read_coil_status(modbus_param_t *mb_param, int slave,
                     int start_addr, int nb, uint8_t *dest);

/* Same as read_coil_status but reads the slaves input table */
int read_input_status(modbus_param_t *mb_param, int slave,
                      int start_addr, int nb, uint8_t *dest);

/* Reads the holding registers in a slave and put the data into an
   array */
int read_holding_registers(modbus_param_t *mb_param, int slave,
                           int start_addr, int nb, uint16_t *dest);

/* Reads the input registers in a slave and put the data into an
   array */
int read_input_registers(modbus_param_t *mb_param, int slave,
                         int start_addr, int nb, uint16_t *dest);

/* Turns ON or OFF a single coil in the slave device */
int force_single_coil(modbus_param_t *mb_param, int slave,
                      int coil_addr, int state);

/* Sets a value in one holding register in the slave device */
int preset_single_register(modbus_param_t *mb_param, int slave,
                           int reg_addr, int value);

/* Sets/resets the coils in the slave from an array in argument */
int force_multiple_coils(modbus_param_t *mb_param, int slave,
                         int start_addr, int nb, const uint8_t *data);

/* Copies the values in the slave from the array given in argument */
int preset_multiple_registers(modbus_param_t *mb_param, int slave,
                              int start_addr, int nb, const uint16_t *data);

/* Returns the slave id! */
int report_slave_id(modbus_param_t *mb_param, int slave, uint8_t *dest);

/* Initializes the modbus_param_t structure for RTU.
   - device: "/dev/ttyS0"
   - baud:   9600, 19200, 57600, 115200, etc
   - parity: "even", "odd" or "none" 
   - data_bits: 5, 6, 7, 8 
   - stop_bits: 1, 2
*/
void modbus_init_rtu(modbus_param_t *mb_param, const char *device,
                     int baud, const char *parity, int data_bit,
                     int stop_bit);
                     
/* Initializes the modbus_param_t structure for TCP.
   - ip : "192.168.0.5" 
   - port : 1099

   Set the port to MODBUS_TCP_DEFAULT_PORT to use the default one
   (502). It's convenient to use a port number greater than or equal
   to 1024 because it's not necessary to be root to use this port
   number.
*/
void modbus_init_tcp(modbus_param_t *mb_param, const char *ip_address, int port);

/* By default, the error handling mode used is RECONNECT_ON_ERROR.

   With RECONNECT_ON_ERROR, the library will attempt an immediate
   reconnection which may hang for several seconds if the network to
   the remote target unit is down.

   With NOP_ON_ERROR, it is expected that the application will
   check for network error returns and deal with them as necessary.

   This function is only useful in TCP mode.
 */
void modbus_set_error_handling(modbus_param_t *mb_param, error_handling_t error_handling);

/* Establishes a modbus connexion.
   Returns -1 if an error occured. */
int modbus_connect(modbus_param_t *mb_param);

/* Closes a modbus connection */
void modbus_close(modbus_param_t *mb_param);

/* Activates the debug messages */
void modbus_set_debug(modbus_param_t *mb_param, int boolean);

/**
 * SLAVE/CLIENT FUNCTIONS 
 **/

/* Allocates 4 arrays to store coils, input status, input registers and
   holding registers. The pointers are stored in modbus_mapping structure. 

   Returns: TRUE if ok, FALSE on failure
 */
int modbus_mapping_new(modbus_mapping_t *mb_mapping,
                       int nb_coil_status, int nb_input_status,
                       int nb_holding_registers, int nb_input_registers);

/* Frees the 4 arrays */
void modbus_mapping_free(modbus_mapping_t *mb_mapping);

/* Initializes the modbus_param_t structure for a TCP slave (server) */
int modbus_init_listen_tcp(modbus_param_t *mb_param);

/* Listens for any query from a modbus master in TCP 
   Not tested in RTU communication. */
int modbus_listen(modbus_param_t *mb_param, uint8_t *query, int *query_length);

/* Manages the received query.
   Analyses the query and constructs a response.

   If an error occurs, this function construct the response
   accordingly.
*/
void modbus_manage_query(modbus_param_t *mb_param, const uint8_t *query,
                         int query_length, modbus_mapping_t *mb_mapping);


/** 
 * UTILS FUNCTIONS
 **/

/* Sets many input/coil status from a single byte value (all 8 bits of
   the byte value are setted) */
void set_bits_from_byte(uint8_t *dest, int address, const uint8_t value);

/* Sets many input/coil status from a table of bytes (only the bits
   between address and address + nb_bits are setted) */
void set_bits_from_bytes(uint8_t *dest, int address, int nb_bits,
                         const uint8_t *tab_byte);

/* Gets the byte value from many input/coil status.
   To obtain a full byte, set nb_bits to 8. */
uint8_t get_byte_from_bits(const uint8_t *src, int address, int nb_bits);

#ifdef __cplusplus
}
#endif

#endif  /* _MODBUS_H_ */

/* Revision History
 *
 * $Log: modbus.h,v $
 * Revision 1.1  2009/05/14 16:55:36  dechavez
 * initial release
 *
 */
