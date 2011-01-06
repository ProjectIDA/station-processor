#pragma ident "$Id: auz_isi.cpp,v 1.4 2009/02/02 22:34:19 dechavez Exp $"
// auz_isi.cpp : Defines the entry point for the console application.
//

#include "isi.h"
#include "util.h"
#include "auzinfo.h"
#include "detect.h"
#include <vector>
#include <string>
#include "auzglobals.h"
#include "mail.h"

using namespace std;

void LoadChannelList(vector <AUZINFO> &channels);
void LoadConfigFromServer(const char *server, int nNrtsPort, vector <AUZINFO> &channels);
bool BackGround();
void help(char *myname);

int main(int argc, char* argv[])
	{
	time_t t1=0, t2=0;
	int status, j, i;
	ISI_GENERIC_TS *ts;
	ISI *isi=NULL;
	ISI_PARAM par;
	REAL64 begtime = ISI_NEWEST;
	REAL64 endtime = ISI_KEEPUP;
	char *begstr = NULL;
	int maxdur    = 0;
	int compress  = ISI_COMP_NONE;
	int format    = ISI_FORMAT_GENERIC;
	char *dbspec  = NULL;
	ISI_DATA_REQUEST *dreq;
	string streams;
	vector <AUZINFO> channels;
	static char *fid = "main"; 


    utilNetworkInit();
    isiInitDefaultPar(&par);

	if(argc<2) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            auzglobals::server = argv[i] + strlen("server=");
        } else if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
            auzglobals::port = atoi(argv[i] + strlen("port="));
            isiSetServerPort(&par, auzglobals::port);
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            begstr = argv[i] + strlen("beg=");
		} else if (strncmp(argv[i], "email=", strlen("email=")) == 0) {
            auzglobals::pemail = argv[i] + strlen("email=");
		} else if (strncmp(argv[i], "-bd", strlen("-bd")) == 0) {
            auzglobals::bd = true;
		} else if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
		} else if (strncmp(argv[i], "-debug", strlen("-debug")) == 0) {
            auzglobals::debug = true;
		}

		}

	if(auzglobals::pemail.length()==0) help(argv[0]);

	if(auzglobals::bd) utilBackGround();

	util_logopen("syslogd", 1, 9, 1, (char *)auzglobals::server.c_str(), argv[0]);



	if (begstr != NULL) begtime = utilAttodt(begstr);

	util_log(1, "started: email=%s server=%s port=%d", auzglobals::pemail.c_str(),
		auzglobals::server.c_str(), auzglobals::port);

	LoadChannelList(channels);
	LoadConfigFromServer(auzglobals::server.c_str(), auzglobals::port, channels);
	int nElements=auzglobals::det_streams.size();
	for (i = 0; i<nElements; ++i) 
		{
		string channelName="+";
		channelName+=auzglobals::det_streams[i]->sta+"."+auzglobals::det_streams[i]->chan+"."+auzglobals::det_streams[i]->lcode;
		streams+=channelName;
		}



	while(auzglobals::bContinue)
		{

		if ((dreq = isiAllocSimpleDataRequest(begtime, endtime, (char *)streams.c_str())) == NULL) 
			{
			char cErr[1024];
			sprintf(cErr, "%s: isiAllocSimpleDataRequest error. Streams :%s Program terminated", fid, streams.c_str());
			if(auzglobals::bd)util_log(1, cErr);
			printf("\n%s", cErr);
			exit(-1);
			continue;
			}
		isiSetDatreqFormat(dreq, format);
		isiSetDatreqCompress(dreq, compress);

		if(!auzglobals::bContinue) break;

		if ((isi = isiInitiateDataRequest((char *)auzglobals::server.c_str(), &par, dreq)) == NULL) 
			{
			if(auzglobals::bd) util_log(1, "unable to connect to %s:%d", auzglobals::server.c_str(), auzglobals::port); 
			sleep(5);
			continue;
			}
		util_log(1, "Connected to %s, I/O timeout=%d sec", auzglobals::server.c_str(), auzglobals::timeout);
		isiFreeDataRequest(dreq);


		while(auzglobals::bContinue) 
			{
			ts = isiReadGenericTS(isi, &status);

			if(status == ISI_ERROR) continue;
			if(status == ISI_BREAK) break;
			if(status == ISI_DONE) break;
			if( !(ts->hdr.desc.type==ISI_TYPE_INT16||ts->hdr.desc.type==ISI_TYPE_INT32)) continue;

			char *sta, *chan, *loc;
			double sint;

			sint = isiSrateToSint(&ts->hdr.srate);

			sta  = ts->hdr.name.sta;
			chan = ts->hdr.name.chn;
			loc  = ts->hdr.name.loc;

			int nsamp = ts->hdr.nsamp;
			long *data = new long[nsamp];
			short *shData = (short *)ts->data;
			long  *lData  = (long *)ts->data;


			if(data==NULL){
				char cErr[1024];
				sprintf(cErr, "%s: unable to allocate memory. Program terminated", fid);
				if(auzglobals::bd) util_log(1, cErr);
				printf("\n%s", cErr);
				exit(-1);
			}


			for(i=0; i<nsamp; ++i)
				{
				if(ts->hdr.desc.type==ISI_TYPE_INT16)
					{
					data[i]=(long)shData[i];
					}
				if(ts->hdr.desc.type==ISI_TYPE_INT32)
					{
					data[i]=(long)lData[i];
					}

				}

		for (j = 0; j<nElements; ++j) 
			{
			if(strcasecmp(sta,auzglobals::det_streams[j]->sta.c_str())==0)
			if(strcasecmp(chan,auzglobals::det_streams[j]->chan.c_str())==0)
			if(strcasecmp(loc,auzglobals::det_streams[j]->lcode.c_str())==0)
				{
				auzglobals::det_streams[j]->SingleLoop(data, nsamp, ts->hdr.tofs.value);
				break;
				}

			}

		delete [] data;

		t1=time(NULL);
		if( abs(t1-t2) >300)
			{
			SendAlert(auzglobals::pemail.c_str());
			t2 = t1;
			}

		}
	}
