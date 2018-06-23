set platform=%1
set configuration=%2
set WORKDIR=sakura-%platform%-%configuration%
set WORKLOG=%WORKDIR%\Log
set OUTFILE=sakura-%platform%-%configuration%.zip

@rem cleanup for local testing
if exist "%OUTFILE%" (
	del %OUTFILE%
)
if exist "%WORKDIR%" (
	rmdir /s /q %WORKDIR%
)

mkdir %WORKDIR%
mkdir %WORKLOG%
copy %platform%\%configuration%\*.exe %WORKDIR%\
copy %platform%\%configuration%\*.dll %WORKDIR%\
copy %platform%\%configuration%\*.pdb %WORKDIR%\
copy msbuild-%platform%-%configuration%.log %WORKLOG%\

copy help\macro\macro.chm    %WORKDIR%\
copy help\plugin\plugin.chm  %WORKDIR%\
copy help\sakura\sakura.chm  %WORKDIR%\

7z a %OUTFILE%  -r %WORKDIR%
7z l %OUTFILE%

if exist %WORKDIR% (
	rmdir /s /q %WORKDIR%
)
