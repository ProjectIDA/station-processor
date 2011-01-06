#pragma ident "$Id: load_config.c,v 1.2 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Load the current configuration into the appropriate output location
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "nrts.h"
#include "xfer.h"
#include "edes.h"

int load_config(struct edes_req *request,  struct xfer_cnf *cnf, struct edes_params *edes)
{
struct xfer_cnfgen1 *gen1;
struct xfer_cnfnrts *nrts;

    switch (cnf->format = request->cnf_form) {
      case XFER_CNFGEN1:
        return load_cnfgen1(&cnf->type.gen1, edes);
        break;
      case XFER_CNFNRTS:
        return load_cnfnrts(&cnf->type.nrts, edes);
        break;
      default:
        xfer_errno = XFER_EFORMAT;
        return -1;
    }
}

/* Revision History
 *
 * $Log: load_config.c,v $
 * Revision 1.2  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
