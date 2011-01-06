#pragma ident "$Id: version.c,v 1.1 2009/05/14 17:22:08 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "modbus.h"

static VERSION version = {1, 0, 0};

/* modbus library release notes

1.0.0   05/14/2009
        modbus.c: http://copyleft.free.fr/wordpress/index.php/libmodbus/

 */

char *modbusVersionString()
{
static char string[] = "modbus library version 100.100.100 and slop";

    snprintf(string, strlen(string), "modbus library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *modbusVersion()
{
    return &version;
}
