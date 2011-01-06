#pragma ident "$Id: logrec5.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 5 log record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define CR 0x0d
#define LF 0x0a

#define YEAR_OFF      6  /* year                            */
#define MULT_OFF      8  /* system and 1-hz time multiplier */
#define TAGSYS_OFF   10  /* record stamp system time        */
#define TAGEXT_OFF   14  /* record stamp external time      */
#define TAGHZ_OFF    18  /* record stamp 1-Hz time          */
#define TAGQUAL_OFF  22  /* record stamp Omega quality code */
#define PREFIX       34  /* common header                   */
#define LEVEL_OFF     1  /* log level code                  */
#define UNIT_OFF      3  /* unit (ARS/DAS) code             */
#define PREAMBLE_LEN 25  /* preamble to message             */
#define TEXT_OFF     PREAMBLE_LEN       /* message                */
#define MAXTEXTLEN   988 - PREAMBLE_LEN /* max message length     */

int logrec_rev5(IDA *ida, IDA_LOG_REC *dest, UINT8 *src)
{
UINT8 *ptr;
int count, done;
int numentry = 0;
short stemp;
long  ltemp;

/*  Record time stamp  */

    memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
    dest->ttag.year = stemp;
    memcpy(&stemp, src + MULT_OFF, 2); SSWAB(&stemp, 1);
    dest->ttag.mult = stemp;
    memcpy(&ltemp, src + TAGSYS_OFF, 4); LSWAB(&ltemp, 1);
    dest->ttag.sys = ltemp;
    memcpy(&ltemp, src + TAGEXT_OFF, 4); LSWAB(&ltemp, 1);
    dest->ttag.ext = ltemp;
    memcpy(&ltemp, src + TAGHZ_OFF,  4); LSWAB(&ltemp, 1);
    dest->ttag.hz  = ltemp;
    memcpy(&dest->ttag.code, src + TAGQUAL_OFF, 1);

    ida_filltime(ida, &dest->ttag);

/* Skip over common header  */

    ptr = src + PREFIX;

/* Skip over any initial NULL's (known "feature")  */

    while (*ptr == 0 && ptr < src + IDA_BUFSIZ) ++ptr;
    if (ptr >= src + IDA_BUFSIZ) return 0;

    done = 0;
    while (!done) {

        /* Skip over any initial <CR><LF>'s */

        while ((*ptr == CR || *ptr == LF) && ptr < src + IDA_BUFSIZ) ++ptr;
        if (ptr >= src + IDA_BUFSIZ || *ptr == 0) {
            dest->numentry = numentry;
            return 0;
        }

        /* Determine log level */

        switch (ptr[LEVEL_OFF]) {
          case 'D': dest->entry[numentry].level = IDA_LOGDEBUG; break;
          case 'I': dest->entry[numentry].level = IDA_LOGINFO;  break;
          case 'W': dest->entry[numentry].level = IDA_LOGWARN;  break;
          case 'E': dest->entry[numentry].level = IDA_LOGERROR; break;
          case 'F': dest->entry[numentry].level = IDA_LOGFATAL; break;
          case 'O': dest->entry[numentry].level = IDA_LOGOPER;  break;
          default:  dest->entry[numentry].level = IDA_UNKNOWN;  break;
        }

        /* Determine message source */

        switch (ptr[UNIT_OFF]) {
          case 'D': dest->entry[numentry].atod = IDA_DAS ; break;
          case 'A': dest->entry[numentry].atod = IDA_ARS ; break;
          default:  dest->entry[numentry].atod = IDA_UNKNOWN ; break;
        }

        /* Copy over the preamble  */

        memcpy(dest->entry[numentry].preamble, ptr, PREAMBLE_LEN);
        dest->entry[numentry].preamble[PREAMBLE_LEN] = 0;

        /* Copy over the message CR or LF delimited message */

        ptr += PREAMBLE_LEN;
        dest->entry[numentry].message[count=0] = 0;

        while (
            count < MAXTEXTLEN &&
            *ptr != 0 && *ptr != CR && *ptr != LF
        ) {
            dest->entry[numentry].message[count] = *ptr;
            ++ptr;
            ++count;
        }

        dest->entry[numentry].message[count] = 0;

        ++numentry;

        if (
            numentry == IDA_MAXLOGENTRY ||
            ptr >= src + IDA_BUFSIZ     ||
            *ptr == 0
        ) done = 1;
    }

    dest->numentry = numentry;

    return 0;
}

/* Revision History
 *
 * $Log: logrec5.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
