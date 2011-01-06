#include <windows.h>


int WINAPI WinMain(
  HINSTANCE hInstance,  // handle to current instance
  HINSTANCE hPrevInstance,  // handle to previous instance
  LPSTR lpCmdLine,      // pointer to command line
  int nCmdShow          // show state of window
)
 
	{
	BOOL bStart;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	char cPath[4096], cCurrentDirectory[4096];
	char drive[8];
	char dir[4096];


   

	GetPrivateProfileString(
		"autorun",
		"open",
		"",  
		cPath,
		sizeof(cPath),
		"winidadmx.ini"
	);

	if(*cPath==0) return 0;

   _splitpath( cPath, drive, dir, NULL, NULL );
	
   lstrcpy(cCurrentDirectory, drive);
   lstrcat(cCurrentDirectory, dir);

   _splitpath( __argv[0], drive, NULL, NULL, NULL );


   lstrcat(cPath, " /CD=");
   lstrcat(cPath,drive);

    ZeroMemory( &si, sizeof(si) );
    ZeroMemory( &pi, sizeof(pi) );
	
    si.cb = sizeof(si);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_NORMAL;


	bStart=CreateProcess(NULL,
		cPath,  // pointer to command line string
		NULL,  // process security attributes
		NULL,   // thread security attributes
		FALSE,  // handle inheritance flag
		CREATE_NEW_CONSOLE , // creation flags
		NULL,  // pointer to new environment block
		cCurrentDirectory,   // pointer to current directory name
		&si,  // pointer to STARTUPINFO
		&pi  // pointer to PROCESS_INFORMATION
	);
	

	return 0;
	}