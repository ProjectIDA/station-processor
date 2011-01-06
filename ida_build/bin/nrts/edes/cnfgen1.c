#pragma ident "$Id: cnfgen1.c,v 1.2 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Load an XFER_CNFGEN1 format structure
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "util.h"
#include "xfer.h"
#include "edes.h"

#define SYS  (edes->sys + i)
#define STA  (edes->sys[i].sta + j)
#define CHN  (STA->chn + k)
#define CPTR (cnf->sta[cnf->nsta].chn + k)
#define EPTR (edes->cnf[i].sta[j].chn + k)

int load_cnfgen1(struct xfer_cnfgen1 *cnf, struct edes_params *edes)
{
int i, j, k;

    cnf->order = util_order();
    cnf->nsta  = 0;
    for (i = 0; i < edes->nsys; i++) {
        for (j = 0; j < SYS->nsta; j++) {

            if (cnf->nsta == XFER_MAXSTA) {
                xfer_errno = XFER_ELIMIT;
                return -101;
            } else {
                edes->cnf[i].sta[j].index = cnf->nsta;
            }

            cnf->sta[cnf->nsta].nchn = STA->nchn;

            if (cnf->sta[cnf->nsta].nchn > XFER_MAXCHN) {
                xfer_errno = XFER_ELIMIT;
                return -102;
            }

            strcpy(cnf->sta[cnf->nsta].name, STA->name);
            cnf->sta[cnf->nsta].lat   = edes->cnf[i].sta[j].lat;
            cnf->sta[cnf->nsta].lon   = edes->cnf[i].sta[j].lon;
            cnf->sta[cnf->nsta].elev  = edes->cnf[i].sta[j].elev;
            cnf->sta[cnf->nsta].depth = edes->cnf[i].sta[j].depth;

            for (k = 0; k < STA->nchn; k++) {
                EPTR->index = k;
                strcpy(CPTR->name, CHN->name);
                strcpy(CPTR->instype, EPTR->instype);
                CPTR->wrdsiz  = EPTR->wrdsiz;
                CPTR->order   = EPTR->order;
                CPTR->sint    = EPTR->sint;
                CPTR->calib   = EPTR->calib;
                CPTR->calper  = EPTR->calper;
                CPTR->vang    = EPTR->vang;
                CPTR->hang    = EPTR->hang;
                CPTR->beg     = CHN->beg;
                CPTR->end     = CHN->end;
            }
            ++cnf->nsta;
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: cnfgen1.c,v $
 * Revision 1.2  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
