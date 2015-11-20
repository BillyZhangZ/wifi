@echo OFF
set NOPAUSE=%1

mingw32-make TOOL=iar CONFIG=debug LOAD=intflash build -j3 
if errorlevel 1 (
set NOPAUSE=0
pause
)

mingw32-make TOOL=iar CONFIG=release LOAD=intflash build -j3 
if errorlevel 1 (
set NOPAUSE=0
pause
)

if not "%1" == "nopause" (pause)
