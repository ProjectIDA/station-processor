#pragma ident "$Id: mail.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
void SendAlert(const char *pemailaddr);
void AddArrivalToMailQueue(const char * Sta, double t, double dSTA, float fSNR, float ampl, float per);


/* Revision History
 *
 * $Log: mail.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
