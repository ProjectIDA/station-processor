#pragma ident "$Id: action.h,v 1.7 2006/05/18 06:44:53 dechavez Exp $"
/*======================================================================
 * 
 * Prototypes for the event handlers
 *
 *====================================================================*/
#ifndef qdp_fsa_action_included
#define qdp_fsa_action_included

#ifdef __cplusplus
extern "C" {
#endif

#define SEQNO_MASK (QDP_MAX_WINDOW_NBUF - 1)

/* action.c */
void ActionSEND(QDP *qp, QDP_EVENT *event);
void ActionSRVRQ(QDP *qp, QDP_EVENT *event);
void ActionBLDRSP(QDP *qp, QDP_EVENT *event);
void ActionTLU(QDP *qp, QDP_EVENT *event);
void ActionTLD(QDP *qp, QDP_EVENT *event);
void ActionSLEEP(QDP *qp, QDP_EVENT *event);
void ActionREGERR(QDP *qp, QDP_EVENT *event);
void ActionCERR(QDP *qp, QDP_EVENT *event);
void ActionCACK(QDP *qp, QDP_EVENT *event);
void ActionCTRL(QDP *qp, QDP_EVENT *event);
void ActionRQCNF(QDP *qp, QDP_EVENT *event);
void ActionLDCNF(QDP *qp, QDP_EVENT *event);
void ActionRQMEM(QDP *qp, QDP_EVENT *event);
void ActionLDMEM(QDP *qp, QDP_EVENT *event);
void ActionOPEN(QDP *qp, QDP_EVENT *event);
void ActionDTO(QDP *qp, QDP_EVENT *event);
void ActionHBEAT(QDP *qp, QDP_EVENT *event);
void ActionUSRCMD(QDP *qp, QDP_EVENT *event);
void ActionDACK(QDP *qp, QDP_EVENT *event);
void ActionFILL(QDP *qp, QDP_EVENT *event);
void ActionDROP(QDP *qp, QDP_EVENT *event);
void ActionFLUSH(QDP *qp, QDP_EVENT *event);
void ActionCRCERR(QDP *qp, QDP_EVENT *event);
void ActionFAIL(QDP *qp, QDP_EVENT *event);

/* reorder.c */
void ActionINITDACK(QDP *qp);
void ActionDATA(QDP *qp, QDP_EVENT *event);

#ifdef __cplusplus
}
#endif

#endif /* qdp_fsa_action_included */

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
 * $Log: action.h,v $
 * Revision 1.7  2006/05/18 06:44:53  dechavez
 * initial release
 *
 */
