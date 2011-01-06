# Microsoft Developer Studio Generated NMAKE File, Based on isitapqt.dsp
!IF "$(CFG)" == ""
CFG=isitapqt - Win32 Debug
!MESSAGE No configuration specified. Defaulting to isitapqt - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "isitapqt - Win32 Release" && "$(CFG)" != "isitapqt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "isitapqt.mak" CFG="isitapqt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "isitapqt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "isitapqt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "isitapqt - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\isitapqt.exe"


CLEAN :
	-@erase "$(INTDIR)\configDlg.obj"
	-@erase "$(INTDIR)\DateTimeEdit.obj"
	-@erase "$(INTDIR)\GettingDataThread.obj"
	-@erase "$(INTDIR)\ISI_GENERIC_TS_Packet.obj"
	-@erase "$(INTDIR)\isitap_globals.obj"
	-@erase "$(INTDIR)\isitap_time.obj"
	-@erase "$(INTDIR)\isitapdlg.obj"
	-@erase "$(INTDIR)\isiTapResults.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\moc_configDlg.obj"
	-@erase "$(INTDIR)\moc_isitapdlg.obj"
	-@erase "$(INTDIR)\moc_isiTapResults.obj"
	-@erase "$(INTDIR)\nrtschaninfo.obj"
	-@erase "$(INTDIR)\nrtsinfo.obj"
	-@erase "$(INTDIR)\nrtsstainfo.obj"
	-@erase "$(INTDIR)\ReadFileCache.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WFDISC.OBJ"
	-@erase "$(OUTDIR)\isitapqt.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\isitapqt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS="qt-mt334.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" delayimp.lib "..\..\lib\iacp\Release\iacp.lib" "..\..\lib\isi\Release\isi.lib" "..\..\lib\util\Release\util.lib" "..\..\lib\logio\Release\logio.lib" "..\..\lib\cssio\Release\cssio.lib" /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\isitapqt.pdb" /machine:IX86 /out:"$(OUTDIR)\isitapqt.exe" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll 
LINK32_OBJS= \
	"$(INTDIR)\configDlg.obj" \
	"$(INTDIR)\DateTimeEdit.obj" \
	"$(INTDIR)\GettingDataThread.obj" \
	"$(INTDIR)\ISI_GENERIC_TS_Packet.obj" \
	"$(INTDIR)\isitap_globals.obj" \
	"$(INTDIR)\isitap_time.obj" \
	"$(INTDIR)\isitapdlg.obj" \
	"$(INTDIR)\isiTapResults.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\nrtschaninfo.obj" \
	"$(INTDIR)\nrtsinfo.obj" \
	"$(INTDIR)\nrtsstainfo.obj" \
	"$(INTDIR)\ReadFileCache.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\WFDISC.OBJ" \
	"$(INTDIR)\moc_configDlg.obj" \
	"$(INTDIR)\moc_isitapdlg.obj" \
	"$(INTDIR)\moc_isiTapResults.obj"

"$(OUTDIR)\isitapqt.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\isitapqt.exe"


CLEAN :
	-@erase "$(INTDIR)\configDlg.obj"
	-@erase "$(INTDIR)\DateTimeEdit.obj"
	-@erase "$(INTDIR)\GettingDataThread.obj"
	-@erase "$(INTDIR)\ISI_GENERIC_TS_Packet.obj"
	-@erase "$(INTDIR)\isitap_globals.obj"
	-@erase "$(INTDIR)\isitap_time.obj"
	-@erase "$(INTDIR)\isitapdlg.obj"
	-@erase "$(INTDIR)\isiTapResults.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\moc_configDlg.obj"
	-@erase "$(INTDIR)\moc_isitapdlg.obj"
	-@erase "$(INTDIR)\moc_isiTapResults.obj"
	-@erase "$(INTDIR)\nrtschaninfo.obj"
	-@erase "$(INTDIR)\nrtsinfo.obj"
	-@erase "$(INTDIR)\nrtsstainfo.obj"
	-@erase "$(INTDIR)\ReadFileCache.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WFDISC.OBJ"
	-@erase "$(OUTDIR)\isitapqt.exe"
	-@erase "$(OUTDIR)\isitapqt.ilk"
	-@erase "$(OUTDIR)\isitapqt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\isitapqt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS="qt-mt334.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" "..\..\lib\iacp\Debug\iacp.lib" "..\..\lib\isi\Debug\isi.lib" "..\..\lib\util\Debug\util.lib" "..\..\lib\logio\Debug\logio.lib" "..\..\lib\cssio\Debug\cssio.lib" /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\isitapqt.pdb" /debug /machine:IX86 /out:"$(OUTDIR)\isitapqt.exe" /pdbtype:sept /libpath:"$(QTDIR)\lib" 
