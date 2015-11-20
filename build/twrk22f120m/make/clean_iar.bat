@echo OFF
SET CWD=%CD%

setlocal
cd %CWD%\..\..\..\mqx\build\make\bsp_twrk22f120m
cmd /c clean_iar nopause
cd %CWD%\..\..\..\mfs\build\make\mfs_twrk22f120m
cmd /c clean_iar nopause
cd %CWD%\..\..\..\mqx\build\make\psp_twrk22f120m
cmd /c clean_iar nopause
cd %CWD%\..\..\..\shell\build\make\shell_twrk22f120m
cmd /c clean_iar nopause

if not "%1" == "nopause" (pause)
