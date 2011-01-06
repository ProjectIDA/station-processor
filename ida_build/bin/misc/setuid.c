#pragma ident "$Id: setuid.c,v 1.1.1.1 2000/02/08 20:20:09 dec Exp $"
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

main(argc, argv)
int argc;
char *argv[];
{
struct passwd *passwd;
char *name;
char *path;
uid_t owner;
uid_t group;

    if (argc == 3) {
        name = argv[1];
        path = argv[2];
    } else {
        fprintf(stderr, "Usage: %s owner path\n", argv[0]);
        exit(1);
    }

    if ((passwd = getpwnam(name)) == (struct passwd *) NULL) {
        perror(name);
        exit(1);
    }

    owner = passwd->pw_uid;
    group = passwd->pw_gid;

    if (chown(path, owner, group) != 0) {
        perror(path);
        exit(errno == ENOENT ? 0 : 1);
    }

    if (chmod(path, 04775) != 0) {
        perror(path);
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: setuid.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
