#pragma ident "$Id: dir.c,v 1.1 2006/12/12 22:41:11 dechavez Exp $"
/*======================================================================
 *
 *  Portable directory operations
 *
 *====================================================================*/
#include "util.h"

#ifdef HAVE_DIRENT

LNKLST *utilListDirectory(char *path)
{
DIR *dir;
struct dirent *dp;
struct stat sbuf;
LNKLST *list;

    if (path == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (!utilDirectoryExists(path)) {
        errno = ENOENT;
        return NULL;
    }

    if ((dir = opendir(path)) == NULL) return NULL;
    if ((list = listCreate()) == NULL) return NULL;

    for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
        if (!listAppend(list, dp->d_name, strlen(dp->d_name)+1)) {
            listDestroy(list);
            return NULL;
        }
    }
    closedir(dir);
    
    if (!listSetArrayView(list)) {
        listDestroy(list);
        return NULL;
    }

    return list;
}
# else

LNKLST *utilListDirectory(char *path)
{

#error "utilListDirectory not implemented for this platform"

}

#endif /* !HAVE_DIRENT */

#ifdef DEBUG_TEST

static void Process(char *path)
{
int i;
char *name;
LNKLST *list;

    if ((list = utilListDirectory(path)) == NULL) {
        printf("utilListDirectory: '%s': %s\n", path, strerror(errno));
        return;
    }

    printf("path='%s'\n", path);
    for (i = 0; i < list->count; i++) {
        name = (char *) list->array[i];
        printf("%s\n", name);
    }

    listDestroy(list);
}

int main(int argc, char **argv)
{
int i;

    if (argc == 1) {
        Process(".");
    } else {
        for (i = 1; i < argc; i++) Process(argv[i]);
    }
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: dir.c,v $
 * Revision 1.1  2006/12/12 22:41:11  dechavez
 * initial release
 *
 */
