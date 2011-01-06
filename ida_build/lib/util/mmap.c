#pragma ident "$Id: mmap.c,v 1.3 2005/09/13 00:32:22 dechavez Exp $"
/*======================================================================
 *
 * Portable mapping between process address space and a file
 *
 *====================================================================*/
#include "util.h"

#ifdef unix

BOOL utilMmap(char *path, off_t offset, size_t len, UTIL_MMAP *map)
{
int prot = PROT_READ | PROT_WRITE;
int mode = O_RDWR | O_CREAT | O_SYNC;
int flags = MAP_SHARED;
int perms = 0644;

    if (path == NULL || offset < 0 || len <= 0 || map == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if ((map->fd = open(path, mode, perms)) < 0) return FALSE;
    if ((map->ptr = mmap(0, len, prot, flags, map->fd, offset)) == MAP_FAILED) return FALSE;
    map->fd = 0;
    map->len = len;

    return TRUE;
}

VOID utilUnmap(UTIL_MMAP *map)
{
    if (map == NULL) return;
    if (map->ptr != NULL) munmap(map->ptr, map->len); map->ptr = NULL;
    close(map->fd);
}

#else

BOOL utilMmap(char *path, off_t offset, size_t len, UTIL_MMAP *map)
{
LPVOID lpShMem = NULL;
HANDLE hMapHandle = NULL;
static DWORD prot    = PAGE_READWRITE;
static DWORD mode    = GENERIC_READ|GENERIC_WRITE;
static DWORD mapmode = FILE_MAP_ALL_ACCESS;

    if (path == NULL || offset < 0 || len <= 0 || map == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    map->fd = CreateFile(
        path,
        mode,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (map->fd == INVALID_HANDLE_VALUE) return FALSE;

    map->hMapHandle = CreateFileMapping(map->fd, NULL, prot, 0, 0, NULL);
    if (map->hMapHandle == NULL) return FALSE;

    map->ptr = MapViewOfFile(map->hMapHandle, mapmode, 0, 0, 0);

    map->len = len;
    return TRUE;
}

VOID utilUnmap(UTIL_MMAP *map)
{
    if (map == NULL) return;

    if (map->ptr != NULL) UnmapViewOfFile(map->ptr); map->ptr = NULL;
    if (map->fd != NULL) CloseHandle(map->fd); map->fd = NULL;
    if (map->hMapHandle != NULL) CloseHandle(map->hMapHandle); map->hMapHandle = NULL;

}

#endif /* !unix */

/* Revision History
 *
 * $Log: mmap.c,v $
 * Revision 1.3  2005/09/13 00:32:22  dechavez
 * removed MAP_ALIGN option
 *
 * Revision 1.2  2005/05/25 00:32:56  dechavez
 * initial production release
 *
 * Revision 1.1  2005/05/13 18:53:13  dechavez
 * checkpoint build prior to 05-11 akimov update
 *
 */
