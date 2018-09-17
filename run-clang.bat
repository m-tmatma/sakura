@echo off
call :run-format-recursive              sakura_core
exit /b 0

:run-format-recursive
	pushd %1
	for /r %%i in (*.cpp) do (
		call :run-format                %%i
	)
	for /r %%i in (*.h) do (
		call :run-format                %%i
	)
	popd
	exit /b 0

:run-format
	pushd %~dp0
	set CLANG_FORMAT=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\vcpackages\clang-format.exe
	git checkout -f                %1
	@echo "%CLANG_FORMAT%" -style=file -i %1
	      "%CLANG_FORMAT%" -style=file -i %1
	popd
	exit /b 0
