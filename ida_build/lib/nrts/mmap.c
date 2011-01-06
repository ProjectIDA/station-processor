#pragma ident "$Id: mmap.c,v 1.4 2004/07/26 22:52:16 dechavez Exp $"
/*======================================================================
 *
 * Returns a pointer to a memory mapped nrts_sys structure.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#ifndef O_SYNC
#define O_SYNC 0
#endif

#ifndef _WIN32
int nrts_mmap(char *fname, char *perm, int code, struct nrts_mmap *maptr)
{
FILE *fp;
char *addr;
size_t len;
long diff;
off_t off;
int mode, prot, flag, fd;
char *map;
struct stat stat;
static char *fid = "nrts_mmap";

/* determine mapping length  */

    switch (code) {
      case NRTS_SYSTEM:
        len = (size_t) sizeof(struct nrts_sys);
        break;
      default:
        errno = EINVAL;
        return -1;
    }
        
/* open file for mapping file */

    util_lcase(perm);

    if (strcmp(perm, "r") == 0) {
        prot = PROT_READ;
        mode =  O_RDONLY |   O_NDELAY;
    } else {
        prot = PROT_READ | PROT_WRITE;
        mode =    O_RDWR |    O_CREAT | O_SYNC;
    }

    if ((fd = open(fname, mode, 0644)) < 0) {
        util_log(1, "%s: open(%s, 0x%x, 0644): %s",
            fid, fname, mode, syserrmsg(errno)
        );
        return -1;
    }

/* if we have write access make sure file is correct length */
/* (allow for possible increases in NRTS_MAXCHN)            */

    addr = (char *) 0;
    flag = (int)    MAP_FILE | MAP_SHARED;
    off  = (off_t)  0;

    if (fstat(fd, &stat) != 0) {
        util_log(1, "%s: fstat(fd, &stat): %s", fid, syserrmsg(errno));
        return -1;
    }
    diff = (long) len - (long) stat.st_size;

    if (diff != 0) {
        if (diff < 0) {
            util_log(1, "ERROR: shrinking sys files are not tolerated");
            return -1;
        }
        if (diff % sizeof(struct nrts_chn) == 0) {
            util_log(1, "NRTS_MAXCHN evidently changed from %d to %d",
                NRTS_MAXCHN - diff / sizeof(struct nrts_chn), NRTS_MAXCHN
            );
            if (prot | PROT_WRITE) {
                util_log(1, "adjusting sys file to match new NRTS_MAXCHN");
                if (lseek(fd, len-1, SEEK_SET) == -1) {
                    util_log(1, "%s: lseek(fd, %ld, SEEK_SET): %s",
                        fid, len-1, syserrmsg(errno)
                    );
                    return -1;
                }
                if (write(fd, "", 1) != 1) {
                    util_log(1, "%s: write(fd, <nothing>, 1): %s",
                        fid, syserrmsg(errno)
                    );
                    return -1;
                }
            }
        }
    }

/* do the mapping */

    if ((map = mmap(addr,len,prot,flag,fd,off)) == (char *) -1) {
        util_log(1, "%s: mmap(addr,%ld,%d,%d,fd,%ld): %s",
                 fid, len, prot, flag, off, syserrmsg(errno));
        return -1;
    }

    maptr->fd  = fd;
    maptr->ptr = (void *) map;

    return 0;
}

void nrts_unmap(struct nrts_mmap *maptr)
{
    if (maptr->fd > 0) close(maptr->fd);
    maptr->fd = 0;
}
#else
int nrts_mmap(char *fname, char *perm, int code, struct nrts_mmap *maptr)
    {
    size_t len;
    LPVOID lpShMem = NULL;
    HANDLE hFileHandle = NULL, hMapHandle = NULL;
    DWORD prot, mode, mapmode;

    switch (code) {
      case NRTS_SYSTEM:
        len = (size_t) sizeof(struct nrts_sys);
        break;
      default:
        errno = EINVAL;
        return -1;
    }


    if (strcmp(perm, "r") == 0) {
        prot    = PAGE_READONLY;
        mode    = GENERIC_READ;
        mapmode = FILE_MAP_READ;
    } else {
        prot    = PAGE_READWRITE;
        mode    = GENERIC_READ|GENERIC_WRITE;
        mapmode = FILE_MAP_ALL_ACCESS;
    }

    if (INVALID_HANDLE_VALUE != (hFileHandle = CreateFile(fname, mode, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
        {
        if ((hMapHandle = CreateFileMapping(hFileHandle, NULL, prot, 0,0, NULL)) != NULL)
            {
            lpShMem = MapViewOfFile(hMapHandle, mapmode, 0, 0, 0);
            }
        }
        else 
        {
        hFileHandle = NULL;
        hMapHandle  = NULL;
        return -1;
        }

    maptr->fd = hFileHandle;
    maptr->hMapHandle  = hMapHandle;
    maptr->ptr         = lpShMem;

    return 0;
    }
void nrts_unmap(struct nrts_mmap *maptr)
    {
    if(maptr->ptr!=NULL)UnmapViewOfFile(maptr->ptr);
    if( maptr->fd != NULL )  CloseHandle(maptr->fd);
    if( maptr->hMapHandle  != NULL )  CloseHandle(maptr->hMapHandle);
    }
#endif

/* Revision History
 *
 * $Log: mmap.c,v $
 * Revision 1.4  2004/07/26 22:52:16  dechavez
 * change win32 port to create files with read/write access
 *
 * Revision 1.3  2004/06/24 17:37:40  dechavez
 * removed unnecessary includes, WIN32 port (aap)
 *
 * Revision 1.2  2003/10/16 17:22:54  dechavez
 * ansi style declaration
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
