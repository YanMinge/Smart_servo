cd Objects
:: get current directory
set dir_cur=%cd% 

:: get current time. eg 20160620
set t=%date:~,4%
set t=%t%%date:~5,2%%date:~8,2%

:: get current app name.
for /f "delims=." %%a in ('dir /a-d/b *.hex') do set app=%%a

:: del old firmware in neurons_firmwave directory.
cd ..
cd .. 
cd ..
cd ..
cd neurons_firmware
if exist %app%*.hex del %app%*.hex

:: append date to the name of the firmwave.
cd %dir_cur%
for /f "delims=" %%a in ('dir /a-d/b *.hex') do ren "%%~a" "%%~na_%t%%%~xa"

:: copy new firmware to the directroy neurons_firmwave.
copy /Y  *hex  ..\..\..\..\neurons_firmware

:: delete new firmware.
for /r %%i in (*.hex) do del *.hex