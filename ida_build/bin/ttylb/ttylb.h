#pragma ident "$Id: ttylb.h,v 1.2 2005/05/26 23:48:43 dechavez Exp $"
#ifndef ttylb_h_included
#define ttylb_h_included

#include "ttyio.h"
#include "util.h"

void StartReader(char *port, long baud, int flow, int sbits);
void StartWriter(char *port, long baud, int flow, int sbits);

#endif

/* Revision History 
 *
 * $Log: ttylb.h,v $
 * Revision 1.2  2005/05/26 23:48:43  dechavez
 * switch to new ttyio calls
 *
 * Revision 1.1  2000/05/15 22:35:10  dec
 * import existing sources
 *
 */
