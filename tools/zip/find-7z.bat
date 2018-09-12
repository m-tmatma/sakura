@rem see readme.md
@echo off
if "%FORCE_POWERSHELL_ZIP%" == "1" (
	set REAL_CMD_7Z=
	exit /b 0
)

if not "%CMD_7Z%" == "" (
	set REAL_CMD_7Z=%CMD_7Z%
	exit /b 0
)
set PATH_7Z_1=
set PATH_7Z_2=
set PATH_7Z_3=

if not "%ProgramFiles%"      == "" set "PATH_7Z_1=%ProgramFiles%\7-Zip\7z.exe"
if not "%ProgramFiles(x86)%" == "" set "PATH_7Z_2=%ProgramFiles(x86)%\7-Zip\7z.exe"
if not "%ProgramW6432%"      == "" set "PATH_7Z_3=%ProgramW6432%\7-Zip\7z.exe"

set RESULT_PATH_7Z_0=--
set RESULT_PATH_7Z_1=--
set RESULT_PATH_7Z_2=--
set RESULT_PATH_7Z_3=--

where 7z 1>nul 2>&1
if "%ERRORLEVEL%" == "0" (
	set RESULT_PATH_7Z_0=OK
	set REAL_CMD_7Z=7z
) else if exist "%PATH_7Z_1%" (
	set RESULT_PATH_7Z_1=OK
	set "REAL_CMD_7Z=%PATH_7Z_1%"
) else if exist "%PATH_7Z_2%" (
	set RESULT_PATH_7Z_2=OK
	set "REAL_CMD_7Z=%PATH_7Z_2%"
) else if exist "%PATH_7Z_3%" (
	set RESULT_PATH_7Z_3=OK
	set "REAL_CMD_7Z=%PATH_7Z_3%"
)

@echo %RESULT_PATH_7Z_0% 7z.exe
@echo %RESULT_PATH_7Z_1% %PATH_7Z_1%
@echo %RESULT_PATH_7Z_2% %PATH_7Z_2%
@echo %RESULT_PATH_7Z_3% %PATH_7Z_3%
@echo.
@echo CMD_7Z "%REAL_CMD_7Z%"
@echo.
