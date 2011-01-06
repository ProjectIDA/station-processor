// checkaspi32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "checkaspi32.h"
#include "win32/wnaspi32.h"




BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
		}
    return TRUE;
}


extern "C" LONG APIENTRY ChkASPI32(HWND hwnd, LPLONG lpIValue, LPSTR lpszValue)
	{

	HINSTANCE hlibAPI = 0;
	LPSENDASPI32COMMAND lpfnSendASPI32Command;
	LPGETASPI32SUPPORTINFO lpfnGetASPI32SupportInfo;

	UINT fuError;



	fuError = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	hlibAPI = LoadLibrary("wnaspi32.dll");
	SetErrorMode(fuError);

	if(hlibAPI==NULL) return 10;

	lpfnSendASPI32Command=(LPSENDASPI32COMMAND)GetProcAddress (hlibAPI,"SendASPI32Command");
	lpfnGetASPI32SupportInfo=(LPGETASPI32SUPPORTINFO)GetProcAddress (hlibAPI,"GetASPI32SupportInfo");

	if(lpfnSendASPI32Command==NULL || lpfnGetASPI32SupportInfo==NULL)
		{
		FreeLibrary(hlibAPI);
		MessageBox(hwnd,"ASPI32 error", "ASPI test",0);
		return 9;
		}

	{
	DWORD ASPIStatus;
	BYTE NumAdapters;
	ASPIStatus = GetASPI32SupportInfo();
	switch ( HIBYTE(LOWORD(ASPIStatus)) )
		{
		case SS_COMP:
   /*
    * ASPI for Win32 is properly initialized
    */
			NumAdapters = LOBYTE(LOWORD(ASPIStatus));
			break;
		default:
			MessageBox(hwnd,"ASPI32 error", "ASPI test",0);
			return 2;
		}
   
	}
	
	FreeLibrary(hlibAPI);
	hlibAPI=NULL;
	MessageBox(hwnd,"ASPI32 is OK", "ASPI test",0);
	return 0;
	}


