#pragma ident "$Id: cfgpair.c,v 1.2 2001/05/07 22:40:12 dec Exp $"
/*======================================================================
 *
 *  Get an (identifer, value) pair from a configuration file.
 *
 *  Return values: 0 => success
 *                 1 => EOF
 *                -1 => error
 *
 *  In the case of read errors, errno is set by the system.  If the
 *  configuration file is improperly formatted (ie, has something other
 *  than an (identifer, value) pair or comments then errno is set to
 *  EINVAL.
 *
 *  By passing this routine the address of an int counter (lineno) the
 *  calling routine can know the line number of the entry which caused
 *  the error.
 *
 *  The output strings identifer and value point to static memory
 *  in this routine.  It is the responsibility of the calling process
 *  to copy the contents elsewhere in order to avoid overwriting them
 *  on subsequent calls to util_cfgpair().
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "util.h"

#define COMMENT      '#'
#define QUOTE        '"'
#define DELIMITERS   "= \t\"" /* Be sure to include QUOTE */
#define MAXTOKENS    3 /* more than 2 so we can catch errors */
#define LOCAL_BUFLEN 1024

/*FILE *fp           => open configuration file            */
/*int  *lineno       => line number of line in buffer      */
/*char **identifier  => output identifier string           */
/*char **value       => output value associated with above */

int util_cfgpair(FILE *fp, int *lineno, char **identifier, char **value)
{
static char buffer[LOCAL_BUFLEN];
static char *token[MAXTOKENS];
int status, ntoken;

    status = util_getline(fp, buffer, LOCAL_BUFLEN, COMMENT, lineno);
    if (status != 0) return status;

    ntoken = util_parse(buffer, token, DELIMITERS, MAXTOKENS, QUOTE);
    if (ntoken != 2) {
        errno = EINVAL;
        return -1;
    }
    
    *identifier = token[0];
    *value      = token[1];

    return 0;
}

#ifdef DEBUGTEST

main(argc, argv)
int argc;
char *argv[];
{
int i, status, lineno = 0;
char *identifier, *value;
FILE *fp;

    for (i = 1; i < argc; i++) {
        if ((fp = fopen(argv[i], "r")) == NULL) {
            perror(argv[i]);
        } else {
            printf("\nParsing %s:\n", argv[i]);
            do {
                status = util_cfgpair(fp, &lineno, &identifier, &value);
                if (status == 0) {
                    printf("line %d gives %s = %s\n", lineno, identifier, value);
                } else if (status == 1) {
                    printf("%d lines processed\n", lineno);
                } else {
                    if (errno == EINVAL) {
                        printf("WARNING: syntax error at line %d (ignored)\n", lineno);
                        status = 0;
                    } else {
                        fprintf(stderr, "error at line %d of file ", lineno);
                        perror(argv[i]);
                    }
                }
            } while (status == 0);
        }
    }

    exit(0);
}

#endif /* DEBUGTEST */

/* Revision History
 *
 * $Log: cfgpair.c,v $
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
