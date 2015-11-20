@echo OFF
SET CWD=%CD%

setlocal
cd %CWD%\..\..\..\mqx\build\make\bsp_twrk22f120m
cmd /c build_gcc_arm nopause
cd %CWD%\..\..\..\mfs\build\make\mfs_twrk22f120m
cmd /c build_gcc_arm nopause
cd %CWD%\..\..\..\mqx\build\make\psp_twrk22f120m
cmd /c build_gcc_arm nopause
cd %CWD%\..\..\..\shell\build\make\shell_twrk22f120m
cmd /c build_gcc_arm nopause

if not "%1" == "nopause" (pause)
