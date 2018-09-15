set PLATFORM=%1
set CONFIFURATION=%2

if "%PLATFORM%" == "Win32" (
	set MINGW_PATH=C:\MinGW\bin
) else if "%PLATFORM%" == "x64" (
	set MINGW_PATH=C:\mingw-w64\x86_64-7.2.0-posix-seh-rt_v5-rev1
) else (
	@echo unsupported platform
	exit /b 1
)

call mingw-build.bat %CONFIFURATION% %MINGW_PATH%
