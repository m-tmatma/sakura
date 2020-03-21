set CLANG_TIDY=C:\Program Files\LLVM\bin\clang-tidy.exe
set OPTIONS=-checks=-readability-braces-around-statements

@rem "%CLANG_TIDY%" -style=Microsoft -dump-config > .clang-format

git checkout                    sakura_core\*.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CWriteManager.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CAutoReloadAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CAutoSaveAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CBackupAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CCodeChecker.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CDataProfile.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CDicMgr.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CEditApp.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CEol.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CFileExt.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CGrepAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CHokanMgr.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CKeyWordSetMgr.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CLoadAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CMarkMgr.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\COpe.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\COpeBlk.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\COpeBuf.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CProfile.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CPropertyManager.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CReadManager.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CRegexKeyword.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CSaveAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CSearchAgent.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CSelectLang.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\CSortedTagJumpList.cpp
"%CLANG_TIDY%"  %OPTIONS% sakura_core\cmd\CViewCommander.cpp
