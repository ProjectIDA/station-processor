// MWindow.h: interface for the CMWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MWINDOW_H_INCLUDED_)
#define _MWINDOW_H_INCLUDED_
#include <qstring.h>
#include <qptrlist.h> 
#include "DataBlock.h"
class CMWindow  
{
public:
	unsigned long nBlocksRec;
	int x1,x2,y1,y2;
	int xw1,xw2,yw1,yw2;
	QString StaName;
	QString ChanName;
	QString LCODE;
	double dSpS;
	long amin,amax;
	QPtrList<CDataBlock> xferData;

public:
	CMWindow();
	virtual ~CMWindow();
	void Add(CDataBlock *dblk);

};

#endif // !defined(_MWINDOW_H_INCLUDED_)
