#pragma ident "$Id: query.c,v 1.4 2005/05/25 00:31:43 dechavez Exp $"
/*======================================================================
 *
 *  Ask a question, require a 'y' or 'n' answer
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

int utilQuery(char *string)
{
#define YES 1
#define NO  0
#define UNDEFINED -1

char    ans;
int    value;

    while (1) {
    
      value = UNDEFINED;
      fprintf(stderr,"%s",string);
      fflush(stderr);
      do { ans = getchar(); } while ((ans < 041)||(ans > 0176));
      if (ans == 'y' || ans == 'Y') value = YES;
      if (ans == 'n' || ans == 'N') value = NO;
      do { ans = getchar(); } while (ans != 012);
      if (value != UNDEFINED) return value;
      fprintf(stderr,"\07Please answer 'y' or 'n'.\n");

    }
}

char utilPause(BOOL prompt)
{
char key;

    if (prompt) {
        fprintf(stderr, "Press any key to continue.");
        fflush(stderr);
    }
    key = getchar();
    return key;
}

/* Revision History
 *
 * $Log: query.c,v $
 * Revision 1.4  2005/05/25 00:31:43  dechavez
 * utilPause() to return char that was pressed
 *
 * Revision 1.3  2003/06/09 23:57:54  dechavez
 * switch to newConvention for function naming
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
