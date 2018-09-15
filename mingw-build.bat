set CONF=%1
set MINGW=%2
set PATH=%MINGW%;%PATH%

if "%CONF%" == "Debug" (
	set MYDEFINES=-D_DEBUG
	set MYCFLAGS=-g -O0
	set MYLIBS=-g
) else if "%CONF%" == "Release" (
	set MYDEFINES=-D_DEBUG
	set MYLIBS=-s
)

cd /d %~dp0

pushd sakura_core

@rem mingw32-make githash stdafx Funccode_enum.h Funccode_define.h
mingw32-make clean
mingw32-make MYDEFINES="%MYDEFINES%" MYCFLAGS="%MYCFLAGS%" MYLIBS="%MYLIBS%" githash stdafx Funccode_enum.h Funccode_define.h
mingw32-make MYDEFINES="%MYDEFINES%" MYCFLAGS="%MYCFLAGS%" MYLIBS="%MYLIBS%" -j4
popd
