@echo off

rem convert path to backslash format
set ROOTDIR=%1
set ROOTDIR=%ROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3



rem copy common files
IF NOT EXIST "%OUTPUTDIR%\" mkdir "%OUTPUTDIR%\"
copy "%ROOTDIR%\shell\source\include\sh_mfs.h" "%OUTPUTDIR%\sh_mfs.h" /Y
copy "%ROOTDIR%\shell\source\include\sh_enet.h" "%OUTPUTDIR%\sh_enet.h" /Y
copy "%ROOTDIR%\shell\source\include\sh_rtcs.h" "%OUTPUTDIR%\sh_rtcs.h" /Y
copy "%ROOTDIR%\shell\source\include\shell.h" "%OUTPUTDIR%\shell.h" /Y


goto tool_%TOOL%

rem iar files
:tool_iar
goto copy_end

rem uv4 files
:tool_uv4
goto copy_end

rem gcc_arm files
:tool_gcc_arm
goto copy_end

:copy_end

