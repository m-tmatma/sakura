@echo off
set platform=%1
set configuration=%2
set TEST_LAUNCHED=0
set ERROR_RESULT=0

set BUILDDIR=build\%platform%
set BINARY_DIR=%~dp0%BUILDDIR%\unittests\%platform%\%configuration%

pushd %BINARY_DIR%
for /r %%i in (tests*.exe) do (
	set TEST_LAUNCHED=1

	call :RunTest %%i %%i-googletest-%platform%-%configuration%.xml || set ERROR_RESULT=1
)
popd

if "%TEST_LAUNCHED%" == "0" (
	@echo ERROR: no tests are available.
	exit /b 1
)

if "%ERROR_RESULT%" == "1" (
	@echo ERROR
	exit /b 1
)
exit /b 0

:RunTest
set EXEPATH=%1
set XMLPATH=%2

@echo %EXEPATH% --gtest_output=xml:%XMLPATH%
      %EXEPATH% --gtest_output=xml:%XMLPATH%                   || set TEMP_RESULT=1
exit /b %TEMP_RESULT%
