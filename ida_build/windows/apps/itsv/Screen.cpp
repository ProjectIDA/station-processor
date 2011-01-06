// Screen.cpp: implementation of the CScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "Screen.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreen::CScreen()
	{
	nMaxSel=0;
	}

CScreen::~CScreen()
	{
	Clear();
	}
void CScreen::Clear()
	{
	nMaxSel=0;
	while(Screen.GetSize()>0)
		{
		CScreenElem  *pElm=Screen[0];
		Screen.RemoveAt(0);
		delete pElm;
		}
	}
CScreen::AddElement(int nRec)
	{
	CScreenElem  *pElm=new CScreenElem(nRec);
	Screen.Add(pElm);
	}
int CScreen::GetMaxWindow()
	{
	return Screen.GetSize();
	}
int CScreen::GetWaveformN(int n)
	{
	CScreenElem  *pElm=Screen[n];
	return pElm->nWfdRec;
	}
bool CScreen::IsSelected(int n)
	{
	CScreenElem  *pElm=Screen[n];
	if(pElm->state==0)
		return false;
	return true;
	}

int CScreen::SelectUnselectElement(int nWindow)
	{
	CScreenElem  *pElm=Screen[nWindow];
	if(pElm->state==0)
		{
		++pElm->state;
		pElm->nSel=nMaxSel++;
		return 1;
		}
	else
		{
		int nMax=GetMaxWindow();
		pElm->state=0;
		
		for(int i=0; i<nMax; ++i)
			{
			CScreenElem  *pElma=Screen[i];
			if(pElma->state>0)
				{
				if(pElma->nSel>pElm->nSel)
					{
					--pElma->nSel;
					}
				}
			}
		--nMaxSel;
		pElm->nSel=0;
		return 0;
		}
	}
int __cdecl  Compare(const void *arg1, const void *arg2 )
	{
	SREANELEM *se1=(SREANELEM *)arg1;
	SREANELEM *se2=(SREANELEM *)arg2;


	if(se1->nPos>se2->nPos)
		{
		return -1;
		}
	else
		{
		return 1;
		}
	}


void CScreen::SelectTraces()
	{
	int i=0;

	while(i<GetMaxWindow())
		{
		CScreenElem  *pElma=Screen[i];
		if(pElma->state==0)
			{
			Screen.RemoveAt(i);
			delete pElma;
			continue;
			}
		else
			{
			pElma->state=0;
			++i;
			}
		}
	int nMax=GetMaxWindow();
	struct SREANELEM  *scrMas=new SREANELEM[nMax];

	for(i=0; i<nMax; ++i)
		{
		CScreenElem  *pElm=Screen[i];
		scrMas[i].nPos=pElm->nSel;
		scrMas[i].nWfdRec=pElm->nWfdRec;
		}
	qsort(scrMas,nMax,sizeof(SREANELEM),Compare);
	Clear();

	for(i=0; i<nMax; ++i)
		{
		AddElement(scrMas[i].nWfdRec);
		}
	nMaxSel=0;
	delete[]scrMas;
	}


void CScreen::DeleteTraces()
	{
	int i=0;

	while(i<GetMaxWindow())
		{
		CScreenElem  *pElma=Screen[i];
		if(pElma->state!=0)
			{
			Screen.RemoveAt(i);
			delete pElma;
			continue;
			}
		else
			{
			pElma->state=0;
			++i;
			}
		}
	int nMax=GetMaxWindow();
	struct SREANELEM  *scrMas=new SREANELEM[nMax];

	for(i=0; i<nMax; ++i)
		{
		CScreenElem  *pElm=Screen[i];
		scrMas[i].nPos=pElm->nSel;
		scrMas[i].nWfdRec=pElm->nWfdRec;
		}
	qsort(scrMas,nMax,sizeof(SREANELEM),Compare);
	Clear();

	for(i=0; i<nMax; ++i)
		{
		AddElement(scrMas[i].nWfdRec);
		}
	nMaxSel=0;
	delete[]scrMas;
	}




void CScreen::SelectAll()
	{
	int nMaxWin=GetMaxWindow();
	for(int i=0; i<nMaxWin; ++i)
		{
		CScreenElem  *pElma=Screen[i];
		pElma->state=1;
		pElma->nSel=i;
		}
	nMaxSel=nMaxWin;
	}
void CScreen::UnSelectAll()
	{
	int nMaxWin=GetMaxWindow();
	for(int i=0; i<nMaxWin; ++i)
		{
		CScreenElem  *pElma=Screen[i];
		pElma->state=0;
		}
	nMaxSel=0;
	}
int CScreen::GetSelectionNumber(int n)
	{
	CScreenElem  *pElm=Screen[n];
	return pElm->nSel;
	}