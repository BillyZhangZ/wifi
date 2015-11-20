@echo off

rem convert path to backslash format
set ROOTDIR=%1
set ROOTDIR=%ROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3



rem copy common files
IF NOT EXIST "%OUTPUTDIR%\" mkdir "%OUTPUTDIR%\"
copy "%ROOTDIR%\audio\include\cci_metadata.h"    "%OUTPUTDIR%\cci_metadata.h"/Y
copy "%ROOTDIR%\audio\include\codec_interface.h" "%OUTPUTDIR%\codec_interface.h"/Y
copy "%ROOTDIR%\audio\include\codec_interface_ap_unusedi.h"  "%OUTPUTDIR%\codec_interface_ap_unusedi.h"/Y
copy "%ROOTDIR%\audio\include\codec_interface_mqx_wrapper.h" "%OUTPUTDIR%\codec_interface_mqx_wrapper.h"/Y
copy "%ROOTDIR%\audio\include\codec_interface_public_api.h"  "%OUTPUTDIR%\codec_interface_public_api.h"/Y
copy "%ROOTDIR%\audio\include\codec_support.h"               "%OUTPUTDIR%\codec_support.h"/Y
copy "%ROOTDIR%\audio\include\error.h"                       "%OUTPUTDIR%\error.h"/Y
copy "%ROOTDIR%\audio\include\errordefs.h"                   "%OUTPUTDIR%\errordefs.h"/Y
copy "%ROOTDIR%\audio\include\types.h"                       "%OUTPUTDIR%\types.h"/Y
copy "%ROOTDIR%\audio\lib\iar\codec_metadata.a"              "%OUTPUTDIR%\codec_metadata.a"/Y
copy "%ROOTDIR%\audio\lib\iar\flacdec_lib.a"                 "%OUTPUTDIR%\flacdec_lib.a"/Y
copy "%ROOTDIR%\audio\lib\iar\apedec_lib.a"                  "%OUTPUTDIR%\apedec_lib.a"/Y
copy "%ROOTDIR%\audio\lib\iar\mp3dec_lib.a"                  "%OUTPUTDIR%\mp3dec_lib.a"/Y
copy "%ROOTDIR%\audio\lib\iar\wavdec_lib.a"                  "%OUTPUTDIR%\wavdec_lib.a"/Y
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
