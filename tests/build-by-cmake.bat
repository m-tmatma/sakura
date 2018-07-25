cd /d %~dp0

if not exist googletest (
    git submodule init
    git submodule update
)

set BUILDDIR=build

if exist %BUILDDIR% rmdir /s /q %BUILDDIR%
mkdir %BUILDDIR%

cmake -D BUILD_SHARED_LIBS=0 -B%BUILDDIR% -H. || exit /b 1

cmake --build %BUILDDIR%  --config Debug   || exit /b 1
cmake --build %BUILDDIR%  --config Release || exit /b 1

exit /b 0
