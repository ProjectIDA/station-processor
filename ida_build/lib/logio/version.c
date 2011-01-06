#pragma ident "$Id: version.c,v 1.23 2009/01/26 21:09:29 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "logio.h"

static VERSION version = {2, 4, 1};

/* logio library release notes

2.4.1   01/26/2009
        watchdog.c: added logioStartWatchdog(), logioStopWatchdog()
            and changed logioUpdateWatchdog() to use LOGIO_WATCHDOG
            handle.  Changed logioInitWatchdogDir() to logioInitWatchdog()
            which returns the handle.

2.4.0   01/05/2008
        msg.c: added logioTstamp(), changed DeliverMessage() to use same
        watchdog.c: initial release

2.3.3   10/31/2007
        init.c: replaced string memcpy with strlcpy
        threshold.c: replaced string memcpy with strlcpy

2.3.2   09/14/2007
        msg.c: eliminated extra prefix
  
2.3.1   05/01/2007
        init.c: fixed bug parsing facility from syslogd:facility specifiers

2.3.0   04/18/2007
        init.c: set LOG_USER as the default facility and fixed bug of
          spurious "syslog" files lying about when facility was not explicitly
          given

2.2.5   01/10/2007
        msg.c: fixed 2.2.4 bug causing messages to be truncated after preamble

2.2.4   01/07/2007
        dump.c: snprintf() instead of sprintf()
        init.c: snprintf() instead of sprintf()
        msg.c: snprintf() instead of sprintf(), BSD builds print hex tid
        version.c: snprintf() instead of sprintf()

2.2.3   08/14/2006
        threashold.c: define min() macro if necessary

2.2.2   07/18/2006
        threshold.c: copy no more than strlen(prefix) bytes in logioSetPrefix (aap)_

2.2.1   05/18/2006
        msg.c: print Linux thread id's as hex since they can be so huge

2.2.0   02/08/2005
        init.c: permit NULL lp in logioInit()

2.1.1   10/17/2005
        msg.c: actually, LOG_ERR and LOG_WARN were being explicitly filtered here, so
               fixed that and restored syslog logging at user specified level

2.1.0   10/13/2005
        msg.c: syslog messages all logged at LOG_INFO, since LOG_ERR was getting filtered by syslogd
  
2.0.1   07/26/2005
        threshold.c: check for NULL prefix in logioSetPrefix()

2.0.0   07/25/2005
        msg.c: log to stdout if called with uninitialized handle

1.3.0   12/22/2003 (cvs rtag liblogio_1_3_0 liblogio)
        init.c: allow specifying syslog with syslogd:facility 
        msg.c: include pname[pid:tid] in syslog messages

1.2.2   11/19/2003 (cvs rtag liblogio_1_2_2 liblogio)
        threshold.c: better argument checking

1.2.1   11/04/2003 (cvs rtag liblogio_1_2_1 liblogio)
        string.c: allow for tags w/o the log_ prefix

1.2.0   10/16/2003 (cvs rtag liblogio_1_2_0 liblogio)
        dump.c: initial release

1.1.0   06/30/2003 (cvs rtag liblogio_1_1_0 liblogio)
        Replaced logioOpen with logioInit, eliminated logioClose

1.0.0   06/09/2003 (cvs rtag liblogio_1_0_0 liblogio)
        Initial release
  
 */

char *logioVersionString()
{
static char string[] = "logio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "logio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *logioVersion()
{
    return &version;
}
