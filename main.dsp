# Microsoft Developer Studio Project File - Name="main" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=main - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "main.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "main.mak" CFG="main - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "main - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "main - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "main - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "main - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "FT2_BUILD_LIBRARY" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_LIB" /D "_OPEN_BINARY" /D "HAVE_IO_H" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /D "HAVE_BASETSD_H" /FD /GZ @"include.ini" /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "main - Win32 Release"
# Name "main - Win32 Debug"
# Begin Group "winbase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\winbase\main_display.c
# End Source File
# Begin Source File

SOURCE=.\winbase\win_display.c
# End Source File
# End Group
# Begin Group "libs"

# PROP Default_Filter ""
# Begin Group "lib_graphics"

# PROP Default_Filter ""
# Begin Group "lib_base_draw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_base_draw\basedraw.c
# End Source File
# End Group
# Begin Group "lib_base_img_handle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_base_img_handle\img_base_handle.c
# End Source File
# End Group
# Begin Group "lib_bmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_bmp\bmp.c
# End Source File
# End Group
# Begin Group "lib_giflib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\dev2gif.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\dgif_lib.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\egif_lib.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\getarg.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\gif_err.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\gif_font.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\gif_hash.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\gifalloc.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\qprintf.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_giflib\quantize.c
# End Source File
# End Group
# Begin Group "lib_jpeglib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\ansi2knr.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\cdjpeg.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\cjpeg.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\ckconfig.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\djpeg.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\example.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jaricom.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcapimin.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcapistd.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcarith.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jccoefct.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jccolor.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jchuff.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcinit.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcmainct.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcmarker.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcmaster.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcomapi.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcparam.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcprepct.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jcsample.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jctrans.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdapimin.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdapistd.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdarith.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdatadst.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdcolor.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdhuff.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdinput.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdmainct.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdmarker.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdmaster.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdmerge.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdpostct.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdsample.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jdtrans.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jerror.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jfdctint.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jidctflt.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jidctfst.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jidctint.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jmemansi.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jpegtran.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jquant1.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jquant2.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\jutils.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdbmp.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdcolmap.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdgif.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdjpgcom.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdppm.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdrle.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdswitch.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\rdtarga.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\transupp.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\wrbmp.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\wrgif.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\wrjpgcom.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\wrppm.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\wrrle.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_jpglib\wrtarga.c
# End Source File
# End Group
# Begin Group "lib_png"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\png.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngconf.h
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngget.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngread.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngset.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_graphics\lib_png\pngwutil.c
# End Source File
# End Group
# End Group
# Begin Group "lib_font"

# PROP Default_Filter ""
# Begin Group "lib_dot_font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_font\lib_dot_font\dot_font.c
# End Source File
# End Group
# Begin Group "lib_freetype"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\autofit\autofit.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\cff\cff.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftadvanc.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftbase.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftbbox.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftbitmap.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftglyph.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftinit.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftstroke.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\base\ftsystem.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\pshinter\pshinter.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\psnames\psnames.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\raster\raster.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\sfnt\sfnt.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\smooth\smooth.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_font\lib_freetype\ft2\src\truetype\truetype.c
# End Source File
# End Group
# End Group
# Begin Group "lib_other"

# PROP Default_Filter ""
# Begin Group "lib_zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\compress.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\infback.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\minigzip.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\libs\lib_other\lib_zlib\zutil.c
# End Source File
# End Group
# Begin Group "lib_fhandle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libs\lib_other\lib_fhandle\basefun.c
# End Source File
# End Group
# End Group
# End Group
# Begin Group "debug_code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\debug_code\trace.c
# End Source File
# End Group
# Begin Group "test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\test\test_ft2_main.c
# End Source File
# Begin Source File

SOURCE=.\test\test_gif_main.c
# End Source File
# Begin Source File

SOURCE=.\test\test_img_base_handle_main.c
# End Source File
# Begin Source File

SOURCE=.\test\test_jpeg_main.c
# End Source File
# Begin Source File

SOURCE=.\test\test_main.c
# End Source File
# Begin Source File

SOURCE=.\test\test_png_main.c
# End Source File
# Begin Source File

SOURCE=.\test\test_zlib_main.c
# End Source File
# End Group
# Begin Group "test_zzf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\test_zzf\test_zzf_main.c
# End Source File
# End Group
# End Target
# End Project
