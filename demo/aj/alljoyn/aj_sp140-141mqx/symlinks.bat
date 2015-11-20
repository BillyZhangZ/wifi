@ECHO OFF

if not exist %1\components\rel\wlanhost.ioe\3.2\baseline (
   echo path to "components\rel\wlanhost.ioe\3.2\baseline" must be passed in as the first parameter
   pause
   exit /B
)

set branch=%1\components\rel\wlanhost.ioe\3.2\baseline

set MQX_ROOT=%branch%\MQX_4.0.2
set IOT_ROOT=%branch%\IOT_Common\IOT_Driver

set atheros_wifi=%MQX_ROOT%\mqx\source\io\enet\atheros_wifi
set phy_dir=%MQX_ROOT%\mqx\source\io\enet\phy

rem Delete potentially stale driver folder
del /f /s /q  %atheros_wifi%
del /f /s /q  %phy_dir%

rem Cleanup old links
rmdir %atheros_wifi%
rmdir %phy_dir%
rmdir %~dp0config
rmdir %~dp0lib
rmdir %~dp0mqx
rmdir %~dp0rtcs

rem Create driver folders
mkdir %atheros_wifi%
mkdir %phy_dir%

rem MQX 4.0.2
mklink /d %~dp0config    %MQX_ROOT%\config
mklink /d %~dp0lib       %MQX_ROOT%\lib
mklink /d %~dp0mqx       %MQX_ROOT%\mqx
mklink /d %~dp0rtcs      %MQX_ROOT%\rtcs

rem Set access right on the iar project directories
icacls %MQX_ROOT%\mqx\build\iar\psp_sp140_kf /t /grant everyone:(OI)(CI)F
icacls %MQX_ROOT%\mqx\build\iar\psp_sp140_kf /t /grant everyone:RWD
attrib /S -r %MQX_ROOT%\mqx\build\iar\psp_sp140_kf\*
icacls %MQX_ROOT%\mqx\build\iar\bsp_sp140_kf /t /grant everyone:(OI)(CI)F
icacls %MQX_ROOT%\mqx\build\iar\bsp_sp140_kf /t /grant everyone:RWD
attrib /S -r %MQX_ROOT%\mqx\build\iar\bsp_sp140_kf\*

rem Link Atheros WiFI driver
mklink /d %atheros_wifi%\common_src              %IOT_ROOT%\atheros_wifi_common\common_src
mklink /d %atheros_wifi%\include                 %IOT_ROOT%\atheros_wifi_common\include
mklink /d %atheros_wifi%\custom_src              %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\custom_src

rem Top level header files
mklink /H %atheros_wifi%\atheros_wifi.h          %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\atheros_wifi.h
mklink /H %atheros_wifi%\atheros_wifi_api.h      %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\atheros_wifi_api.h

mklink /H %atheros_wifi%\atheros_wifi_internal.h %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\atheros_wifi_internal.h
mklink /H %atheros_wifi%\atheros_stack_offload.h %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\atheros_stack_offload.h
mklink /H %atheros_wifi%\custom_stack_offload.h  %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\custom_src\stack_custom\custom_stack_offload.h

mklink /H %phy_dir%\atheros_phy.c                %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\phy\atheros_phy.c
mklink /H %phy_dir%\atheros_phy.h                %IOT_ROOT%\atheros_wifi_custom\mqx40_custom\phy\atheros_phy.h

@ECHO ON
