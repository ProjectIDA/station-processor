#pragma ident "$Id: isiclient.cpp,v 1.3 2008/02/01 18:23:55 akimov Exp $"

#define INCLUDE_IACP_DEFAULT_ATTR 
#define INCLUDE_ISI_STATIC_SEQNOS
#include "platform.h"
#include "isi.h"
#include "util.h"
#include "iacp.h"
#include "ISICLIENT_PARAM.h"
#include "ServerThreadPool.h"




static BOOL ReadRawPacket(ISI *isi, ISI_RAW_PACKET *raw)
{
int status;
static char *fid = "ReadRawPacket";

    while ((status = isiReadFrame(isi, TRUE)) == ISI_OK) {
        if (isi->frame.payload.type != ISI_IACP_RAW_PKT) {
            fprintf(stderr, "unexpected type %d packet ignored\n", isi->frame.payload.type);
        } else {
            isiUnpackRawPacket(isi->frame.payload.data, raw);
            return TRUE;
        }
    }

    switch (status) {
      case ISI_DONE:
        fprintf(stderr, "request complete\n");
        break;

      case ISI_BREAK:
		if(ISICLIENT_PARAM::bd)
			util_log(1, "%s: isiReadFrame: %s", fid, "server disconnect");
		else
			fprintf(stderr, "server disconnect\n"); 
        break;

      default:
		if(ISICLIENT_PARAM::bd)
			util_log(1, "%s: isiReadFrame: %s", fid, "unknown error");			
		else
			fprintf(stderr, "isiReadFrame: unknown error");
        break;
    }

    return FALSE;
} 

static THREAD_FUNC ISIClientthread(void *argptr)
	{
	#define LOCALBUFLEN 1024
	UINT8 buf[LOCALBUFLEN]; 
	ISI_RAW_PACKET raw; 
	ISI_PARAM par;
	ISI *isi; 
	ISI_SEQNO begseqno = ISI_NEWEST_SEQNO, endseqno = ISI_KEEPUP_SEQNO; 
	ISICLIENT_PARAM *isi_param = (ISICLIENT_PARAM *)argptr;
	int nLoopCount=0;
	static char *fid = "ISIClientthread";


	ISI_DATA_REQUEST *dreq; 


   while(1){

		if ((dreq = isiAllocSimpleSeqnoRequest(&begseqno, &endseqno, (char *)isi_param->site.c_str())) == NULL) {
			util_log(1, "%s: isiAllocSimpleSeqnoRequest: %s", fid, strerror(errno)); 
			exit(-1);
		}

		par.port = isi_param->port;
		par.attr = IACP_DEFAULT_ATTR;
		par.debug = 0;
		par.lp = NULL;
		par.decompress = TRUE;

		isi = isiInitiateDataRequest((char *)isi_param->server.c_str(), &par, dreq); 

		isiFreeDataRequest(dreq); 

   		if (isi == NULL) {
        util_log(1, "unable to connect to %s:%d", isi_param->server.c_str(), par.port);
    	}
		else	
			{
			util_log(1, "Connected to %s, I/O timeout=%d sec", isi_param->server.c_str(), isi_param->timeout);
			nLoopCount=0;
			while (ReadRawPacket(isi, &raw)) 
				{

				if(raw.hdr.desc.type!=ISI_TYPE_MSEED)
					{
					char cErr[512];
					sprintf(cErr, "%s: Site %s disk loop type is not MSEED. It's impossible to proceed.Terminated.", fid, isi_param->site.c_str());
					util_log(1, cErr);
					puts(cErr);
					exit(-1);
					}

				begseqno = raw.hdr.seqno;
				isiIncrSeqno(&begseqno);

				if (!isiDecompressRawPacket(&raw, buf, LOCALBUFLEN)) 
					{
					if(ISICLIENT_PARAM::bd) 
						util_log(1, "%s: isiDecompressRawPacket: %s", fid, syserrmsg(errno)); 
					else
						fprintf(stderr, "isiDecompressRawPacket error\n");
					} 
				else 
					{
					if(!ISICLIENT_PARAM::bd) utilPrintHexDump(stderr, raw.payload, 64);
					ServerThreadPool::BroadcastData((const char *)raw.payload, raw.hdr.len.used);
					}
				}
			isiClose(isi);
			sleep(5);
			}
		sleep(nLoopCount+1);
		nLoopCount= (nLoopCount*2)%60+1;
	}


	}


void RunISIClientthread(ISICLIENT_PARAM *isi_param)
	{
	THREAD tid;
	static char *fid = "RunISIClientthread";

    if (!THREAD_CREATE(&tid, ISIClientthread, (void *) isi_param)) {
		util_log(1, "%s: THREAD_CREATE: %s", fid, syserrmsg(errno)); 
        exit(1);
    } 

	}

/* Revision History
 *
 * $Log: isiclient.cpp,v $
 * Revision 1.3  2008/02/01 18:23:55  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.2  2008/01/25 21:58:03  akimov
 * verify that input disk loop is of type ISI_TYPE_MSEED
 *
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
