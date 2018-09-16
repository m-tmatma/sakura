set CLANG_FORMAT=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\vcpackages\clang-format.exe
for %%i in (sakura_core\*.cpp) do (
	git checkout -f                 %%i
	"%CLANG_FORMAT%" -style=file -i %%i
)
for %%i in (sakura_core\*.h) do (
	git checkout -f                 %%i
	"%CLANG_FORMAT%" -style=file -i %%i
)
