# Microsoft Developer Studio Project File - Name="i10dmxw" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=i10dmxw - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "i10dmxw.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "i10dmxw.mak" CFG="i10dmxw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "i10dmxw - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "i10dmxw - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ../../lib/dbio/Release/dbio.lib ../../lib/util/Release/util.lib ../../lib/cssio/Release/cssio.lib ../../lib/ida10/Release/ida10.lib ../../lib/miow/Release/miow.lib  ../../lib/logio/Release/logio.lib ../../lib/win32/Release/win32.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Release/isidb.lib /nologo /subsystem:console /machine:I386 /out:"Release/i10dmx.exe"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ../../lib/dbio/Debug/dbio.lib ../../lib/util/Debug/util.lib ../../lib/cssio/Debug/cssio.lib ../../lib/ida10/Debug/ida10.lib ../../lib/miow/Debug/miow.lib  ../../lib/win32/Debug/win32.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Debug/isidb.lib ../../lib/logio/Debug/logio.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/i10dmx.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "i10dmxw - Win32 Release"
# Name "i10dmxw - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\exitcode.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\help.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\init.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\main.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\msglog.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\names.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\rdrec.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\ReleaseNotes.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\tsrec_ok.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\ttag.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\unpackii.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\unpacklm.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\wfdisc.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\i10dmx\wrtdat.c

!IF  "$(CFG)" == "i10dmxw - Win32 Release"

!ELSEIF  "$(CFG)" == "i10dmxw - Win32 Debug"

# ADD CPP /MDd

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
