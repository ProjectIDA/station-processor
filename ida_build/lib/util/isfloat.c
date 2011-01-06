#pragma ident "$Id: isfloat.c,v 1.3 2003/12/10 06:07:43 dechavez Exp $"
/*======================================================================
 *
 *  Test to see if a string represents a legal floating point number.
 *  Integer forms are considered OK.
 *
 *====================================================================*/
#include <strings.h>
#ifndef HAVE_STRINGTODECIMAL
#    if defined SUNOS || defined SOLARIS
#        define HAVE_STRINGTODECIMAL
#    endif
#endif

#ifdef HAVE_STRINGTODECIMAL

#include <stdio.h>
#include <floatingpoint.h>

int util_isfloat(char *string, char *temp)
{
char *pc;
int i, nmax;
int have_dot = 0;
int have_exp = 0;
int all_zero = 1;
decimal_record pd;
enum decimal_string_form pform;
char *pechar;

    if (string == NULL || strlen(string) <= 0) return 0;

    strcpy(temp, string);
    for (i = 0; i < strlen(temp); i++) if (temp[i] != '0') all_zero = 0;
    if (all_zero) return 1;
    for (i = 0; i < strlen(temp); i++) if (temp[i] == '.') have_dot = 1;
    for (i = 0; i < strlen(temp); i++) if (temp[i] == 'e') have_exp = 1;
    for (i = 0; i < strlen(temp); i++) if (temp[i] == 'E') have_exp = 1;
    for (i = 0; i < strlen(temp); i++) if (temp[i] == 'd') have_exp = 1;
    for (i = 0; i < strlen(temp); i++) if (temp[i] == 'D') have_exp = 1;
    if (have_dot && !have_exp) {
        i = strlen(temp) - 1;
        while (i >= 0 && temp[i] != '.' && temp[i] == '0') temp[i--] = 0;
    }
    pc = temp;

    nmax = strlen(pc);
    string_to_decimal(&pc, nmax, 1, &pd, &pform, &pechar);

    if (
        strlen(pc) != 0          ||
        pform == invalid_form    ||
        pform == whitespace_form ||
        pform == inf_form        ||
        pform == infinity_form   ||
        pform == nan_form        ||
        pform == nanstring_form
    ) return 0;

    return 1;
}

#endif

/* Revision History
 *
 * $Log: isfloat.c,v $
 * Revision 1.3  2003/12/10 06:07:43  dechavez
 * added includes to calm solaris cc
 *
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
