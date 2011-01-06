#pragma ident "$Id: EvInfo.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// EvInfo.h: interface for the CEvInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVINFO_H__EA142716_92AE_11D3_8BF9_008048C20040__INCLUDED_)
#define AFX_EVINFO_H__EA142716_92AE_11D3_8BF9_008048C20040__INCLUDED_


class CEvInfo  
	{
	public:
		CEvInfo();
		virtual ~CEvInfo();
		double lastEventTime,timeToStopAlarm;
		double lastCountTime;
		BOOL bFlag;
	};

#endif // !defined(AFX_EVINFO_H__EA142716_92AE_11D3_8BF9_008048C20040__INCLUDED_)

/* Revision History
 *
 * $Log: EvInfo.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
