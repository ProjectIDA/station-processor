#pragma ident "$Id: mail.cpp,v 1.2 2008/01/18 20:16:59 dechavez Exp $"
#include "platform.h"
#include "util.h"
#include <vector>
#include <string>

const static char *pHead = "Net       Sta   BeamID          Date       Time      Phase     Azim  Slow   SNR       Amp   Per   STA   Dur Author       DetID\n";
using namespace std;
const static char *pNet = "IDAHUB";

// 1-9 Network a9 Network code
// 11-15 Sta a5 Station code
// 17-28 Beamid a12 Beam ID
// 30-39 Date i4,a1,i2,a1,i2 Detection date (yyyy/mm/dd) 
// 41-52 Time i2,a1,i2,a1,f6.3 Detection time (hh:mm:ss.sss) 
// 54-61 Phase a8 Preliminary phase code 
// 63-67 Azim f5.1 Observed backazimuth (degrees) 
// 69-73 Slow f5.1 Observed slowness (seconds/degree) 
// 75-79 SNR f5.1 Signal-to-noise ratio 
// 81-89 Amp f9.1 Amplitude (nanometers) 
// 91-95 Per f5.2 Period (seconds) 
// 97-101 STA f5.1 Short term average 
// 103-107 Duration f5.1 Detection duration (seconds) 
// 109-117 Author a9 Author of the detection 
// 119-126 DetID a8 Detection ID 



typedef struct
	{
	char sta[16];
	double t;
	double dSTA;
	double SNR;
	float ampl;
	float per;
	}EVNTINFO;

//static CRITICAL_SECTION critsec;

static vector <EVNTINFO> evnt;



void AddArrivalToMailQueue(const char * Sta, double t, double dSTA, float fSNR, float ampl, float per)
	{
	EVNTINFO ei;

	strcpy(ei.sta, Sta);
	ei.t    = t;
	ei.dSTA = dSTA;
	ei.SNR  = fSNR;
	ei.ampl = ampl;
	ei.per  = per;

	evnt.push_back(ei);

	}

void SendAlert(const char *pemailaddr)
	{
	int yr,mnth,da,day,hh,mm,sc,ms;
	int i;
	char buff[32];
	char line[130];
	string sMessage;

	sMessage = pHead;
	if(evnt.size()==0) return;

	for(i=0; i<evnt.size(); ++i)
		{
		memset(line,' ', 130);
		line[129] = 0;
		line[128] = '\n';

//		EVNTINFO ei = evnt[i]
//		if(MailerParam::recipient.size()>0)
//			{
//			sprintf(buff,"%-9s", MailerParam::network.c_str());
//			}
//		else
//			{
//			sprintf(buff,"%-9s", "OBN");
//			}
		memcpy(&line[0],pNet,strlen(pNet));

		sprintf(buff,"%-5s", evnt[i].sta);
		util_ucase(buff);
		memcpy(&line[10],buff,strlen(buff));

		sprintf(buff,"%-12s", "mb_beam");
		memcpy(&line[16],buff,strlen(buff));


		utilTsplit(evnt[i].t, &yr, &da, &hh, &mm, &sc, &ms);
		utilJdtomd(yr, da, &mnth, &day);



// 30-39 Date i4,a1,i2,a1,i2 Detection date (yyyy/mm/dd) 

		sprintf(buff,"%4d/%02d/%02d", yr, mnth, day);
		memcpy(&line[29],buff,strlen(buff));

// 41-52 Time i2,a1,i2,a1,f6.3 Detection time (hh:mm:ss.sss) 
		sprintf(buff,"%02d:%02d:%02d.%03d", hh, mm, sc, ms);
		memcpy(&line[40],buff,strlen(buff));

// 54-61 Phase a8 Preliminary phase code 

		sprintf(buff,"%-12s", "N");
		memcpy(&line[53],buff,strlen(buff));

// 63-67 Azim f5.1 Observed backazimuth (degrees) 

		sprintf(buff,"%5.1f", 0.f);
		memcpy(&line[62],buff,strlen(buff));


// 69-73 Slow f5.1 Observed slowness (seconds/degree) 

		sprintf(buff,"%5.1f", 0.f);
		memcpy(&line[68],buff,strlen(buff));

// 75-79 SNR f5.1 Signal-to-noise ratio 

		sprintf(buff,"%5.1f", evnt[i].SNR);
		memcpy(&line[74],buff,strlen(buff));


// 81-89 Amp f9.1 Amplitude (nanometers) 

		sprintf(buff,"%9.1f", evnt[i].ampl);
		memcpy(&line[80],buff,strlen(buff));

// 91-95 Per f5.2 Period (seconds) 
		sprintf(buff,"%5.2f", evnt[i].per);
		memcpy(&line[90],buff,strlen(buff));

// 97-101 STA f5.1 Short term average 
		sprintf(buff,"%5.1f", evnt[i].dSTA);
		memcpy(&line[90],buff,strlen(buff));

// 103-107 Duration f5.1 Detection duration (seconds) 
		sprintf(buff,"%5.1f", 1.);
		memcpy(&line[102],buff,strlen(buff));

// 109-117 Author a9 Author of the detection 
		sprintf(buff,"%-9s", "AUZ_ISI");
		memcpy(&line[108],buff,strlen(buff));

// 119-126 DetID a8 Detection ID 

		sprintf(buff,"%08d", i);
		memcpy(&line[118],buff,strlen(buff));

		sMessage += line;

		}
	evnt.clear();

	FILE *f = fopen("auz_isi.mail","wt");
	if(f!=NULL)
		{
		fwrite(sMessage.c_str(), 1, sMessage.size(), f);
		fclose(f);
#ifndef WIN32
		util_email((char *)pemailaddr, "ALERT", "auz_isi.mail");
#endif
		}

	}

/* Revision History
 *
 * $Log: mail.cpp,v $
 * Revision 1.2  2008/01/18 20:16:59  dechavez
 * upcase station name
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
