// DataBlock.h: interface for the CDataBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABLOCK_H__F0C2AFB6_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_DATABLOCK_H__F0C2AFB6_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "xfer.h"

class CDataBlock  
{
public:
	CDataBlock();
	virtual ~CDataBlock();
public:
    char   sname[XFER_SNAMLEN+1];  /* station name                     */
    float  lat;                    /* latitutude                       */
    float  lon;                    /* longitude                        */
    float  elev;                   /* elevation                        */
    float  depth;                  /* sensor depth                     */
    char   cname[XFER_CNAMLEN+1];  /* channel name                     */
    float  sint;                   /* nominal sample interval, sec     */
    double beg;                    /* time of first sample in packet   */
    double end;                    /* time of last  sample in packet   */
    int    tear;                   /* if set, there was a time tear    */
    long   nsamp;                  /* number of samples                */
    long   data[XFER_MAXDAT*4];   /* raw data storage (if required)   */
	};

#endif // !defined(AFX_DATABLOCK_H__F0C2AFB6_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
