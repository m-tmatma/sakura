set CLANG_FORMAT=C:\Program Files\LLVM\bin\clang-format.exe

@rem "%CLANG_FORMAT%" -style=Microsoft -dump-config > .clang-format

git checkout                    sakura_core\*.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CWriteManager.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CAutoReloadAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CAutoSaveAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CBackupAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CCodeChecker.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CDataProfile.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CDicMgr.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CEditApp.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CEol.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CFileExt.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CGrepAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CHokanMgr.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CKeyWordSetMgr.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CLoadAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CMarkMgr.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\COpe.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\COpeBlk.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\COpeBuf.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CProfile.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CPropertyManager.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CReadManager.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CRegexKeyword.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CSaveAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CSearchAgent.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CSelectLang.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\CSortedTagJumpList.cpp
"%CLANG_FORMAT%" -style=file -i sakura_core\cmd\CViewCommander.cpp
