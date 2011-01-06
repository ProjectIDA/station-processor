#pragma ident "$Id: help.c,v 1.4 2006/03/24 18:38:44 dechavez Exp $"
/*======================================================================
 *
 *  Print usage summary.
 *
 *====================================================================*/
#include <stdio.h>
#include "idalst.h"

extern char *VersionIdentString;

void help(struct defaults *def, FILE *fp)
{
    fprintf(fp,"idalst %s\n", VersionIdentString);
    fprintf(fp,"\n");
    fprintf(fp,"usage: idalst rev=code [sta=name map=name options]\n");
    fprintf(fp,"\n");
    fprintf(fp,"rev=code => data revision code (REQUIRED)\n");
    fprintf(fp,"\n");
    fprintf(fp,"Print options (preceeded with +/- to enable/disable):\n");
    fprintf(fp,"pc     => print calibration records\n");
    fprintf(fp,"pd     => print data records\n");
    fprintf(fp,"pe     => print event records\n");
    fprintf(fp,"pi     => print identification (header) records\n");
    fprintf(fp,"pk     => print configuration records\n");
    fprintf(fp,"ps     => print DAS status records\n");
    fprintf(fp,"pl[=n] => print log records [of level <= n]\n");
    fprintf(fp,"pdcc   => print records added at DCC\n");
    fprintf(fp,"pdet   => print event detector parameters\n");
    fprintf(fp,"ptt    => print time triples\n");
    fprintf(fp,"pdd    => print duplicate data record headers\n");
    fprintf(fp,"pmap   => print mapping of data logger stream codes\n");
    fprintf(fp,"pum    => print unmapped data record information\n");
    fprintf(fp,"pcr    => print corrupt record status information\n");
    fprintf(fp,"ptq    => print all time quality changes (if +ctq)\n");
    fprintf(fp,"pbt    => print illegal time tags (if +ctag)\n");
    fprintf(fp,"psn    => print packet seq nos (if +pd and applicable)\n");
    fprintf(fp,"pall   => all of the above\n");
    fprintf(fp,"tag    => tag output for sorting\n");
    fprintf(fp,"v      => verbose stderr commentary\n");
    fprintf(fp,"\n");
    fprintf(fp,"Check options (preceeded with +/- to enable/disable):\n");
    fprintf(fp,"ctag => check time tag structure for internal errors\n");
    fprintf(fp,"ci   => time tag increment checks (internal & external)\n");
    fprintf(fp,"coff => compare beg/end ext-hz times within records\n");
    fprintf(fp,"cdr  => check for duplicate adjacent records\n");
    fprintf(fp,"clw  => decompression (last word) checks\n");
    fprintf(fp,"cns  => verify number of samples is valid\n");
    fprintf(fp,"ctq  => track time code quality transitions in data\n");
    fprintf(fp,"ctg  => check for discontinuities in true time stamp\n");
    fprintf(fp,"call => all of the above\n");
    fprintf(fp,"\n");
    fprintf(fp,"Other options:\n");
    fprintf(fp,"if=input    => read from file/device 'input'\n");
    fprintf(fp,"bs=size     => set read block size\n");
    fprintf(fp,"map=name    => override default channel map with 'name'\n");
    fprintf(fp,"sort=rec    => sort tinc error output by record number\n");
    fprintf(fp,"sort=stream =>   \"    \"    \"      \"    \" stream\n");
    fprintf(fp,"grp=sign    => group  \"    \"      \"    \" sign\n");
    fprintf(fp,"grp=size    => group  \"    \"      \"    \" size\n");
    fprintf(fp,"grp=none    => don't group tinc error output\n");
    fprintf(fp,"keep=list   => list of streams (internal id) to process\n");
    fprintf(fp,"reject=list => list of streams (internal id) to reject\n");
    fprintf(fp,"+/-data     => input Data records have/lack data portion\n");
    fprintf(fp,"\n");
    fprintf(fp,"default: ");
    fprintf(fp,"if=%s +data bs=%ld ",  def->input, def->bs);
    fprintf(fp,"keep=all ");
    fprintf(fp,"sort=");
    switch(def->sort) {
        case BY_RECORD: fprintf(fp,"rec ");     break;
        case BY_STREAM: fprintf(fp,"stream ");  break;
        default:        fprintf(fp,"HELP_ERR"); break;
    }
    fprintf(fp,"group=");
    switch(def->group) {
        case BY_SIZE: fprintf(fp,"sign ");     break;
        case BY_SIGN: fprintf(fp,"size ");  break;
        default:      fprintf(fp,"HELP_ERR"); break;
    }
    fprintf(fp,"\n");
    fprintf(fp,"print: ");
    fprintf(fp,"%cpc ",   (def->print & P_CALIB   ) ? '+' : '-');
    fprintf(fp,"%cpd ",   (def->print & P_DATA    ) ? '+' : '-');
    fprintf(fp,"%cps ",   (def->print & P_SREC    ) ? '+' : '-');
    fprintf(fp,"%cpe ",   (def->print & P_EVENT   ) ? '+' : '-');
    fprintf(fp,"%cpi ",   (def->print & P_IDENT   ) ? '+' : '-');
    fprintf(fp,"%cpk ",   (def->print & P_CONFIG  ) ? '+' : '-');
    fprintf(fp,"%cpl",    (def->print & P_LOG     ) ? '+' : '-');
    if (def->print & P_LOG) {
        fprintf(fp, "=%d ", def->loglevel);
    } else {
        fprintf(fp, " ");
    }
    fprintf(fp,"%cpdcc ", (def->print & P_DCCREC  ) ? '+' : '-');
    fprintf(fp,"%cpdet ", (def->print & P_DETECT  ) ? '+' : '-');
    fprintf(fp,"%cptt ",  (def->print & P_TTRIP   ) ? '+' : '-');
    fprintf(fp,"%cpdd ",  (def->print & P_DUPDAT  ) ? '+' : '-');
    fprintf(fp,"%cpmap ", (def->print & P_STRMAP  ) ? '+' : '-');
    fprintf(fp,"%cpum ",  (def->print & P_DUNMAP  ) ? '+' : '-');
    fprintf(fp,"%cpbt ",  (def->print & P_BADTAG  ) ? '+' : '-');
    fprintf(fp,"%cpcr ",  (def->print & P_CORRUPT ) ? '+' : '-');
    fprintf(fp,"%ctag ",  (def->print & P_TAG     ) ? '+' : '-');
    fprintf(fp,"%cv\n",   (def->verbose           ) ? '+' : '-');
    fprintf(fp,"check: ");
    fprintf(fp,"%cctag ", (def->check & C_BADTAG) ? '+' : '-');
    fprintf(fp,"%cci ",   (def->check & C_TINC  ) ? '+' : '-');
    fprintf(fp,"%ccoff ", (def->check & C_TOFF  ) ? '+' : '-');
    fprintf(fp,"%ccdr ",  (def->check & C_SAME  ) ? '+' : '-');
    fprintf(fp,"%cclw ",  (def->check & C_LASTW ) ? '+' : '-');
    fprintf(fp,"%ccns ",  (def->check & C_NSAMP ) ? '+' : '-');
    fprintf(fp,"%cctq\n", (def->check & C_TQUAL ) ? '+' : '-');
    exit(1);
}

/* Revision History
 *
 * $Log: help.c,v $
 * Revision 1.4  2006/03/24 18:38:44  dechavez
 * true time tag test support added (akimov)
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2000/02/18 00:51:32  dec
 * #define and print VERSION
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
