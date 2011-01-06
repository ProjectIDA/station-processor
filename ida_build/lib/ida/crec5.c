#pragma ident "$Id: crec5.c,v 1.2 2006/06/27 00:18:47 akimov Exp $"
/*======================================================================
 *
 *  Load a rev 5 calibration record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define YEAR_OFF     6  /* year                               */
#define MULT_OFF     8  /* system and 1-hz time multiplier    */
#define SYS_OFF     10  /* record tag system time             */
#define EXT_OFF     14  /* record tag external time           */
#define HZ_OFF      18  /* record tag 1-Hz time               */
#define QUAL_OFF    22  /* record tag Omega quality code      */
#define UNIT_OFF    24  /* unit id                            */
#define STATE_OFF   34  /* calibration on/off flag            */
#define FUNC_OFF    36  /* calibration function code          */
#define CHAN_OFF    38  /* calibration channels bitmap        */
#define GAIN_OFF    40  /* array of calibration channel gains */
#define ATTEN_OFF   64  /* DAC attenuation factor             */
#define PERIOD_OFF  66  /* period or width                    */
#define POLY_OFF    70  /* polynomial % 2 or impulse interval */
#define SEED_OFF    74  /* random number generator seed       */

#define IMPULSE  0
#define SQUARE   1
#define SINE     2
#define TRIANGLE 3
#define RANDOM   4

#define MAXCALCHAN 12

#define BBCAL  0x01
#define SPCAL  0x02
#define INTCAL 0x04

int crec_rev5(IDA *ida, IDA_CALIB_REC *dest, UINT8 *src)
{
int i;
char *ptr;
short stemp;
long  ltemp;

/*  Digitizer info  */

    memcpy(&stemp, src + UNIT_OFF, 2); SSWAB(&stemp, 1);
    if (stemp == 0) {
        dest->atod = IDA_ARS;
    } else {
        dest->atod = IDA_DAS;
    }

/*  Record time tag  */

    memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
    dest->ttag.year = stemp;

    memcpy(&stemp, src + MULT_OFF, 2); SSWAB(&stemp, 1);
    dest->ttag.mult = stemp;

    memcpy(&ltemp, src + SYS_OFF, 4); LSWAB(&ltemp, 1);
    dest->ttag.sys = ltemp;
    memcpy(&ltemp, src + EXT_OFF, 4); LSWAB(&ltemp, 1);
    dest->ttag.ext = ltemp;
    memcpy(&ltemp, src + HZ_OFF,  4); LSWAB(&ltemp, 1);
    dest->ttag.hz  = ltemp;
    memcpy(&dest->ttag.code, src + QUAL_OFF, 1);

    ida_filltime(ida, &dest->ttag);

/*  On/Off flag  */

    memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
    dest->state = stemp;

/*  Function code  */

    memcpy(&stemp, src + FUNC_OFF, 2); SSWAB(&stemp, 1);
    switch (stemp) {
        case IMPULSE:  dest->func = IDA_IMPULSE;  break;
        case SQUARE:   dest->func = IDA_SQUARE;   break;
        case SINE:     dest->func = IDA_SINE;     break;
        case TRIANGLE: dest->func = IDA_TRIANGLE; break;
        case RANDOM:   dest->func = IDA_RANDOM;   break;
        default:       dest->func = IDA_UNKNOWN;  break;
    };
    
/*  Attenuation factor  */

    memcpy(&stemp, src + ATTEN_OFF, 2); SSWAB(&stemp, 1);
    dest->atten = stemp;

/*  Function period / pulse width  */

    memcpy(&stemp, src + PERIOD_OFF, 2); SSWAB(&stemp, 1);
    dest->period = dest->width = stemp;

/*  Primitive polynomial % 2 or pulse interval  */

    memcpy(&stemp, src + POLY_OFF, 2); SSWAB(&stemp, 1);
    dest->poly = dest->interval = stemp;

/*  Random number generator seed  */

    memcpy(&stemp, src + SEED_OFF, 2); SSWAB(&stemp, 1);
    dest->seed = stemp;

/*  Calibration channel bitmap  */

    memcpy(&stemp, src + SEED_OFF, 2); SSWAB(&stemp, 1);
    dest->chan = 0x00;
    if (stemp & BBCAL)  dest->chan |= IDA_BBCAL;
    if (stemp & SPCAL)  dest->chan |= IDA_SPCAL;
    if (stemp & INTCAL) dest->chan |= IDA_INTCAL;

/*  Calibration DAC gains  */

    if (dest->chan & IDA_INTCAL) {
        if (MAXCALCHAN > IDA_MAXCALCHAN) return -101;
        for (ptr=src+GAIN_OFF, i=0; i < MAXCALCHAN; i++, ptr+=2) {
            memcpy(&stemp, ptr, 2); SSWAB(&stemp, 1);
            dest->gain[i] = stemp;
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: crec5.c,v $
 * Revision 1.2  2006/06/27 00:18:47  akimov
 * misc WIN32 warning removals
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
