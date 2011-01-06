#pragma ident "$Id: myip.c,v 1.1 2006/05/04 20:36:52 dechavez Exp $"
/*======================================================================
 * 
 * Determine my IP address.  Very kludgy.
 *
 *====================================================================*/
#include "util.h"

UINT32 utilMyIpAddr(void)
{
#define TMPBUFLEN 1024
char tmp[TMPBUFLEN];
struct hostent *hp;
struct sockaddr_in myaddress;
UINT32 result;

    if (gethostname(tmp, TMPBUFLEN) != 0) return 0;

    hp = gethostbyname(tmp);
    if (hp == (struct hostent *) NULL) return 0;
    memcpy((void *) &myaddress.sin_addr, hp->h_addr, hp->h_length);
    myaddress.sin_family = AF_INET;

    result = (UINT32) inet_addr(inet_ntoa(myaddress.sin_addr));

    return result;
}


/* Revision History
 *
 * $Log: myip.c,v $
 * Revision 1.1  2006/05/04 20:36:52  dechavez
 * initial release
 *
 */
