@echo off
python -c "import sphinx"
if errorlevel 1 (
	echo Please run 'pip install sphinx' in advance.
	exit /b 1
)

call "%~dp0help\sphinx\sakura\make.bat" htmlhelp
if errorlevel 1 (
	echo Please run 'pip install sphinx' in advance.
	exit /b 1
)

call "%~dp0tools\hhc\find-hhc.bat"
if "%CMD_HHC%" == "" (
	echo hhc.exe was not found.
	exit /b 1
)

set HHP_SAKURA=help\sphinx\sakura\build\htmlhelp\SAKURAEditordoc.hhp
"%CMD_HHC%" %HHP_SAKURA%
if not errorlevel 1 (
	echo error %HHP_SAKURA% errorlevel %errorlevel%
	"%CMD_HHC%" %HHP_SAKURA%
)
if not errorlevel 1 (
	echo error %HHP_SAKURA% errorlevel %errorlevel%
	exit /b 1
)

exit /b 0
