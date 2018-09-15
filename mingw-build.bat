set CONF=%1
set MINGW=%2
set MINGW32MAKE=%MINGW%\mingw32-make.exe

if "%CONF%" == "Debug" (
	set MYDEFINES=-D_DEBUG
	set MYCFLAGS=-g -O0
	set MYLIBS=-g
) else if "%CONF%" == "Release" (
	set MYDEFINES=-D_DEBUG
	set MYLIBS=-s
)

if not exist "%MINGW32MAKE%" (
	@echo %MINGW32MAKE% does not exist. skip.
	exit /b 0
)
cd /d %~dp0

pushd sakura_core

@rem mingw32-make githash stdafx Funccode_enum.h Funccode_define.h
%MINGW32MAKE% clean
%MINGW32MAKE% MYDEFINES="%MYDEFINES%" MYCFLAGS="%MYCFLAGS%" MYLIBS="%MYLIBS%" githash stdafx Funccode_enum.h Funccode_define.h
%MINGW32MAKE% MYDEFINES="%MYDEFINES%" MYCFLAGS="%MYCFLAGS%" MYLIBS="%MYLIBS%" -j4
popd
