#pragma ident "$Id: iboot.h,v 1.1 2008/10/10 22:54:31 dechavez Exp $"
/*======================================================================
 *
 *  iboot library support
 *
 *====================================================================*/
#ifndef iboot_h_included
#define iboot_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IBOOT_DEFAULT_SERVER "192.168.1.254"
#define IBOOT_DEFAULT_PASSWD "PASS"
#define IBOOT_DEFAULT_PORT   80
#define IBOOT_ON             'n'
#define IBOOT_OFF            'f'
#define IBOOT_CYCLE          'c'
#define IBOOT_QUERY          'q'
#define IBOOT_ON_STRING      "ON"
#define IBOOT_OFF_STRING     "OFF"
#define IBOOT_CYCLE_STRING   "CYCLE"
#define IBOOT_BUSY_STRING    "BUSY"

/* iboot.c */
char *ibootCommand(char *server, int port, char *passwd, char action);

/* version.c */

VERSION *ibootVersion(VOID);
char *ibootVersionString(VOID);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: iboot.h,v $
 * Revision 1.1  2008/10/10 22:54:31  dechavez
 * initial release
 *
 */
