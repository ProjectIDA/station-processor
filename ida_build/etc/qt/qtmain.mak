# Microsoft Developer Studio Generated NMAKE File, Based on qtmain.dsp
!IF "$(CFG)" == ""
CFG=qtmain - Win32 Release
!MESSAGE No configuration specified. Defaulting to qtmain - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "qtmain - Win32 Release" && "$(CFG)" != "qtmain - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qtmain.mak" CFG="qtmain - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qtmain - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "qtmain - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "qtmain - Win32 Release"

OUTDIR=$(QTDIR)\lib
INTDIR=tmp\obj\release_mt_shared

ALL : "..\lib\qtmain.lib"


CLEAN :
	-@erase "$(INTDIR)\qtmain_win.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\lib\qtmain.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /O1 /I "tmp" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(QTDIR)\lib/qtmain.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"C:\Qt\3.3.4\lib\qtmain.lib" 
LIB32_OBJS= \
	"$(INTDIR)\qtmain_win.obj"

"..\lib\qtmain.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "qtmain - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\lib\qtmain.lib"


CLEAN :
	-@erase "$(INTDIR)\qtmain_win.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\lib\qtmain.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_LIB" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\qtmain.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"C:\Qt\3.3.4\lib\qtmain.lib" 
LIB32_OBJS= \
	"$(INTDIR)\qtmain_win.obj"

"..\lib\qtmain.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("qtmain.dep")
!INCLUDE "qtmain.dep"
!ELSE 
!MESSAGE Warning: cannot find "qtmain.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "qtmain - Win32 Release" || "$(CFG)" == "qtmain - Win32 Debug"
SOURCE=kernel\qtmain_win.cpp

!IF  "$(CFG)" == "qtmain - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "tmp" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /D "QT_NO_DEBUG" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD -Zm200 /c 

"$(INTDIR)\qtmain_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "qtmain - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Zi /Od /I "tmp" /I "$(QTDIR)\include" /I "." /I "tmp\moc\release_mt_shared" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_LIB" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ -Zm200 /c 

"$(INTDIR)\qtmain_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

