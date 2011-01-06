#pragma ident "$Id: md5.c,v 1.7 2007/10/31 17:13:57 dechavez Exp $"
/*======================================================================
 * 
 * Generate MD5 digest for server challenge response
 *
 *====================================================================*/
#include "qdp.h"
#include "md5.h"

void qdpMD5(QDP_TYPE_C1_SRVRSP *c1_srvrsp, UINT64 authcode)
{
int i;
char *ptr;
md5_state_t state;
md5_byte_t digest[16];
char string[80 + 1];
    
    ptr = string;
    snprintf(string, 81, "%016llx%08lx%04x%04x%016llx%016llx%016llx",
        c1_srvrsp->challenge,
        c1_srvrsp->dp.ip,
        c1_srvrsp->dp.port,
        c1_srvrsp->dp.registration,
        authcode,
        c1_srvrsp->serialno,
        c1_srvrsp->random
    );

    md5_init(&state);
    md5_append(&state, (const md5_byte_t *) string, strlen(string));
    md5_finish(&state, digest);

    memcpy(c1_srvrsp->md5, digest, 16);
#ifdef LTL_ENDIAN_HOST
    utilSwapUINT32(c1_srvrsp->md5, 4);
#endif
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: md5.c,v $
 * Revision 1.7  2007/10/31 17:13:57  dechavez
 * replaced sprintf with snprintf
 *
 * Revision 1.6  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
