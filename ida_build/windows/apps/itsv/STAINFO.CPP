// StaInfo.cpp: implementation of the CStaInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "StaInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaInfo::CStaInfo()
	{

	}

CStaInfo::~CStaInfo()
	{
	Clear();
	}
void CStaInfo::Clear()
	{
	while(AllSta.GetSize()>0)
		{
		CSta *si=AllSta[0];
		AllSta.RemoveAt(0);
		delete si;
		}

	}
void CStaInfo::AddInfo(wfdisc &wfd)
	{
	int i,j;
	CChanInfo *ci;
	CSta *si;

	for(i=0; i<AllSta.GetSize();++i)
		{
		if(AllSta[i]->Sta.Compare(wfd.sta)==0)
			{
			for(j=0; j<AllSta[i]->ChanInfo.GetSize(); ++j)
				{
				if(AllSta[i]->ChanInfo[j]->Chan.CompareNoCase(wfd.chan)==0)
					{
					return; // Sta and channel already exist in the list. No adding
					}
				}
			// Station exist, but channel info should be added
			ci=new	CChanInfo(wfd.chan, wfd.smprate, true);
			AllSta[i]->ChanInfo.Add(ci);
			return;
			}
		}
	ci=new	CChanInfo(wfd.chan, wfd.smprate, true);
	si=new CSta(wfd.sta);
	i=AllSta.Add(si);
	AllSta[i]->ChanInfo.Add(ci);
	return;
	}
CStaInfo &CStaInfo::operator =(CStaInfo &sinf)
	{
	CChanInfo *ci;
	CSta *si;
	int i,j;

	Clear();

	for(i=0; i<sinf.AllSta.GetSize(); ++i)
		{
		si=new CSta(sinf.AllSta[i]->Sta);
		AllSta.Add(si);
		for(j=0; j<sinf.AllSta[i]->ChanInfo.GetSize(); ++j)
			{
			ci=new	CChanInfo(sinf.AllSta[i]->ChanInfo[j]->Chan, sinf.AllSta[i]->ChanInfo[j]->dSpS, sinf.AllSta[i]->ChanInfo[j]->bSelected);
			AllSta[i]->ChanInfo.Add(ci);
			}
		}
	return *this;
	}