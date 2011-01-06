#pragma ident "$Id: q330.c,v 1.12 2007/10/31 17:36:02 dechavez Exp $"
/*======================================================================
 *
 * Service a Quanterra Q330
 *
 *====================================================================*/
#include "isidl.h"
#include "qdp.h"

#define MY_MOD_ID ISIDL_MOD_Q330

/* called by qdp library each time it gets a packet for the app */

void SaveQ330Packet(void *args, QDP_PKT *pkt)
{
Q330 *q330;
static char *fid = "SaveQ330Packet";

    q330 = (Q330 *) args;

/* Add our serial number to the header */

    InsertQdplusSerialno(&q330->local.raw, q330->par.serialno);

/* Copy in the QDP packet from the digitizer */

    CopyQDPToQDPlus(&q330->local.raw, pkt);

    if (q330->first && pkt->hdr.cmd == QDP_DT_DATA) {
        LogMsg(LOG_INFO, "initial data packet received from Q330 No. %016llX", q330->par.serialno);
        q330->first = FALSE;
    }

/* Save it */

    ProcessLocalData(&q330->local);

}

/* called by the qdp library each time it gets a new set of handshake meta-data */

void SaveQ330Meta(void *args, QDP_META *meta)
{
Q330 *q330;
static char *fid = "SaveQ330Meta";

    q330 = (Q330 *) args;
    ProcessMetaData(q330, meta);
}

/* Initialize a new Q330 instance (except for metadata) */

static BOOL InitQ330(ISIDL_PAR *par, Q330 *q330, Q330_CFG *cfg, char *argstr)
{
char *name;
Q330_ADDR addr;
int port, instance, debug = 0;
LNKLST *TokenList;
UINT64 serialno, authcode;
static char *fid = "InitQ330";

/* Parse the argument list */

    if ((TokenList = utilStringTokenList(argstr, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }
    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (TokenList->count != 3 && TokenList->count != 4) {
        fprintf(stderr, "incorrect q330 string \"%s\"\n", argstr);
        return FALSE;
    }

    if (TokenList->count == 4) debug = atoi((char *) TokenList->array[3]);

    name = (char *) TokenList->array[0];
    port = atoi((char *) TokenList->array[1]);
    if (port < 1 || port > 4) {
        fprintf(stderr, "%d is an incorrect q330 data port number\n");
        return FALSE;
    }
    instance = atoi((char *) TokenList->array[2]);

/* Lookup the serial number and auth code from the name */

    if (!q330LookupAddr(name, cfg, &addr)) {
        fprintf(stderr, "Unable to locate '%s' in Q330 config file '%s'\n", name, cfg->fname);
        return FALSE;
    }

    qdpInitPar(&q330->par, port);
    qdpSetHost(&q330->par, addr.name);
    qdpSetSerialno(&q330->par, addr.serialno);
    qdpSetAuthcode(&q330->par, addr.authcode);
    qdpSetMyCtrlPort(&q330->par, instance);
    qdpSetMyDataPort(&q330->par, instance);
    qdpSetDebug(&q330->par, debug);
    q330->lp = par->lp;
    q330->first = TRUE;
    q330->qp = NULL;
    return TRUE;
}

/* Add a new Q330 instance to the list */

char *AddQ330(ISIDL_PAR *par, char *argstr, char *cfgpath)
{
Q330 new;
static Q330_CFG *cfg = NULL;
static char *fid = "AddQ330";
int i;

/* Read the config file once, the first time here */

    if (cfg == NULL) {
        if ((cfg = q330ReadCfg(cfgpath)) == NULL) {
            fprintf(stderr, "%s: q330ReadCfg: %s\n", fid, strerror(errno));
            return NULL;
        }
    }

    if (!InitQ330(par, &new, cfg, argstr)) return NULL;
    if (!listAppend(&par->q330, &new, sizeof(Q330))) {
        fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
        return NULL;
    }

    return cfg->fname;
}

/* Read packets from the Q330.  Since we are using the user supplied
 * packet function, we just block forever and let the library drive
 * all further action.
 */

static THREAD_FUNC Q330Thread(void *argptr)
{
Q330 *q330;
static char *fid = "Q330Thread";

    q330 = (Q330 *) argptr;
    qdpSetUser(&q330->par, (void *) q330, SaveQ330Packet);
    qdpSetMeta(&q330->par, (void *) q330, SaveQ330Meta);
    if ((q330->qp = qdpConnectWithPar(&q330->par, q330->lp)) == NULL) {
        LogMsg(LOG_INFO, "%s: qdpOpenWithPar: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }
#ifndef WIN32
    while (1) pause();
#endif /* !WIN32 */
}

/* Launch a Q330 reader */

void StartQ330Reader(Q330 *q330)
{
THREAD tid;
static char *fid = "StartQ330Thread";

    if (!THREAD_CREATE(&tid, Q330Thread, (void *) q330)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: Q330Thread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
}

/* Revision History
 *
 * $Log: q330.c,v $
 * Revision 1.12  2007/10/31 17:36:02  dechavez
 *  Lookup Q330_ADDR instead of serialno and authcode
 *
 * Revision 1.11  2007/09/07 20:05:38  dechavez
 * use Q330 config file to get address parameters via Q330_CFG
 *
 * Revision 1.10  2007/05/11 16:24:50  dechavez
 * fixed incorrect fid
 *
 * Revision 1.9  2007/02/20 02:35:14  dechavez
 * aap (2007-02-19)
 *
 * Revision 1.8  2007/02/08 22:53:17  dechavez
 * LOG_ERR to LOG_INFO, use LOCALPKT handle
 *
 * Revision 1.7  2006/12/12 23:28:10  dechavez
 * use QDP metadata callback for tranparent saving of metadata
 *
 * Revision 1.6  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.5  2006/06/27 00:25:58  dechavez
 * switch to library qdpParseArgString() for parsing command line args
 *
 * Revision 1.4  2006/06/23 18:31:19  dechavez
 * Added client side port parameter to Q330 argument list
 *
 * Revision 1.3  2006/06/15 00:01:42  dechavez
 * added missing return value for InitQ330()
 *
 * Revision 1.2  2006/06/07 22:40:25  dechavez
 * block forever after return from qdpConnectWithPar()
 *
 * Revision 1.1  2006/06/02 21:07:56  dechavez
 * initial release
 *
 */
