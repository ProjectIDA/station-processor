#pragma ident "$Id: mio.c,v 1.2 2005/05/17 21:36:39 dechavez Exp $"
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
#include "platform.h"
#include "mio.h"
#include "win32\scsitape.h"
#include "winmt.h"

/*extern int errno;*/

MIO *mioopen(char *name, char *desired_mode)
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
		mio->tp = mtopen(name, mode);
        if (mio->tp == NULL) return NULL;
		mio->type = MIO_TAPE;
//		return NULL;
    } else if (strcmp(name,"stdin") == 0 || strcmp(name,"STDIN") == 0) {
        mio->fp   = stdin;
        mio->type = MIO_DISK;
        if (strlen(mode) == 2 && mode[1] == 'b') {
            if (_setmode(_fileno(mio->fp), _O_BINARY) == -1) return NULL;
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

long mioread(MIO *mio, unsigned char *buffer, long count)
{
long bytes=0;

    if (!(mio->perm & MIO_RD)) return (long) -(errno = EACCES);

    if (mio->type == MIO_DISK) 
		{
        return fread(buffer, sizeof(unsigned char) , count, mio->fp);
		}
	else 
		{
        bytes = mtread(mio->tp, buffer, count);
//			np+=bytes;
//			count-=bytes;
//			} 
//		while (bytes == 0 && !mioeof(mio));
//		while (bytes != 0);
        return bytes;
		}
}

long miowrite(MIO *mio,unsigned char *buffer,long count)
{

    if (!(mio->perm & MIO_WR)) return (long) -(errno = EACCES);

    if (mio->type == MIO_DISK) {
        return fwrite(buffer, sizeof(unsigned char), count, mio->fp);
    } else {
//        return mtwrite(mio->tp, buffer, count);
		return 0;
    }
}

int mioeof(MIO *mio)
{
    if (mio->type == MIO_DISK) {
        return feof(mio->fp);
    } else {
/*        if (mtpos(mio->tp) == SCSI_AT_EOM || 
            mtpos(mio->tp) == SCSI_AT_EOD) return TRUE;*/
        return TRUE;
    }
}

void mioclose(MIO *mio)
{
    if (mio->type == MIO_DISK) {
        fclose(mio->fp);
    } else {
		mtclose(mio->tp);
    }
}



/* Revision History
 *
 * $Log: mio.c,v $
 * Revision 1.2  2005/05/17 21:36:39  dechavez
 * 05-17 update
 *
 * Revision 1.1  2005/02/09 21:08:41  dechavez
 * initial (miow library) release
 *
 * Revision 1.1  2000/07/29 00:56:20  akimov
 * import ida/unix files
 *
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
