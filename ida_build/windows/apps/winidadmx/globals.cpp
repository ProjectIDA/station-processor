#include "stdafx.h"

#pragma comment(lib, "version.lib")

BOOL bStartFromCD=FALSE;
CString csCD;

CString GetFileVersion()
	{
	HANDLE  hMem;         
	LPVOID  lpvMem; 
	BOOL  fRet;
    char  szFullPath[256];
	DWORD dwVerHnd;
	DWORD dwVerInfoSize; 
	CString csRet;
	char    szGetName[256]; 

    UINT  cchVer = 0;
	LPSTR lszVer = NULL;


	GetModuleFileName(AfxGetInstanceHandle(), szFullPath, sizeof(szFullPath));
	dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd); 
    if (dwVerInfoSize)     
		{
		VS_FIXEDFILEINFO *pvs;
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem); 
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem); 


		lstrcpy(szGetName, "\\"); 

		fRet = VerQueryValue(lpvMem, szGetName, (void **)&lszVer, &cchVer); 
		pvs = (VS_FIXEDFILEINFO *)lszVer;

		csRet.Format("%d.%d%d%d", 
			(pvs->dwProductVersionMS&0xffff0000)>>16,
			(pvs->dwProductVersionMS&0x0000ffff), 
			(pvs->dwProductVersionLS&0xffff0000)>>16,
			(pvs->dwProductVersionLS&0x0000ffff)); 
		GlobalUnlock(hMem);
		GlobalFree(hMem); 
		}
	return csRet;
	}
int CALLBACK      BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
	{
         switch(uMsg) 
			{
            case BFFM_INITIALIZED: 
               SendMessage(hwnd,BFFM_SETSELECTION,TRUE, pData);
               break;
            default:
               break;
			}
         return 0;
      }


DWORD CreateDir (LPCSTR pb )
	{
	BOOL  fOK;
	DWORD Erc;
	DWORD Depth;
	DWORD i1;
	char  SVDir[MAX_PATH];

	if ( CreateDirectory ( pb, NULL )) return 0;

	Erc = GetLastError ( );
	if ( Erc == ERROR_ALREADY_EXISTS ) return 0;

  if ( Erc != ERROR_PATH_NOT_FOUND ) return Erc;    // Path does not exist

  strcpy ( SVDir, pb );

  fOK   = FALSE;
  Depth = 0;

  do { for ( i1 = strlen ( SVDir ) - 1;
             i1 > 0 && !fOK && SVDir [i1] != '\\';
             i1-- );
       if ( i1 == 0 ) return Erc;
          { SVDir[i1] = '\0';
            if ( CreateDirectory ( SVDir, NULL )) fOK = TRUE; else
               { Erc = GetLastError ( );
                 if ( Erc == ERROR_ALREADY_EXISTS ) fOK = TRUE; else
                    { Depth++;
                      if ( Erc != ERROR_PATH_NOT_FOUND ) return Erc; } }
          } } while ( i1 > 0 && !fOK );

  for ( i1 = Depth; i1 > 0; i1-- )
     { SVDir [ strlen ( SVDir )] = '\\';
       if ( !CreateDirectory ( SVDir, NULL ))
          { Erc = GetLastError ( );
            if ( Erc != ERROR_PATH_NOT_FOUND ) return Erc; } }

  if ( CreateDirectory ( pb, NULL )) return 0;
  Erc = GetLastError ( );
  if ( Erc == ERROR_ALREADY_EXISTS )
		{
		return 0;
		}
  return Erc; 
	}

