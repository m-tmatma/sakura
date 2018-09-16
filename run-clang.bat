set CLANG_FORMAT=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\vcpackages\clang-format.exe
for /r %%i in (*.cpp) do (
	git checkout -f                 %%i
	"%CLANG_FORMAT%" -style=file -i %%i
)
for /r %%i in (*.h) do (
	git checkout -f                 %%i
	"%CLANG_FORMAT%" -style=file -i %%i
)
