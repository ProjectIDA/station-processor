# Microsoft Developer Studio Project File - Name="isid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=isid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "isid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "isid.mak" CFG="isid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "isid - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "isid - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "isid - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ..\..\lib\iacp\Release\iacp.lib ..\..\lib\isi\Release\isi.lib ..\..\lib\util\Release\util.lib ..\..\lib\logio\Release\logio.lib ..\..\lib\nrts\Release\nrts.lib ..\..\lib\ida10\Release\ida10.lib ..\..\lib\ida\Release\ida.lib ..\..\lib\dbio\Release\dbio.lib ..\..\lib\cssio\Release\cssio.lib ..\..\lib\msgq\Release\msgq.lib ..\..\lib\win32\Release\win32.lib ..\..\lib\zlib\zdll.lib ..\..\lib\isidl\Release\isidl.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Release/isidb.lib /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "isid - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ..\..\lib\iacp\Debug\iacp.lib ..\..\lib\isi\Debug\isi.lib ..\..\lib\util\Debug\util.lib ..\..\lib\logio\Debug\logio.lib ..\..\lib\nrts\Debug\nrts.lib ..\..\lib\ida10\Debug\ida10.lib ..\..\lib\ida\Debug\ida.lib ..\..\lib\dbio\Debug\dbio.lib ..\..\lib\cssio\Debug\cssio.lib ..\..\lib\msgq\Debug\msgq.lib ..\..\lib\win32\Debug\win32.lib ..\..\lib\isidl\Debug\isidl.lib ..\..\lib\zlib\zdll.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Debug/isidb.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "isid - Win32 Release"
# Name "isid - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\bin\isid\client.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\convert.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\copy.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\die.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\hbeat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\isi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\nrts.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\par.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\ReleaseNotes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\report.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\respond.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\seqno.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\service.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\signals.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\status.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\twind.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\iacp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isid\isid.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