LINK32_OBJS= \
	"$(INTDIR)\configDlg.obj" \
	"$(INTDIR)\DateTimeEdit.obj" \
	"$(INTDIR)\GettingDataThread.obj" \
	"$(INTDIR)\ISI_GENERIC_TS_Packet.obj" \
	"$(INTDIR)\isitap_globals.obj" \
	"$(INTDIR)\isitap_time.obj" \
	"$(INTDIR)\isitapdlg.obj" \
	"$(INTDIR)\isiTapResults.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\nrtschaninfo.obj" \
	"$(INTDIR)\nrtsinfo.obj" \
	"$(INTDIR)\nrtsstainfo.obj" \
	"$(INTDIR)\ReadFileCache.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\WFDISC.OBJ" \
	"$(INTDIR)\moc_configDlg.obj" \
	"$(INTDIR)\moc_isitapdlg.obj" \
	"$(INTDIR)\moc_isiTapResults.obj"

"$(OUTDIR)\isitapqt.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("isitapqt.dep")
!INCLUDE "isitapqt.dep"
!ELSE 
!MESSAGE Warning: cannot find "isitapqt.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "isitapqt - Win32 Release" || "$(CFG)" == "isitapqt - Win32 Debug"
SOURCE=..\..\..\bin\isiTapqt\configDlg.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\configDlg.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\configDlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\configDlg.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\configDlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\DateTimeEdit.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\DateTimeEdit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\DateTimeEdit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\GettingDataThread.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\GettingDataThread.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h" "..\..\..\bin\isiTapqt\isitapdlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\GettingDataThread.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h" "..\..\..\bin\isiTapqt\isitapdlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\ISI_GENERIC_TS_Packet.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\ISI_GENERIC_TS_Packet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\ISI_GENERIC_TS_Packet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\isitap_globals.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\isitap_globals.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\isitap_globals.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\isitap_time.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\isitap_time.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\isitap_time.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\isitapdlg.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\isitapdlg.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\configDlg.h" "..\..\..\bin\isiTapqt\isiTapResults.h" "..\..\..\bin\isiTapqt\isitapdlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\isitapdlg.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\configDlg.h" "..\..\..\bin\isiTapqt\isiTapResults.h" "..\..\..\bin\isiTapqt\isitapdlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\isiTapResults.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\isiTapResults.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\isiTapResults.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\main.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isitapdlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isitapdlg.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\nrtschaninfo.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\nrtschaninfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\nrtschaninfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\nrtsinfo.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\nrtsinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\nrtsinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\nrtsstainfo.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\nrtsstainfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\nrtsstainfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\ReadFileCache.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\ReadFileCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\ReadFileCache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\thread.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\WFDISC.CPP

!IF  "$(CFG)" == "isitapqt - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\WFDISC.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\WFDISC.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\configDlg.h

!IF  "$(CFG)" == "isitapqt - Win32 Release"

InputPath=..\..\..\bin\isiTapqt\configDlg.h

".\moc_configDlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc  "..\..\..\bin\isiTapqt\configDlg.h" -o moc_configDlg.cpp
<< 
	

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

InputPath=..\..\..\bin\isiTapqt\configDlg.h

".\moc_configDlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\configDlg.h -o moc_configDlg.cpp
<< 
	

!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\isitapdlg.h

!IF  "$(CFG)" == "isitapqt - Win32 Release"

InputPath=..\..\..\bin\isiTapqt\isitapdlg.h

".\moc_isitapdlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isitapdlg.h -o moc_isitapdlg.cpp
<< 
	

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

InputPath=..\..\..\bin\isiTapqt\isitapdlg.h

".\moc_isitapdlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isitapdlg.h -o moc_isitapdlg.cpp
<< 
	

!ENDIF 

SOURCE=..\..\..\bin\isiTapqt\isiTapResults.h

!IF  "$(CFG)" == "isitapqt - Win32 Release"

InputPath=..\..\..\bin\isiTapqt\isiTapResults.h

".\moc_isiTapResults.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isiTapResults.h -o moc_isiTapResults.cpp
<< 
	

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

InputPath=..\..\..\bin\isiTapqt\isiTapResults.h

".\moc_isiTapResults.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isiTapResults.h -o moc_isiTapResults.cpp
<< 
	

!ENDIF 

SOURCE=.\moc_configDlg.cpp

"$(INTDIR)\moc_configDlg.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\configDlg.h"


SOURCE=.\moc_isitapdlg.cpp

"$(INTDIR)\moc_isitapdlg.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isitapdlg.h"


SOURCE=.\moc_isiTapResults.cpp

"$(INTDIR)\moc_isiTapResults.obj" : $(SOURCE) "$(INTDIR)" "..\..\..\bin\isiTapqt\isiTapResults.h"



!ENDIF 

