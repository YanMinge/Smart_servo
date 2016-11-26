for /r %%i in (*.bat) do (
if %%i NEQ %cd%\keilkill.bat (
if %%i NEQ %cd%\batch_copy.bat (
if %%i NEQ %cd%\copy_hex.bat (
copy /Y %cd%\copy_hex.bat  %%i
)
)
)
)