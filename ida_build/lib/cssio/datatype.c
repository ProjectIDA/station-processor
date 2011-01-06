#pragma ident "$Id: datatype.c,v 1.1.1.1 2000/02/08 20:20:23 dec Exp $"
/*======================================================================
 *
 *  Given a word size and util_order() byte order descriptor, return the
 *  appropriate datatype string.
 * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <sys/types.h>
#include "cssio.h"
#include "util.h"

char *cssio_datatype(wrdsiz, order, integer)
int wrdsiz;
unsigned long order;
int integer;
{
static char *s2 = "s2";
static char *s4 = "s4";
static char *t4 = "t4";
static char *i2 = "i2";
static char *i4 = "i4";
static char *f4 = "f4";
static char *undefined = "-";

    if ( integer && order == BIG_ENDIAN_ORDER && wrdsiz == 2) return s2;
    if ( integer && order == BIG_ENDIAN_ORDER && wrdsiz == 4) return s4;
    if (!integer && order == BIG_ENDIAN_ORDER && wrdsiz == 4) return t4;

    if ( integer && order == LTL_ENDIAN_ORDER && wrdsiz == 2) return i2;
    if ( integer && order == LTL_ENDIAN_ORDER && wrdsiz == 4) return i4;
    if (!integer && order == LTL_ENDIAN_ORDER && wrdsiz == 4) return f4;

    return undefined;
}

/* Revision History
 *
 * $Log: datatype.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
