# Microsoft Developer Studio Project File - Name="CarnegieMellonGraphics2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CarnegieMellonGraphics2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CarnegieMellonGraphics2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CarnegieMellonGraphics2.mak" CFG="CarnegieMellonGraphics2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CarnegieMellonGraphics2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CarnegieMellonGraphics2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CarnegieMellonGraphics2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\src" /I ".\include" /I ".\include\pthread" /I ".\include\gltt" /I ".\include\glut" /I ".\include\libjpeg" /I ".\include\libpng" /I ".\include\zlib" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CarnegieMellonGraphics2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\src" /I ".\include" /I ".\include\pthread" /I ".\include\gltt" /I ".\include\glut" /I ".\include\libjpeg" /I ".\include\libpng" /I ".\include\zlib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "CarnegieMellonGraphics2 - Win32 Release"
# Name "CarnegieMellonGraphics2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\Color.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Daemon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\DaemonCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\DaemonImp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Debugging.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\EditableImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\GLUTImp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\KeyboardEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\KeyModifiers.cpp
# End Source File
# Begin Source File

SOURCE=.\LeakWatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\MouseEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\NamedKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\PortMutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Style.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\TimerEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Transform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\WindowManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\CarnegieMellonGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Daemon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\DaemonCommand.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\DaemonImp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\Debugging.h
# End Source File
# Begin Source File

SOURCE=.\include\glut\glut.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\GLUTImp.h
# End Source File
# Begin Source File

SOURCE=.\include\libjpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\include\LeakWatcher.h
# End Source File
# Begin Source File

SOURCE=.\include\libpng\png.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\PortMutex.h
# End Source File
# Begin Source File

SOURCE=.\include\pthread\pthread.h
# End Source File
# Begin Source File

SOURCE=.\include\pthread\sched.h
# End Source File
# Begin Source File

SOURCE=.\include\pthread\semaphore.h
# End Source File
# Begin Source File

SOURCE=.\include\version.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\lib\pthread.lib
# End Source File
# Begin Source File

SOURCE=.\lib\gltt.lib
# End Source File
# Begin Source File

SOURCE=.\lib\glut32.lib
# End Source File
# Begin Source File

SOURCE=.\lib\libjpeg.lib
# End Source File
# Begin Source File

SOURCE=.\lib\libpng.lib
# End Source File
# End Target
# End Project
