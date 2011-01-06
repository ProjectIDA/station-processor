#pragma ident "$Id: filetimes.c,v 1.1 2005/08/26 20:19:43 dechavez Exp $"
/*======================================================================
 *
 *  Print file time stamps
 *
 *====================================================================*/
#include "platform.h"
#define PATHNAME argv[1]

int main(int argc, char **argv)
{
struct stat buf;
struct tm *mtime;
char tmp[256];

    if (argc != 2) {
        fprintf(stderr, "usage: %s file_name\n", argv[0]);
        exit(1);
    }

    if (stat(PATHNAME, &buf) != 0) {
        perror(PATHNAME);
        exit(1);
    }

	printf("%s ", utilLttostr(buf.st_atime, 0, tmp));
	printf("%s ", utilLttostr(buf.st_mtime, 0, tmp));
	printf("%s\n", utilLttostr(buf.st_ctime, 0, tmp));
    printf("%ld %ld %ld\n", buf.st_atime, buf.st_mtime, buf.st_ctime);
    exit(0);

}

/* Revision History
 *
 * $Log: filetimes.c,v $
 * Revision 1.1  2005/08/26 20:19:43  dechavez
 * initial release
 *
 */
