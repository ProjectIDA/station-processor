// DataBlock.h: interface for the CDataBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABLOCK_H__F0C2AFB6_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_DATABLOCK_H__F0C2AFB6_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "platform.h"

class CDataBlock  
{
public:
	CDataBlock();
	virtual ~CDataBlock();
	void PutData(int n, long *plData);
	void PutData(int n, UINT8 *uint8data);

public:
    char   sname[32];			   /* station name                     */
    float  lat;                    /* latitutude                       */
    float  lon;                    /* longitude                        */
    float  elev;                   /* elevation                        */
    float  depth;                  /* sensor depth                     */
    char   cname[32];			   /* channel name                     */
    float  sint;                   /* nominal sample interval, sec     */
    double beg;                    /* time of first sample in packet   */
    double end;                    /* time of last  sample in packet   */
    int    tear;                   /* if set, there was a time tear    */
    long   nsamp;                  /* number of samples                */
    long   *data;                  /* raw data storage (if required)   */
	};

#endif // !defined(AFX_DATABLOCK_H__F0C2AFB6_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
