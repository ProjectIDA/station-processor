// ScreenElem.cpp: implementation of the CScreenElem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "ScreenElem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenElem::CScreenElem(int nRec)
{
state=0; nSel=-1; nWfdRec=nRec;
}

CScreenElem::~CScreenElem()
{

}
