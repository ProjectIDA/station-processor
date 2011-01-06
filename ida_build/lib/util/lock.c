#pragma ident "$Id: lock.c,v 1.7 2009/03/17 18:15:40 dechavez Exp $"
/*======================================================================
 *
 *  Record locking utilities.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "util.h"

#ifndef WIN32
int util_lock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
struct flock lock;

    lock.l_type   = type;
    lock.l_start  = offset;
    lock.l_whence = whence;
    lock.l_len    = len;

    return fcntl(fd, cmd, &lock);
}

BOOL utilWriteLockWait(FILE *fp)
{
struct flock lock;

    if (fp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    lock.l_type   = F_WRLCK;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    while (fcntl(fileno(fp), F_SETLKW, &lock) == -1) if (errno != EINTR) return FALSE;

    return TRUE;
}

BOOL utilWriteLockTry(FILE *fp)
{
struct flock lock;

    if (fp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    lock.l_type   = F_WRLCK;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    if (fcntl(fileno(fp), F_SETLK, &lock) == -1) return FALSE;

    return TRUE;
}

BOOL utilReadLockWait(FILE *fp)
{
struct flock lock;

    if (fp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    lock.l_type   = F_RDLCK;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    while (fcntl(fileno(fp), F_SETLKW, &lock) == -1) if (errno != EINTR) return FALSE;

    return TRUE;
}

BOOL utilReadLockTry(FILE *fp)
{
struct flock lock;

    if (fp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    lock.l_type   = F_RDLCK;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    if (fcntl(fileno(fp), F_SETLK, &lock) == -1) return FALSE;

    return TRUE;
}

void utilReleaseLock(FILE *fp)
{
struct flock lock;

    if (fp == NULL) return;

    lock.l_type   = F_UNLCK;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    fcntl(fileno(fp), F_SETLK, &lock);

}
#else

int util_lock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    return 0;
}

BOOL utilWriteLockWait(FILE *fp) {
    return TRUE;
}

BOOL utilWriteLockTry(FILE *fp) {
    return TRUE;
}

BOOL utilReadLockWait(FILE *fp) {
    return TRUE;
}

BOOL utilReadLockTry(FILE *fp) {
    return TRUE;
}

void utilReleaseLock(FILE *fp) {
    return;
}

#endif

/* Revision History
 *
 * $Log: lock.c,v $
 * Revision 1.7  2009/03/17 18:15:40  dechavez
 * added utilWriteLockWait(), utilWriteLockTry(), utilReadLockWait(), utilReadLockTry(),
 * utilReleaseLock(), removed utilLockFileWait() and utilUnlockFile()
 *
 * Revision 1.6  2008/03/14 06:17:16  dechavez
 * added utilLockFileWait() and utilUnlockFile()
 *
 * Revision 1.5  2006/06/26 23:48:26  dechavez
 * changed win32 dummy args
 *
 * Revision 1.4  2006/06/26 22:20:42  dechavez
 * made separate unix and windows (dummy) functions
 *
 * Revision 1.3  2004/06/24 17:07:57  dechavez
 * WIN32 "port" (aap)
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