//	isiDestroyGenericTS(ts);



	return 0;
}
//
// Load sta, channel, location and detector parameters
//
void LoadChannelList(vector <AUZINFO> &channels)
	{
	AUZINFO auzinfo;
	char *p;
	char buff[128];
	char sta[10], chan[10], loc[10];
	float fr1, fr2,threshold;
	int nlta, nsta;
	int order;
	static char *fid = "LoadChannelList"; 

	FILE *f = fopen("auz_isi.init","rt");
	if(f==NULL) {
		char cErr[1024];
		sprintf(cErr, "%s: unable to open configuration file auz_isi.init. Program terminated", fid);
		util_log(1, cErr);
		printf("\n%s", cErr);
		exit(-1);
	}

	while(fgets(buff, sizeof(buff)-1, f)!=NULL)
		{
		p=strchr(buff,'#');
		if(p!=NULL) *p = 0;
		if(strlen(buff)<15) continue;

		sscanf(buff,"%5s %5s %5s %d %d %f %f %f %d", sta, chan, loc, &nlta, &nsta, &threshold, &fr1, &fr2, &order);
		auzinfo.sta  = sta;
		auzinfo.chan = chan;
		if(loc[0]=='-') auzinfo.loc = "  "; else auzinfo.loc=loc;
		if( (fr1<0) ) continue;
		auzinfo.fr1 = fr1;
		if( (fr2<=0.) || (fr2<fr1) ) continue;
		auzinfo.fr2 = fr2;
		
		if(nlta<0) continue;
		auzinfo.flta = nlta;
		if(nsta<0) continue;
		auzinfo.fsta = nsta;

		if(threshold<=0) continue;
		auzinfo.threshold = threshold;

		if(order<=0 && order>10) continue;
		auzinfo.order = order;

		channels.push_back(auzinfo);


		}
	fclose(f);
	if(channels.size()==0)
		{
		char cErr[1024];
		sprintf(cErr, "%s: auz_isi.init is empty. There is nothing to do. Program terminated", fid);
		if(auzglobals::bd) util_log(1, cErr);
		printf("\n%s", cErr);
		exit(-1);
		}
	}


void LoadConfigFromServer(const char *server, int nNrtsPort, vector <AUZINFO> &channels)
	{
	int i,j;
	ISI_PARAM par;
	static char *fid = "LoadConfigFromServer"; 

    utilNetworkInit();
    isiInitDefaultPar(&par);
	

	ISI_CNF_REPORT *report;

	par.port = nNrtsPort;

    while ((report = isiCnf((char *)server, &par)) == NULL) 
		{
		char cErr[1024];
		sprintf(cErr, "%s: unable to get configuration from %s", fid, server);
		if(auzglobals::bd) util_log(1, cErr);
		printf("\n%s", cErr);
		sleep(5);
		}

	for(j=0; j<channels.size(); ++j)
		{

		for (i = 0; i < report->nentry; i++)
			{
			string sta, chan, lcode;
			double sint;
			char *p;


			p = report->entry[i].name.sta;
			sta    = p;
			chan   = report->entry[i].name.chn;
			lcode = report->entry[i].name.loc;

			sint = isiSrateToSint(&report->entry[i].srate);

			if(strcasecmp(sta.c_str(),channels[j].sta.c_str())==0)
			if(strcasecmp(chan.c_str(),channels[j].chan.c_str())==0)
			if(strcasecmp(lcode.c_str(),channels[j].loc.c_str())==0)
				{
				CDetect *pdet = new CDetect();
				if(pdet==NULL){
					char cErr[1024];
					sprintf(cErr, "%s: unable to allocate memory. Program terminated", fid);
					util_log(1, cErr);
					printf("\n%s", cErr);
					exit(-1);
				}

				channels[j].sps    = 1./sint;
				channels[j].calib  = report->entry[i].inst.calib;
				channels[j].calper = report->entry[i].inst.calper;
				pdet->Init(channels[j]);
				auzglobals::det_streams.push_back(pdet);
				}

			}
		}
	isiFreeCnf(report);


	return ;
	}
void help(char *myname)
	{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options] \n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
	fprintf(stderr,"email=string      sets email address to send alert. *REQUIRED*\n");
    fprintf(stderr,"server=string     sets the name of the server to the specified string. Default value is 127.0.0.1\n");
    fprintf(stderr,"to=secs           sets read timeout. Default value is 60\n");
    fprintf(stderr,"port=int          sets the port number. Default value is 39136\n");
	fprintf(stderr,"beg=yyyy:ddd-hh:mm:ss   sets data start time.\nIf not specified, current time is used\n");
    fprintf(stderr,"-bd               run in the background\n");
    fprintf(stderr,"-debug            turn on debug messages\n");
    fprintf(stderr,"-h                this help\n");
    fprintf(stderr,"\n"); 
	exit(0);
	} 
/* Revision History
 *
 * $Log: auz_isi.cpp,v $
 * Revision 1.4  2009/02/02 22:34:19  dechavez
 * akimov 2/2/2009 bug fix
 *
 * Revision 1.3  2008/02/01 18:23:44  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.2  2008/01/27 16:56:56  akimov
 * Improved various error diagnostic and added usage message, daemon mode
 * (-bd) and -debug option to log information about alerts.
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
