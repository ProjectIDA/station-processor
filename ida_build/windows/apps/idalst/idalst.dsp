# Microsoft Developer Studio Project File - Name="idalst" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=idalst - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "idalst.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "idalst.mak" CFG="idalst - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "idalst - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "idalst - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "idalst - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ../../lib/dbio/Release/dbio.lib ../../lib/util/Release/util.lib ../../lib/ida/Release/ida.lib ../../lib/miow/Release/miow.lib ../../lib/logio/Release/logio.lib ../../lib/win32/Release/win32.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Release/isidb.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ../../lib/dbio/Debug/dbio.lib ../../lib/util/Debug/util.lib ../../lib/ida/Debug/ida.lib ../../lib/miow/Debug/miow.lib  ../../lib/logio/Debug/logio.lib ../../lib/win32/Debug/win32.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Debug/isidb.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "idalst - Win32 Release"
# Name "idalst - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\bin\idalst\chktrut.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\ckdupadj.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\ckdupdat.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\ckextjmp.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\cklastw.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\cktinc.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\cktoff.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\cktqual.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\doarceof.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\docalib.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\doconfig.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\dodata.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\dodmp.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\doevent.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\doident.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\dolabel.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\dolog.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\donotdef.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\dosrec.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\exitcode.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\getdat.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\help.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\init.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\main.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pbadtag.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pcalib.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pconfig.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pcorrupt.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pdata.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pdunmap.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pevent.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pident.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pinput.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\plog.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\poldcnf.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\psrec.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\pttrip.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\ReleaseNotes.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\idalst\summary.c

!IF  "$(CFG)" == "idalst - Win32 Release"

!ELSEIF  "$(CFG)" == "idalst - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
