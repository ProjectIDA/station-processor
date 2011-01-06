#pragma ident "$Id: string.c,v 1.4 2001/10/08 20:03:31 dec Exp $"
/*======================================================================
 *
 *  SAN related strings
 *
 *====================================================================*/
#include <errno.h>
#include "sanio.h"
#include "util.h"

typedef struct {
    char *text;
    int code;
} NAMETABLE;

static NAMETABLE MesgTable[] = {
    {"SAN_NOP",         SAN_NOP},
    {"SAN_DEFCONFIG",   SAN_DEFCONFIG},
    {"SAN_ABORTCAL",    SAN_ABORTCAL},
    {"SAN_STOPACQ",     SAN_STOPACQ},
    {"SAN_STARTACQ",    SAN_STARTACQ},
    {"SAN_SNDCONFIG",   SAN_SNDCONFIG},
    {"SAN_SNDSOH",      SAN_SNDSOH},
    {"SAN_LOGON",       SAN_LOGON},
    {"SAN_LOGOFF",      SAN_LOGOFF},
    {"SAN_SNDKEY",      SAN_SNDKEY},
    {"SAN_REKEYABORT",  SAN_REKEYABORT},
    {"SAN_SNDSOH_A",    SAN_SNDSOH_A},
    {"SAN_SNDSOH_B",    SAN_SNDSOH_B},
    {"SAN_SNDSOH_C",    SAN_SNDSOH_C},
    {"SAN_SNDSOH_D",    SAN_SNDSOH_D},
    {"SAN_CLEARSB",     SAN_CLEARSB},
    {"SAN_SAVE",        SAN_SAVE},
    {"SAN_REBOOT",      SAN_REBOOT},
    {"SAN_HALT",        SAN_HALT},
    {"SAN_VERSION",     SAN_VERSION},
    {"SAN_PID",         SAN_PID},
    {"SAN_DISCONNECT",  SAN_DISCONNECT},
    {"SAN_SETTO",       SAN_SETTO},
    {"SAN_STATE",       SAN_STATE},
    {"SAN_SOH",         SAN_SOH},
    {"SAN_CONFIG",      SAN_CONFIG},
    {"SAN_CONFIGACK",   SAN_CONFIGACK},
    {"SAN_CALIB",       SAN_CALIB},
    {"SAN_CALIBACK",    SAN_CALIBACK},
    {"SAN_LOGMSG",      SAN_LOGMSG},
    {"SAN_FLASH",       SAN_FLASH},
    {"SAN_FLASHACK",    SAN_FLASHACK},
    {"SAN_RCM",         SAN_RCM},
    {"SAN_KEYGEN",      SAN_KEYGEN},
    {"SAN_KEYGENACK",   SAN_KEYGENACK},
    {"SAN_REKEY",       SAN_REKEY},
    {"SAN_REKEYACK",    SAN_REKEYACK},
    {"SAN_PUBLICKEY",   SAN_PUBLICKEY},
    {"SAN_NEWKEY",      SAN_NEWKEY},
    {"SAN_SIGN_Y",      SAN_SIGN_Y},
    {"SAN_SIGNE_Y",     SAN_SIGNED_Y},
    {"SAN_IDENT",       SAN_IDENT},
    {NULL,               -1}
};

char *sanioMessageTypeToString(int message)
{
int i;
static char unknown[] = "UnknownMessageCode=0xXXXXXXXX and slop";

    i = 0;
    while (MesgTable[i].text != NULL) {
        if (MesgTable[i].code = message) return MesgTable[i].text;
    }
    sprintf(unknown, "UnknownMessageCode=0x%08x", message);
    return unknown;
}

/* Revision History
 *
 * $Log: string.c,v $
 * Revision 1.4  2001/10/08 20:03:31  dec
 * 1.6.4 symbols
 *
 * Revision 1.3  2001/09/18 23:19:54  dec
 * rechristened sanioMessageTypeToString(), added some more constants
 *
 * Revision 1.2  2001/05/31 16:36:56  dec
 * removed tabs
 *
 * Revision 1.1  2001/05/02 17:35:49  dec
 * created
 *
 */
