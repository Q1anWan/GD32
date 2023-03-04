set BAT_DIR=%~dp0
echo %BAT_DIR%

::Keil的安装路径
set KEIL_DIR="D:\ARM_Soft\Keil_v5"

::J-Link的安装路径
set JLINK_DIR="C:\Program Files (x86)\SEGGER\JLink"

set SOFT_ROOT_DIR=%BAT_DIR%

echo "Start Copy FLM..."

copy /y "%SOFT_ROOT_DIR%\GD32E230_W25QXX.FLM" "%KEIL_DIR%\ARM\Flash\"

copy /y "%SOFT_ROOT_DIR%\GD32E230_W25QXX.FLM" %JLINK_DIR%\Devices\

echo "Copy done!"

pause