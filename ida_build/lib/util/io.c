#pragma ident "$Id: io.c,v 1.2 2007/02/20 17:38:04 dechavez Exp $"
/*======================================================================
 *
 * Read/Write FILEs in network byte order
 *
 *====================================================================*/
#include "util.h"

/* Read/Write 16 bit integer values */

BOOL utilWriteUINT16(FILE *fp, UINT16 value)
{
UINT16 sval;
static size_t len = sizeof(UINT16);

    sval = htons(value);
    return fwrite(&sval, 1, len, fp) == len ? TRUE : FALSE;
}

BOOL utilReadUINT16(FILE *fp, UINT16 *out)
{
UINT16 sval;
static size_t len = sizeof(UINT16);

    if (fread(&sval, 1, len, fp) != len) return FALSE;
    *out = ntohs(sval);

    return TRUE;
}

/* Read/Write 32 bit integer values */

BOOL utilWriteUINT32(FILE *fp, UINT32 value)
{
UINT32 lval;
static size_t len = sizeof(UINT32);

    lval = htonl(value);
    return fwrite(&lval, 1, len, fp) == len ? TRUE : FALSE;
}

BOOL utilReadUINT32(FILE *fp, UINT32 *out)
{
UINT32 lval;
static size_t len = sizeof(UINT32);

    if (fread(&lval, 1, len, fp) != len) return FALSE;
    *out = ntohl(lval);

    return TRUE;
}


/* Read/Write 64 bit integer values */

BOOL utilWriteUINT64(FILE *fp, UINT64 value)
{
UINT64 lval, *ptr64;
static size_t len = sizeof(UINT64);

#ifdef LTL_ENDIAN_HOST
    lval = value;
    ptr64 = &lval;
    utilSwapINT64(ptr64, 1);
#else
    ptr64 = &value;
#endif

    return fwrite(ptr64, 1, len, fp) == len ? TRUE : FALSE;
}

BOOL utilReadUINT64(FILE *fp, UINT64 *out)
{
UINT64 llval;
static size_t len = sizeof(UINT64);

    if (fread(&llval, 1, len, fp) != len) return FALSE;
#ifdef LTL_ENDIAN_HOST
    utilSwapINT64(&llval, 1);
#endif
    *out = llval;

    return TRUE;
}

/* Read/Write 32 bit float values */

BOOL utilWriteREAL32(FILE *fp, REAL32 value)
{
union {
    REAL32 f;
    UINT32 l;
} val;
UINT32 lval;
static size_t len = sizeof(REAL32);

    val.f = value;
    lval  = htonl(val.l);
    return fwrite(&lval, 1, len, fp) == len ? TRUE : FALSE;
}

BOOL utilReadREAL32(FILE *fp, REAL32 *out)
{
union {
    REAL32 f;
    UINT32 l;
} val;
UINT32 lval;
static size_t len = sizeof(REAL32);

    if (fread(&lval, 1, len, fp) != len) return FALSE;
    val.l = htonl(lval);
    *out = val.f;

    return TRUE;
}

/* Read/Write 64 bit float values */

BOOL utilWriteREAL64(FILE *fp, REAL64 value)
{
union {
    REAL64 f;
    UINT64 l;
} val;

    val.f = value;
    return utilWriteUINT64(fp, val.l);
}

BOOL utilReadREAL64(FILE *fp, REAL64 *out)
{
union {
    REAL64 f;
    UINT64 l;
} val;

    if (!utilReadUINT64(fp, &val.l)) return FALSE;
    *out = val.f;

    return TRUE;
}

/* Revision History
 *
 * $Log: io.c,v $
 * Revision 1.2  2007/02/20 17:38:04  dechavez
 * changed length variables from char to size_t
 *
 * Revision 1.1  2006/12/12 22:41:11  dechavez
 * initial release
 *
 */
