@rem https://clang.llvm.org/docs/ClangFormatStyleOptions.html
cd %~dp0
"C:\Program Files\LLVM\bin\clang-format.exe"  -dump-config -style=Microsoft > .clang-format
