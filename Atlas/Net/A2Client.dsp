# Microsoft Developer Studio Project File - Name="A2Client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=A2Client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "A2Client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "A2Client.mak" CFG="A2Client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "A2Client - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "A2Client - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "A2Client - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "A2Client - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "A2Client - Win32 Release"
# Name "A2Client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\libAtlas2\AtlasClient.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasCompressor.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasObject.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPackedAsciiDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPackedAsciiEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPackedAsciiProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasProtocolDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasProtocolEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPython.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasServer.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasTCPSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasXMLDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasXMLEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasXMLProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\CyphesisClient.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\libAtlas2\AtlasClient.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasCodec.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasCompressor.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasDebug.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasObject.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPackedAsciiDecoder.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPackedAsciiEncoder.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPackedAsciiProtocol.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasProtocol.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasProtocolDecoder.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasProtocolEncoder.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasPython.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasServer.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasSocket.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasTCPSocket.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasXMLDecoder.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasXMLEncoder.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\AtlasXMLProtocol.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\CodecBench.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\CopyTest.h
# End Source File
# Begin Source File

SOURCE=.\CyphesisClient.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\EchoTest.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\PackedAsciiTest.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\SocketTest.h
# End Source File
# Begin Source File

SOURCE=.\libAtlas2\XMLTest.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
