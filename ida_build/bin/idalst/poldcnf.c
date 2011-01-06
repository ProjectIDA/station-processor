#pragma ident "$Id: poldcnf.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Print the configuration table.
 *
 *====================================================================*/
#include "idalst.h"

void poldcnf(IDA_OLD_HEADER *hrec, int print, long *start_tag)
{
long tagno;
int i, j, num_rows, num_cols;
short *table;
static char *rev4_label[] = { 
"Stream", "Chan #", "Filt #", "Cont  ", "Trig  ",
"Comp  ", "BB_det", "SP_det", "Modem " 
};

    tagno = *start_tag;
    table = hrec->config.table;
    num_rows = hrec->config.nrows;
    num_cols = hrec->config.ncols;

/*  Print the table header */

    tag(tagno++); printf( "\n"); tag(tagno++);
    if (ida->rev.value == 1) {
        printf("                          Configuration Table\n");
        tag(tagno++);
        printf("  Chan #  noskip  direc  delay  predec decim");
        printf("  BBfil  LPfil DETect recDET\n");
    } else if (ida->rev.value == 3) {
        printf("                           Configuration Table\n");
        tag(tagno++);
        printf("Ch  direc cp_dir  delay    res    res  BBfil ");
        printf("cp_BBf  LPfil cp_LPf DETect recDET\n");
    } else if (ida->rev.value == 4) {
        printf("                          C O N F I G U R A T I O N");
        printf("   T A B L E\n");
    }

/*  Print the table contents  */

    if (ida->rev.value == 1) {
        for (i = 0; i < num_rows; i++) {
            tag(tagno++);
            for (j = 0; j < num_cols; j++) {
                printf("%7d",*(table+(i*num_cols)+j));
            }
            printf("\n");
        }

    } else if (ida->rev.value == 3) {
        for (i = 0; i < num_rows; i++) {
            tag(tagno++);
            for (j = 0; j < num_cols; j++) {
                if (j == 0) {
                    printf("%2d",*(table+(i*num_cols)+j));
                } else {
                    printf("%7d",*(table+(i*num_cols)+j));
                }
            }
            printf("\n");
        }

    } else if (ida->rev.value == 4) {
        for (j = 0; j < num_cols; j++) {
            tag(tagno++);
            printf("%s",rev4_label[j]);
            printf( "%2hd", *(table+(0*num_cols)+j));
            for (i = 1; i < num_rows; i++) {
                printf( "%3hd", *(table+(i*num_cols)+j));
            }
            printf("\n");
        }
    }
    
    *start_tag = tagno;
}

/* Revision History
 *
 * $Log: poldcnf.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
