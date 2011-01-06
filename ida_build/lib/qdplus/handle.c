#pragma ident "$Id: handle.c,v 1.9 2008/12/15 23:26:03 dechavez Exp $"
/*======================================================================
 *
 * QDPLUS packet handle
 *
 *====================================================================*/
#include "qdplus.h"

/* Destroy a handle and all the dynamic stuff it contains */

static void DestroyHandlePar(QDPLUS *handle)
{
    if (handle->par.path.meta != NULL) {
        free(handle->par.path.meta);
        handle->par.path.meta = NULL;
    }
    if (handle->par.path.state != NULL) {
        free(handle->par.path.state);
        handle->par.path.state = NULL;
    }
    if (handle->par.site != NULL) {
        free(handle->par.site);
        handle->par.site = NULL;
    }
}

void qdplusDestroyHandle(QDPLUS *handle)
{
QDPLUS_DIGITIZER *digitizer;
QDPLUS_META_DATA *meta;
LNKLST_NODE *crnt;

    if (handle == NULL) return;
    
    DestroyHandlePar(handle);
    qdplusDestroyDigitizerList(handle->list.digitizer);
    qdplusDestroyMetaDataList(handle->list.meta);

    free(handle);

}

/* Create and initialize a new handle */

static BOOL InitHandlePar(QDPLUS *handle, QDPLUS_PAR *par)
{
static QDPLUS_PAR defpar = QDPLUS_DEFAULT_PAR;

    handle->par = defpar;
    if (par == NULL) return TRUE;
    handle->par = *par;
    handle->par.path.meta = handle->par.path.state = handle->par.site = NULL;

    if (par->path.meta != NULL && (handle->par.path.meta = strdup(par->path.meta)) == NULL) return FALSE;
    if (par->path.state != NULL && (handle->par.path.state = strdup(par->path.state)) == NULL) return FALSE;
    if (par->site != NULL && (handle->par.site = strdup(par->site)) == NULL) return FALSE;

    return TRUE;
}

QDPLUS *qdplusCreateHandle(QDPLUS_PAR *par)
{
QDPLUS *handle;
static QDPLUS_PAR defpar = QDPLUS_DEFAULT_PAR;

    if (par == NULL) par = &defpar;

    if ((handle = (QDPLUS *) malloc(sizeof(QDPLUS))) == NULL) return NULL;
    handle->list.digitizer = handle->list.meta = NULL;

    if (!InitHandlePar(handle, par)) {
        qdplusDestroyHandle(handle);
        return NULL;
    }

    if ((handle->list.digitizer = listCreate()) == NULL) {
        qdplusDestroyHandle(handle);
        return NULL;
    }

/* Load metadata, if any */

    if (utilDirectoryExists(handle->par.path.meta)) {
        if ((handle->list.meta = qdplusReadMetaData(handle->par.path.meta)) == NULL) {
            qdplusDestroyHandle(handle);
            return NULL;
        }
    }

/* Load state data, if any */

    if (utilFileExists(handle->par.path.state)) {
        handle->state = qdplusReadStateFile(handle);
    } else {
        handle->state = QDPLUS_STATE_EMPTY;
    }

    return handle;
}

/* Set the LCQ state for all digitizers in the handle */

void qdplusSetLcqStateFlag(QDPLUS *handle, int value)
{
LNKLST_NODE *crnt;
QDPLUS_DIGITIZER *digitizer;

    if (handle == NULL) return;

    crnt = listFirstNode(handle->list.digitizer);
    while (crnt != NULL) {
        digitizer = (QDPLUS_DIGITIZER *) crnt->payload;
        digitizer->lcq.state.flag = value;
        crnt = listNextNode(crnt);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: handle.c,v $
 * Revision 1.9  2008/12/15 23:26:03  dechavez
 * set handle->state in qdplusCreateHandle()
 *
 * Revision 1.8  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
