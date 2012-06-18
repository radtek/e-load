call datetime.bat
call setvars.bat

MKDIR Result\%datetime%

REM copy dlls
COPY "..\..\Is_Tools\lib\IsGUI2008.dll" ".\ELOAD\Bin\IsGUI2008.dll"
COPY "..\..\Is_Tools\lib\IsUtil2008.dll" ".\ELOAD\Bin\IsUtil2008.dll"

%AUTOBUILDINC% -i .\ELOAD\ELOAD.rc
%VS2008% /rebuild Release "./ELOAD.sln" /out "./Result/%datetime%/ELOAD.log"

REM create ini file
copy nul Result\%datetime%\WorkSpace.log
echo.[WorkSpace] >> Result\%datetime%\WorkSpace.log
echo.Count=1 >> Result\%datetime%\WorkSpace.log
echo.WorkSpace0=ELOAD,.\Result\%datetime%\ELOAD.log >> Result\%datetime%\WorkSpace.log

%LOGVIEWER% .\Result\%datetime%\WorkSpace.log

REM build setup file
pushd ".\ELOAD\Bin"

postbuild.bat
if %ERRORLEVEL% NEQ 0 goto failure
:failure
	msg * !!!ERROR!!!

popd
cd ..\..\
