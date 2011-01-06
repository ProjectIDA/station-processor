#pragma ident "$Id: sint.c,v 1.3 2004/06/24 18:18:40 dechavez Exp $"
/*======================================================================
 *
 *  IDA10 sample intervals
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

void ida10SintToFactMult(REAL64 sint, INT16 *fact, INT16 *mult)
{
    if (sint >= 1.0) {
        *fact = (INT16) -sint;
        *mult = 1;
    } else {
        *fact = (INT16) rint((1.0 / sint));
        *mult = 1;
    }
}

REAL64 ida10FactMultToSint(INT16 fact, INT16 mult)
{
   if (fact > 0 && mult > 0) {
        return 1.0 / (double) fact * (double) mult;
    } else if (fact > 0 && mult < 0) {
        return (double) -mult / (double) fact;
    } else if (fact < 0 && mult > 0) {
        return (double) -fact / (double) mult;
    } else {
        return (double) fact / (double) mult;
    }
}

#ifdef DEBUG_TEST

#include <stdio.h>
#include <math.h>

main(argc, argv)
int argc;
char *argv[];
{
REAL64 sint;
INT16 fact, mult;

    if (argc == 2) {
        sint = (REAL64) atof(argv[1]);
        ida10SintToFactMult(sint, &fact, &mult);
        printf("%lf splits to fact = %hd, mult = %hd\n", sint, fact, mult);
    } else if (argc == 3) {
        fact = atoi(argv[1]);
        mult = atoi(argv[2]);
        sint = ida10FactMultToSint(fact, mult);
        printf("fact = %hd, mult = %hd => sint = %lf\n", fact, mult, sint);
    } else {
        fprintf(stderr, "usage: %s sint -OR- %s fact mult\n",
            argv[0], argv[0]
        );
        exit(1);
    }

    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: sint.c,v $
 * Revision 1.3  2004/06/24 18:18:40  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.2  2003/12/10 05:47:05  dechavez
 * included math.h
 *
 * Revision 1.1  2002/09/09 21:46:45  dec
 * created
 *
 */
