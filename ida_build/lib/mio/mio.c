#pragma ident "$Id: mio.c,v 1.2 2003/12/10 06:00:14 dechavez Exp $"
/*======================================================================
 *
 *  General purose I/O routines to permit single function calls which
 *  support disk files, tapes, etc. on MS-DOS PC.  There is a Unix
 *  version provided in order to aid porting of MS-DOS programs and
 *  which also supports remote tape devices.
 *
 *  All MS-DOS or Unix code which call these routines should also link
 *  in the appropriate mtio library.
 *
 *====================================================================*/
#include "mio.h"
#include "oldmtio.h"

#ifdef MSDOS /* begin MS-DOS version */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

extern int errno;

MIO *mioopen(name, desired_mode)
char *name;
char *desired_mode;
{
int i;
MIO *mio;
char *mode;

    if ((mio = (MIO *) malloc(sizeof(MIO))) == 0) {
        errno = ENOMEM;
        return NULL;
    }
        
    if (strlen(desired_mode) < 1 || strlen(desired_mode) > 2) {
        errno = EINVAL;
        return NULL;
    }

    if ((mode = strdup(desired_mode)) == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    for (i = 0; i < 2; i++) mode[i] = tolower(mode[i]);

    mio->perm = 0x0000;
    if (mode[0] == 'r' || mode[1] == 'r') mio->perm |= MIO_RD;
    if (mode[0] == 'w' || mode[1] == 'w') mio->perm |= MIO_WR;
        
    if (strncmp(name, "/dev/", strlen("/dev/")) == 0) {
        if ((mio->tp = mtopen(name, mode)) == NULL) return NULL;
        mio->type = MIO_TAPE;
    } else if (strcmp(name,"stdin") == 0 || strcmp(name,"STDIN") == 0) {
        mio->fp   = stdin;
        mio->type = MIO_DISK;
        if (strlen(mode) == 2 && mode[1] == 'b') {
            if (setmode(fileno(mio->fp), O_BINARY) == -1) return NULL;
        }
    } else if (strcmp(name,"stdout")==0 || strcmp(name,"STDOUT")==0) {
        mio->fp   = stdout;
        mio->type = MIO_DISK;
        if (strlen(mode) == 2 && mode[1] == 'b') {
            if (setmode(fileno(mio->fp), O_BINARY) == -1) return NULL;
        }
    } else {
        if ((mio->fp = fopen(name, mode)) == NULL) return NULL;
        mio->type = MIO_DISK;
    }

    return mio;
}

long mioread(mio, buffer, count)
MIO *mio;
unsigned char far *buffer;
long count;
{
long bytes;

    if (!(mio->perm & MIO_RD)) return (long) -(errno = EACCES);

    if (mio->type == MIO_DISK) {
        return fread(buffer, sizeof(unsigned char far), count, mio->fp);
    } else {
        do {
            bytes = mtread(mio->tp, buffer, count);
        } while (bytes == 0 && !mioeof(mio));
        return bytes;
    }
}

long miowrite(mio, buffer, count)
MIO *mio;
unsigned char far *buffer;
long count;
{

    if (!(mio->perm & MIO_WR)) return (long) -(errno = EACCES);

    if (mio->type == MIO_DISK) {
        return fwrite(buffer, sizeof(unsigned char far), count, mio->fp);
    } else {
        return mtwrite(mio->tp, buffer, count);
    }
}

int mioeof(mio)
MIO *mio;
{
    if (mio->type == MIO_DISK) {
        return feof(mio->fp);
    } else {
        if (mtpos(mio->tp) == SCSI_AT_EOM || 
            mtpos(mio->tp) == SCSI_AT_EOD) return TRUE;
        return FALSE;
    }
}

void mioclose(mio)
MIO *mio;
{
    if (mio->type == MIO_DISK) {
        fclose(mio->fp);
    } else {
        mtclose(mio->tp);
    }
}

#endif /* end of MS-DOS version */

#ifdef unix /* begin Unix version */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

MIO *mioopen(name, desired_mode)
char *name;
char *desired_mode;
{
int i;
FILE *fp;
MIO *mio;
char *mode;

    if ((mio = (MIO *) malloc(sizeof(MIO))) == 0) {
        errno = ENOMEM;
        return NULL;
    }
        
    if (strlen(desired_mode) < 1 || strlen(desired_mode) > 2) {
        errno = EINVAL;
        return NULL;
    }

    if ((mode = strdup(desired_mode)) == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    for (i = 0; i < 2; i++) mode[i] = tolower(mode[i]);

    mio->perm = 0x0000;
    if (mode[0] == 'r' || mode[1] == 'r') mio->perm |= MIO_RD;
    if (mode[0] == 'w' || mode[1] == 'w') mio->perm |= MIO_WR;
        
    if (strncmp(name, "/dev/", strlen("/dev")) == 0) {
        if ((mio->tp = mtopen(name, mode)) < 0) return NULL;
        mio->type = MIO_TAPE;
        mio->eof_count = 0;
    } else if (strcmp(name,"stdin") == 0 || strcmp(name,"STDIN") == 0) {
        mio->fp   = stdin;
        mio->type = MIO_DISK;
    } else if (strcmp(name,"stdout")==0 || strcmp(name,"STDOUT")==0) {
        mio->fp   = stdout;
        mio->type = MIO_DISK;
    } else {
        if ((mio->fp = fopen(name, mode)) == NULL) return NULL;
        mio->type = MIO_DISK;
    }

    return mio;
}

long mioread(mio, buffer, count)
MIO *mio;
char *buffer;
long count;
{
long bytes;

    if (!(mio->perm & MIO_RD)) return (long) -(errno = EACCES);

    if (mio->type == MIO_DISK) {
        return fread(buffer, sizeof(char), count, mio->fp);
    } else {
        do {
            bytes = mtread(mio->tp, buffer, count);
        } while (bytes == 0 && ++mio->eof_count < 2);
        if (bytes > 0) mio->eof_count = 0;
        return bytes;
    }
}

long miowrite(mio, buffer, count)
MIO *mio;
char *buffer;
long count;
{

    if (!(mio->perm & MIO_WR)) return (long) -(errno = EACCES);

    if (mio->type == MIO_DISK) {
        return fwrite(buffer, sizeof(char), count, mio->fp);
    } else {
        return mtwrite(mio->tp, buffer, count);
    }
}

int mioeof(mio)
MIO *mio;
{
    if (mio->type == MIO_DISK) {
        return feof(mio->fp);
    } else {
        return (mio->eof_count > 1) ? TRUE : FALSE;
    }
}

void mioclose(mio)
MIO *mio;
{
    if (mio->type == MIO_DISK) {
        fclose(mio->fp);
    } else {
        mtclose(mio->tp);
    }
}


#endif /* end of unix version */

/* Revision History
 *
 * $Log: mio.c,v $
 * Revision 1.2  2003/12/10 06:00:14  dechavez
 * include unistd.h to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
