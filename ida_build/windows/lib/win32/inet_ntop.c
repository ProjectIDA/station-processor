#pragma ident "$Id: inet_ntop.c,v 1.1 2007/02/20 02:13:24 dechavez Exp $"

#include "platform.h"


const char *inet_ntop(int af, const void *src, char *dst, long cnt)
	{
    if (af == AF_INET)
        {
		sprintf(dst,"%u.%u.%u.%u",((char *)src)[0],((char *)src)[1],((char *)src)[2],((char *)src)[3]);
        return dst;
        }
    return NULL;
	}

/* Revision History
 *
 * $Log: inet_ntop.c,v $
 * Revision 1.1  2007/02/20 02:13:24  dechavez
 * initial release (aap 2007-12-10)
 *
 */
