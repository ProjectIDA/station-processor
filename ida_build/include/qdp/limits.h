#pragma ident "$Id: limits.h,v 1.4 2009/02/04 05:41:26 dechavez Exp $"
/*======================================================================
 *
 *  Quanterra QDP limits
 *
 *====================================================================*/
#ifndef qdp_limits_h_included
#define qdp_limits_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QDP_MAX_WINDOW_NBUF     128 /* sliding window */
#define QDP_MAX_MTU             576
#define QDP_MIN_MTU             276
#define QDP_CMNHDR_LEN           12
#define QDP_IP_OVERHEAD          20
#define QDP_UPD_OVERHEAD          8
#define QDP_OVERHEAD            (QDP_IP_OVERHEAD + QDP_UPD_OVERHEAD + QDP_CMNHDR_LEN)
#define QDP_MAX_PAYLOAD         (QDP_MAX_MTU - QDP_OVERHEAD)
#define QDP_MAX_C1_MEM_PAYLOAD  448
#define QDP_MAX_C1_MEM_CFGDATA  438
#define QDP_TOKEN_BUFLEN       8192
#define QDP_MAX_MEM_SEG          19
#define QDP_MAX_SPS             200

#define QDP_NCHAN                 6 /* number of channels */
#define QDP_NFREQ                 8 /* number of frequencies (freq bit numbers) */
#define QDP_NSENSOR               2 /* number of sensors */
#define QDP_NLP                   4 /* number of logical telemtry ports */
#define QDP_NSERIAL               2 /* number of serial ports */
#define QDP_NSC                   8 /* number of sensor control lines */
#define QDP_NGID                  8 /* number of GPS ID strings */
#define QDP_GIDLEN               31 /* max length of GPS ID strings */
#define QDP_IDENTLEN             31 /* max length of checkout identitier */

#ifdef __cplusplus
}
#endif

#endif /* qdp_limits_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: limits.h,v $
 * Revision 1.4  2009/02/04 05:41:26  dechavez
 * added QDP_NGID, QDP_GIDLEN, QDP_IDENTLEN constants
 *
 * Revision 1.3  2009/01/23 23:53:24  dechavez
 * added QDP_NSC
 *
 * Revision 1.2  2008/02/15 22:57:50  akimov
 * added missing right bracket for C++ builds
 *
 * Revision 1.1  2007/12/20 22:44:06  dechavez
 * created
 *
 */
