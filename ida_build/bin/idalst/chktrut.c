#pragma ident "$Id: chktrut.c,v 1.1 2006/03/24 18:38:29 dechavez Exp $"
/*=====================================================================
 *
 *  check for true time gaps.
 *
 *===================================================================*/
#include "idalst.h"
#include "util.h"
#include "list.h" 

typedef struct
    {
    int nstream;
    double t;
    }
stream_t;

static LNKLST *streamList=NULL;
static LNKLST *messageList=NULL;

extern struct globals *Global;

static stream_t *getStream(int nstream)
    {
    stream_t stt, *pstt;

    LNKLST_NODE *node; 
    node = listFirstNode(streamList);

    while (node != NULL) 
        {
        pstt = node->payload;
        if(pstt->nstream==nstream) break;
        node = listNextNode(node);
        }

    if(node==NULL)
        {
        stt.t       = 0.;
        stt.nstream = nstream;
        listAppend(streamList, &stt, sizeof(stt)); 
        node = listLastNode(streamList);
        }

    return (stream_t *)node->payload;
    }

static void putMessagetoList(char *msg)
    {
    if(messageList==NULL) 
        {
        messageList=malloc(sizeof(LNKLST));
        if(messageList==NULL)
            {
/*    log error           */
            exit(1);
            }
        listInit(messageList);
        }
    listAppend(messageList, msg, strlen(msg)+1);

    }

static void PutGapInfoToList(int nstream, double t1, double t2)
    {
    char msg[512],tmbuff1[32],tmbuff2[32],cstream[10],gaplen[32];
    double dt;
    int hh,mm,ss,ms;
    

    dt = fabs(t2-t1);

    hh=(int)(dt/3600.);
    mm=((long)dt%3600)/60;
    ss=((long)dt%60);
    ms=(dt-floor(dt))*1000;

    sprintf(gaplen,"%d:%02d:%02d.%d", hh, mm, ss, ms);


    utilDttostr(t1,0, tmbuff1);
    utilDttostr(t2,0, tmbuff2);
    sprintf(cstream,"%d", nstream);
    memset(msg,' ',100); msg[99]='\n';msg[100]=0;
    memcpy(msg,cstream,strlen(cstream));
    memcpy(&msg[6],tmbuff1,strlen(tmbuff1));
    memcpy(&msg[32],tmbuff2,strlen(tmbuff2));
    memcpy(&msg[72], gaplen, strlen(gaplen));
    putMessagetoList(msg);
    }

void chktrut(IDA_DHDR *dhead)
    {
    int nstream;
    stream_t *pInfo;

    if(streamList==NULL) 
        {
        streamList=malloc(sizeof(LNKLST));
        if(streamList==NULL)
            {
/*    log error           */
            exit(1);
            }
        listInit(streamList);

        }

    nstream = dhead->dl_stream;

    pInfo = getStream(nstream);

    if(pInfo->t>0)
        {

        if( (fabs(dhead->beg.tru - pInfo->t) > 20.) )
            {
            PutGapInfoToList(nstream, pInfo->t, dhead->beg.tru);
            }
        else
            {
            if( (fabs(dhead->end.tru - (dhead->nsamp*dhead->sint) - pInfo->t) > 20.) )
                {
                PutGapInfoToList(nstream, pInfo->t, dhead->end.tru);
                }
            }
        }

    pInfo->t = dhead->end.tru;
    }

static void DeleteLists()
    {
    listDestroy(streamList);
    listDestroy(messageList);
    }

void printttglist()
    {
    char msg[101];
    LNKLST_NODE *node; 

    printf("\nDATA GAP INFO\n");

    node = listFirstNode(messageList);

    if(node==NULL)
        {
        printf("\nNo gaps found\n");
        }
    else
        {
        memset(msg,' ', 101);msg[99]='\n';msg[100]=0;
        memcpy(msg,"Stream",6);
        memcpy(&msg[40],"Gap",3);
        memcpy(&msg[80],"Length",6);
        printf(msg);
        }
    while (node != NULL) 
        {
        printf((char *)node->payload);
        node = listNextNode(node);
        }

    DeleteLists();
    }

/* Revision History
*
* $Log: chktrut.c,v $
* Revision 1.1  2006/03/24 18:38:29  dechavez
* initial release (akimov)
*
*/
