#include "stdafx.h"

void CopyStringWithBlankRemove(LPSTR lpTar, LPSTR lpSou)
	{
	LPSTR lpS=lpSou,lpT=lpTar;

	while(1)
		{
		switch(*lpS)
			{
			case 0:
				*lpT=0;
				return;
			case ' ':
				++lpS;
				continue;
			default:
				*lpT++=*lpS++;
			}
		}
	}
UINT APIENTRY MyHook(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) 
	{ 
	LPOFNOTIFY lpNotify;
	switch(uiMsg)
		{
		case WM_NOTIFY:
			lpNotify=(LPOFNOTIFY)lParam;
			break;

		}
	TRACE("%d\n", uiMsg); 

	return 0; 

	} 
