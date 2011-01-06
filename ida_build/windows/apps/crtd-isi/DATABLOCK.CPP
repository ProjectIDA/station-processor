// DataBlock.cpp: implementation of the CDataBlock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crtd.h"
#include "DataBlock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataBlock::CDataBlock()
	{
	data = NULL;
	}

CDataBlock::~CDataBlock()
	{
	if(data!=NULL) delete[] data;
	data=NULL;
	}

void CDataBlock::PutData(int n, long *plData)
	{
	data=new long[n];
	if(data!=NULL)
		{
		for(int i=0; i<n; ++i)
			{
			data[i] = plData[i];
			}
		}
	}

void CDataBlock::PutData(int n, UINT8 *uint8data)
	{
	data=new long[n];
	if(data!=NULL)
		{
		for(int i=0; i<n; ++i)
			{
			data[i] = uint8data[i];
			}
		}
	}

