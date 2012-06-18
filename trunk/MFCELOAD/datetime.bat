@ECHO OFF
@TITLE Backing up source safe databases
FOR /F "tokens=2-4 delims=/ " %%i IN ('date /t') DO SET DATE=%%i-%%j-%%k
FOR /F "tokens=1-3 delims=: " %%i IN ('time /t') DO SET TIME=%%i-%%j-%%k
SET DATETIME=%DATE%-%TIME%
