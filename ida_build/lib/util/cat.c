#pragma ident "$Id: cat.c,v 1.1.1.1 2000/02/08 20:20:41 dec Exp $"
/*======================================================================
 *
 *  Concatenate two files together.
 *
 *  util_cat(src1, src2, dest)
 *
 *  `src1', `src2', and  `dest' can be unique, in which case `dest'
 *  will be created and will contain `src1' + `src2'.
 *
 *  `src2' and `dest' can be the same, in which case `src1' will be
 *  appended to `src2'.  A NULL `dest' is assumed to be `src2'.
 *
 *  `src1' and `dest' can be the same, in which case `src2' will be
 *  appended to `src1'.
 *
 *  `src1' and `src2' can be the same.
 *
 *  No other combinations of `src1', `src2', and `dest' are allowed.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "util.h"

#define BUFLEN 1024

static int _util_cat_copy(fp1, fp2)
FILE *fp1;
FILE *fp2;
{
int nbytes;
char buffer[BUFLEN];

    while ((nbytes = fread(buffer, sizeof(char), BUFLEN, fp1)) > 0) {
        if (fwrite(buffer, sizeof(char), nbytes, fp2) != nbytes) return -11;
    }

    return feof(fp1) ? 0 : -12;

}

int util_cat(src1, src2, dest)
char *src1;
char *src2;
char *dest;
{
int status;
FILE *fp1, *fp2, *fp3;

    if (dest == NULL) dest = src2;

    if (src1 == NULL || src2 == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (strcmp(src1, dest) == 0) {
        if ((fp1 = fopen(src1, "a")) == NULL) return -1;
        if ((fp2 = fopen(src2, "rb")) == NULL) return -1;
        status = _util_cat_copy(fp2, fp1);
        fclose(fp1);
        fclose(fp2);
    } else {
        if ((fp1 = fopen(src1, "rb")) == NULL) return -1;
        if (strcmp(src2, dest) == 0) {
            if ((fp2 = fopen(src2, "ab")) == NULL) return -1;
            status = _util_cat_copy(fp1, fp2);
            fclose(fp1);
            fclose(fp2);
        } else {
            if ((fp2 = fopen(src2, "rb")) == NULL) return -1;
            if ((fp3 = fopen(dest, "wb")) == NULL) return -1;
            status = _util_cat_copy(fp1, fp3);
            if (status == 0) status = _util_cat_copy(fp2, fp3);
            fclose(fp1);
            fclose(fp2);
            fclose(fp3);
        }
    }

    return status;
}

#ifdef DEBUG_TEST

main(argc, argv)
int argc;
char *argv[];
{
int status;
char *src1, *src2, *dest;

    if (argc == 3) {
        src1 = argv[1];
        src2 = argv[2];
        dest = NULL;
        printf("src1 = `%s', src2 = `%s', dest = NULL\n", src1, src2);
    } else if (argc == 4) {
        src1 = argv[1];
        src2 = argv[2];
        dest = argv[3];
        printf("src1 = `%s', src2 = `%s', dest = `%s'\n", src1, src2, dest);
    } else {
        fprintf(stderr, "usage: %s src1 src2 [dest]\n", argv[0]);
        exit(1);
    }

    status = util_cat(src1, src2, dest);
    printf("util_cat() returns %d\n", status);

    exit(status);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: cat.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
