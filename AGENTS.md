# Agent / contributor notes

## 64bit 完全対応・メモリ内部表現（64bit 変数）

以下はコード調査に基づくタスク一覧。優先度は概ね上から。

### P0: メモリバッファ層

- [x] `CMemory`: `m_nRawLen` / `m_nDataBufSize` を 32bit `unsigned` から **`size_t`（または同等の 64bit 幅）**へ変更し、巨大バッファを正しく表現できるようにする。
- [x] `CMemory::AllocBuffer`: **`nAllocSize <= INT_MAX` 制限**と `malloc` / `realloc` の扱いを見直し、64bit プロセスで実用的な上限まで確保できるようにする（失敗時のメッセージ・リセット挙動も確認）。
- [x] `CStringRef`: `m_nDataLen` が **`unsigned`（32bit）**のため、巨大データの参照長を **`size_t` 等**に揃える。

### P1: インデックス・ループの一貫性

- [x] `CLayout::CalcLayoutOffset`（`CLayout.cpp`）— 行長・ループを `CLogicInt` / `size_t` に。
- [ ] その他の **`int` ループ**や **`static_cast<int>(長さ)` / `GetStringLength()` / `GetLengthWithEOL()` 周りの `int` 縮小**を洗い出し、必要箇所を 64bit 安全にする。（進捗例: `CNativeW::Compare`、`CGrepAgent` のパス走査ループ。**追加分**: `CDlgGrep` / `CGrepEnumFileBase` / `CGrepEnumKeys` / `CDlgFileTree` / `CDlgWindowList` の `vector` 走査を `size_t` に。`CFigure::Match` / `CFigureManager::GetFigure` の文字列長を `ssize_t` にし描画パスの縮小を回避。`CEditView_Search`、`CColor_Quote` / `CColor_Heredoc`、`CStringRef_comp`（行ソート用）の長さ取り扱いを整理。**さらに**: `CEditView_Paint`（描画領域マージ）、`CFigureManager`、`COpeBuf`、`CDlgFileTree` / `CDlgFuncList`（ツリー親スタック）、`CEditView_Ime`（行長を `CLogicInt`）。**さらに**: `CGraphics`（クリップ／ペン／ブラシ）、`CColorStrategyPool`、`CViewCommander_Edit_advanced`（ソート／マージ）。**さらに**: `COpeBlk`、`CDlgFuncList`（`TreeDummylParamToFuncInfoIndex`）、`CViewCommander_File`、`CEditView_Command_New`、`CIfObj`（`MEMBERID` と `vector` サイズ）、`CMarkMgr`。**さらに**: `CPropTypesScreen`（コンボ／メソッド削除）、`CImpExpManager`（INI ツリー項目数の上限）、`CType_Cpp::IsCommentBlock`、`CEditView_Cmdisrch`（`SetPattern` の長さ）、`StdControl`（ツリー深さ）、`CDlgFileTree::GetDataTree`（件数上限）、`CDicMgr::HokanSearch`（候補数上限）、`CMenuDrawer`（`Find`／ツールバー）。**さらに**: `CPropCommon::ApplyData`、`CDlgFuncList::SetTreeFile`、`CShareData::ConvertLangValues`（タイプ設定ループ）。**さらに**: `CShareData` デストラクタ（`m_pvTypeSettings`）、`CDlgFileTree`（パス置換ループ）、`CDlgGrepReplace::OnCbnDropDown`。**さらに**: `CTsvModeInfo`、`CPlugin::ReadPluginDefPlug`、`CJackManager`（ジャック／プラグ列挙・`GetCommandCode`）。**さらに**: `CDlgProfileMgr::SetData`（プロファイル一覧）、`CIfObj::Invoke`（`DISPID` 境界）。**さらに**: `CDlgOpenFile_CommonFileDialog`（MRU／フォルダーコンボ）、`CSakuraEnvironment`（MRU フォルダー走査）。**さらに**: `CDlgGrep` / `CDlgReplace` / `CDlgFind`（`OnCbnDropDown` のキーワードコンボ）、`CMainToolBar::AcceptSharedSearchKey`。**さらに**: `CDlgExec::SetData`、`CMRUFile` / `CMRUFolder`（除外 MRU）、`CShareData_IO`（`ShareData_IO_Cmd` / `ShareData_IO_Other` のタグジャンプキーワード。`SetSizeLimit` を `m_aCommands` 誤呼びから `m_aTagJump.m_aTagJumpKeywords` に修正）。**さらに**: `CShareData_IO` の `ShareData_IO_Keys` / `ShareData_IO_Grep` / MRU の `ExceptMRU` 書き込みループ。**さらに**: `CColorStrategyPool::NotifyOnStartScanLogic`、`ShareData_IO_KeyWords`（書き込み側ループ）。**さらに**: `CFileExt::CreateExtFilter`（`m_vFileExtInfo` を `size_t` で走査、`empty()` で空判定）。**さらに**: `SColorStrategyInfo::CheckChangeColor`、`CViewCommander_Clipboard`（色開始探索）、`CPrintPreview`（同様）。**さらに**: `CCodeTypesForCombobox::GetCount()` 周り（`CDlgGrep`／`CPropTypesWindow`／`CDlgOpenFile_CommonItemDialog`／`CDlgOpenFile_CommonFileDialog`）。**さらに**: `anchorList` のレイアウトループ（`CDlgWindowList`／`CDlgFuncList`／`CDlgTagJumpList`／`CDlgFavorite`／`CDlgDiff`／`CDlgCompare`）を `int(std::size(...))` から `size_t`＋`std::size` に。）
- [x] **`Int` / `ssize_t` / `CLogicInt` の使い分け**をドキュメント化し、新規コードの基準を揃える（下記「型の使い分け」）。

#### 型の使い分け（新規コード・リファクタ時の目安）

| 概念 | 推奨型 | メモ |
|------|--------|------|
| バッファの生のバイト数・`capacity()` | `size_t` | 負にならない長さ。Win32 API の「文字数」引数に渡すときだけ `int` 等へ **境界で** 変換。 |
| 文字列の論理長（行内文字数など） | `ssize_t` または **`CLogicInt`** | ドキュメント／レイアウトの「桁・行内位置」は **`CLogicInt` / `CLayoutInt`** で単位を区別。 |
| 汎用の「そこそこ大きいが単位なし」 | **`Int`**（`primitive.h`） | 既定で `ssize_t`。厳格整数モードでは `CLaxInteger`。 |
| 配列・コンテナのインデックス | `size_t` またはループ対象に合わせた符号付き | `v.size()` 周りは **`size_t` で `for (size_t i = 0; i < v.size(); ++i)`** を優先し、`(int)v.size()` を避ける。 |
| Win32 の座標・ピクセル | `LONG` / `int`（API 定義どおり） | ドキュメント座標から渡すときは **`static_cast` で明示**し、巨大ドキュメントでは **飽和・クリップ**を検討（P2）。 |

**避けたい例:** 論理長を `static_cast<int>` してから `memcmp` 系に渡す（2GB 超で誤比較）。**代わりに** 長さは `size_t` / `ssize_t` のまま比較し、戻り値が `int` だけの API では「-1 / 0 / 1」に正規化する（`CNativeW::Compare` がそのパターン）。

### P2: 座標・API 境界

- [x] **ドキュメント座標（64bit）**と **Win32 の `POINT` / `LONG`（32bit）**の混在箇所を整理し、変換境界を明示する（`CMyPoint` 等）。（**方針**: 論理座標は `Int` / `CLogicInt` / `CLayoutInt` で保持し、**GDI へ渡す直前**に **`ClampIntToLongForGdi`（`basis/Win32GdiClamp.h`）**で `LONG` に飽和する。**`CStrictPoint::GetPOINT`** と **`TwoPointToRect`（`CMyPoint.h`）** も同様。**主要経路**: `MoveToEx`/`LineTo`、`Rectangle`、`PolyPolyline` 用 `POINT`、`SetViewportOrgEx`（`CPrintPreview`）、`SetWindowOrgEx`（`CMenuDrawer` オフスクリーン作画）、`BitBlt`/`PatBlt`/`StretchBlt`（`ClampRectWidthHeightForGdi` / `ClampIntExprForGdi` 参照）。**残存リスク**: `CAutoScrollWnd` 等の固定サイズ以外の未改修 GDI、個別 `RECT` 代入。巨大ドキュメントでの手動確認は P4。**進捗（追記）**: `DrawDropRect` / `CMenuDrawer` に加え、`CEditView_Paint` / `CEditView_Scroll` の互換 `BitBlt`、`CPrintPreview` の `BitBlt`/`StretchBlt`（スケール計算は `long long`）、`MyFillRect`（`CGraphics.h`）、`CCaret` / `CImageListMgr`、`CDlgAbout` の URL 静的コントロール背景（`PatBlt` の幅・高さを **正しく** `ClampRectWidthHeightForGdi` に）。）
- [ ] ファイル I/O・クリップボード・外部コマンド等で **`DWORD` や 32bit 長前提**の箇所を洗い出し、2GB 超を扱う経路ではチャンク処理または 64bit API に切り替える。（**進捗**: `CFileLoad::FileOpen` のファイルサイズ取得を **`GetFileSizeEx`** に変更。`CClipboard::SetText` で **`GlobalAlloc` サイズの乗算・加算オーバーフロー**を検出して失敗させる。外部コマンド等は未整理。）

#### P2 の続き（作業メモ）

- **GDI 境界（論理座標 → `LONG`）**  
  `ClampIntToLongForGdi`（`basis/Win32GdiClamp.h`）と `TwoPointToRect` / `CStrictPoint::GetPOINT` で飽和する経路は整理済み。**進捗**: `CGraphics::DrawRect` の `MoveToEx`/`LineTo` へ渡す座標を **`ClampIntToLongForGdi(Int(...))`** に。`CRuler` の `PolyPolyline` 用 `POINT` 配列も同様。`CEditView::CaretUnderLineON` のカーソル縦線・アンダーライン、`CTextDrawer` の桁縦線・ノート線・折り返し線・ブックマーク縦線の `MoveToEx`/`LineTo` も同様。**進捗（追記）**: `CMenuDrawer`（選択枠 `Rectangle`、区切り線・チェックマーク、`SetWindowOrgEx`、**`BitBlt`**）、`CPrintPreview`（用紙・マージン枠 `Rectangle`、行番号縦線、**`SetViewportOrgEx` 設定時**の X/Y、**`BitBlt`/`StretchBlt`**）、`CGraphics::DrawDropRect`（**`PatBlt`**）、**`CEditView_Paint` / `CEditView_Scroll`（互換 `BitBlt`）**、`MyFillRect`（`PatBlt`）、`CCaret` / `CImageListMgr`、`CDlgAbout`（`PatBlt`）。**残り**: 固定ピクセル `BitBlt`（例: `CAutoScrollWnd`）など。

- **ファイルサイズ・列挙**  
  - `CFileLoad` は `GetFileSizeEx` と **`LONGLONG m_nFileSize`**。x64 では `GetLimitSize()` が実質上限なし（`ULLONG_MAX`）— **メモリ・UI・進捗表示**との整合は P4 と合わせて確認。  
  - **`CGrepEnumFileBase`**: [x] `PairGrepEnumItem` の第 2 要素を **`ULONGLONG`**（`GrepFileSizeFromFindData` で `nFileSizeHigh`/`Low` を合成）。`GetFileSizeBytes` に改名。  
  - **`CGrepAgent`**: [x] `AddTail` の標準出力 `WriteFile` は **`DWORD` 上限で分割**（`GetRawLength()` が 4GB 超でも縮小しない）。

- **クリップボード・D&D・その他 `GlobalAlloc`**  
  `CClipboard::SetText` は乗算・加算オーバーフロー対策済み。**進捗**: `CEditWnd.cpp`（タブ D&D の `CF_UNICODETEXT`）でパス長×`wchar_t` の乗算オーバーフロー防止と `GlobalAlloc`/`GlobalLock` 失敗時の後始末。`CEditView.cpp`（`WM_IME_COMPOSITION`）で `DWORD` 加算オーバーフロー防止と `ImmReleaseContext`/`GlobalFree` の漏れを整理。`CDropTarget.cpp`（`CDataObject::GetData`）、`CEditView_Mouse.cpp`（`PostMyDropFiles`）、`util/os.cpp`（`GetGlobalData` のコピー）で `GlobalAlloc`/`GlobalLock` 失敗時の安全化。**進捗（追記）**: `CDlgProperty.cpp` の `_DEBUG` ブロック（`nBufLen` 負値のガード、`cbAlloc`＋`GlobalLock` 失敗時の解放）。`CDlgFuncList.cpp`（`SizeofResource` が 0 のときは `GlobalAlloc` しない）。

- **外部コマンド・パイプ**  
  `CEditView_ExecCmd.cpp` / `CViewCommander_TagJump.cpp` の `ReadFile` は **約 5KiB チャンク**で `DWORD` 範囲内。**蓄積側**（出力バッファが無制限に伸びる経路）のメモリ方針は別途。

### P3: 厳格整数・ビルド設定

- [ ] `USE_STRICT_INT` / `CStrictInteger` 利用時も **土台のバッファ型が 64bit であること**を前提に、デバッグビルドでの型チェックが意味を持つようにする。

### P4: 検証

- [ ] 巨大行・巨大ファイル（**2GB 境界付近および超**）の回帰テストを追加または手動手順を記載する。（**手動手順の例**: x64 ビルドの `sakura.exe` で 2GB 級ファイルまたは極端に長い行を含むファイルを開き、スクロール・行末移動・検索・保存など主要操作で異常終了しないことを確認する。単体テストでは `CMemory.OverMaxSize` / `OverHeapMaxReq` が加算オーバーフロー防止境界をカバー。）
- [ ] メモリ使用量・主要操作のパフォーマンスを計測し、退行がないことを確認する。
- [x] `tests1` の **`CMemory.OverHeapMaxReq` / `CMemory.OverMaxSize`** を P0 後の `AllocBuffer` 仕様に合わせて更新（下記「テスト」）。

### 参考（現状の把握）

- `CLogicInt` / `CLayoutInt` は通常 **`ssize_t`**（`sakura_core/basis/SakuraBasis.h`）。
- **P0 適用後**は `CMemory` の長さが **`size_t`**、`AllocBuffer` は旧来の **`INT_MAX` 未満のみ**といった制限を撤廃。代わりに **`nNewDataLen > SIZE_MAX - sizeof(wchar_t) - 7`** のときは加算オーバーフロー防止のため確保せずメッセージして `Reset`。

### 実装メモ（P0 付随変更）

- `CMemory::GetRawLength` / `CMemory::capacity` / `CNativeW::capacity` を **`size_t`** に。
- `CStringRef::m_nDataLen` は **`size_t`** のまま、**`GetLength()` の戻り値は `ssize_t`**（既存の比較・算術との互換のため。極端に長い行では縮小の可能性あり）。
- `AppendRawData` で **`m_nRawLen + nDataLen` オーバーフロー**時は追加をスキップ。
- `CSearchAgent.cpp`: `capacity()` が `size_t` になったことに合わせて比較を修正。
- **`CStrictPoint`**: 非 `USE_STRICT_INT` 時も **`IntType` 2 引数コンストラクタ**（`CCommandLine.h` の `CLayoutPoint` 返却などの縮小変換エラー回避）。
- **`CRuler` / `CTextDrawer`**: `t_max` / `t_min` / `std::max` の型不一致を修正。
- **`StdAfx.h`**: MSVC で **`C4244` を一時的に無効化**（`CLogicInt`/`Int` とレガシー `int` の混在が多数のため。段階的に型を揃えたら外す）。
- **`CStrictPoint`**: 非 `USE_STRICT_INT` では **`(int,int)` コンストラクタを置かず `IntType` のみ**（曖昧性解消）。`GetPOINT()` は **`LONG` へ `static_cast`**。
- **`CShareData_IO` の `swscanf_s`**: `CKetaXInt` 向けに **`int` 一時変数**へ読み取り後代入。
- **`CType_Cpp` の `SCommentBlock`**: ループ変数 `ssize_t n` と `CLogicInt` 混在を **`static_cast<int>`** で集約。
- **`CViewCommander_Search`**: `CLayoutInt::GetValue()` 前提を **`static_cast<ssize_t>(GetLineCount())`** に変更。
- **`CNativeW::Compare`**: 比較長を `int` に落とさず **`ssize_t` / `size_t`** で `wmemcmp`し、長さ差は **-1/0/1** で返す。
- **`CGrepAgent`**: 複数パス走査のループ変数を **`(int)vPaths.size()` から `size_t` インデックス**に変更。
- **GoogleTest（CMake / MSBuild）**: `src/test/cmake/GoogleTest.cmake` で MSVC 時 **`CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>`** を明示（Debug の gtest/gmock が `/MTd` になり **`tests1` と LNK2038 にならない**）。**`CMAKE_INSTALL_LIBDIR` は `lib` に固定**（`-D` にジェネレータ式を書くと install が失敗するため）。サブプロジェクト用に **`${CMAKE_BINARY_DIR}/gtest-init.cmake`** を生成し **`cmake -C` で `CMAKE_INSTALL_LIBDIR=lib` を FORCE** し、`lib$<$<CONFIG:Debug>:/Debug>` のような誤キャッシュで `cmake --install` が落ちるのを防ぐ。`src/test/msbuild/googletest.props` は Debug/Release とも **`$(CMakeToolsBuildDir)lib`** を参照（`cmake --install` の `--config` と対応。**`lib` 内の `.lib` は最後に install した構成**になるので、Debug/Release を切り替えたら必要に応じて再ビルドで `BuildGoogleTest` を走らせる）。
- x64 **Release / Debug ビルド成功**（`build-sln.bat x64 Release` / `x64 Debug`）。

#### テスト（`test-cmemory.cpp`）

- **`CMemory.OverMaxSize`**: 旧仕様「`INT_MAX` を超えると必ず失敗」は P0 後は成立しない（64bit プロセスでは `INT_MAX+1` バイトの確保が**あり得る**）。**`AllocBuffer` が拒否する最小の `nNewDataLen`** として `SIZE_MAX - sizeof(wchar_t) - 6` を使い、**`GetRawPtr() == nullptr`** を検証する。
- **`CMemory.OverHeapMaxReq`**: **`_HEAP_MAXREQ` を `unsigned` に落として +1** する旧テストは x64 で意図と異なる。上記と同じ **加算オーバーフロー防止境界**で検証する。
- 手元の XML 出力例: `.\tests1.exe --gtest_output=xml:tests1.exe-googletest-x64-Debug.xml`（作業ディレクトリは `x64\Debug` など `tests1.exe` がある場所）。

#### CI・エンコーディング（`checkEncoding.py`）

- **Check Encoding** ワークフロー（`.github/workflows/check-encoding.yml`）は `python checkEncoding.py` を実行する。`origin/master` と `HEAD` の **merge base からの差分**にある `.cpp`/`.h`/`.rc`/`.rc2` が対象（拡張子ごとに期待エンコーディングが異なる）。
- `.cpp`/`.h` は **`UTF-8-SIG`（UTF-8 BOM）または `ascii`** のみ OK。BOM なし UTF-8 は `chardet` が `utf-8` と返し **NG** になる。
- 新規ヘッダ（例: `basis/Win32GdiClamp.h`）を追加するときは **必ず BOM 付き UTF-8** でコミットする。Visual Studio では「**署名付き UTF-8** で保存」など。
- `AGENTS.md` など一部パスは workflow の `paths-ignore` により **push/PR トリガから除外**されるが、`checkEncoding.py` 自体は差分にソースが含まれれば **本文のエンコーディングはチェックされる**（Markdown の除外はトリガ条件のみ）。

#### P2 付随の実装メモ（追記）

- **`CGrepEnumFileBase`**: ファイルサイズ型を `GrepEnumFileSizeBytes` として **`_WIN64` では `ULONGLONG`、Win32 では `DWORDLONG`**（いずれも 64bit）。`GetCount`/`GetFileName` を `const` に調整。
- **`CGrepAgent::AddTail`**: 標準出力への `WriteFile` を **`std::numeric_limits<DWORD>::max()` バイト以下のチャンク**でループ（1 回の `DWORD` キャストで `GetRawLength()` が切り詰められないようにする）。
- **`GlobalAlloc`**: `CClipboard` に続き、`CEditWnd`（タブ D&D）、`CEditView`（IME）、`CDropTarget::GetData`、`PostMyDropFiles`、`GetGlobalData` のコピー、`CDlgProperty` の `_DEBUG` 読み込み、`CDlgFuncList` のテンプレートサイズ 0 ガード等を整理。
- **GDI**: `ClampIntToLongForGdi` を `CGraphics::DrawRect` / **`DrawDropRect`（`PatBlt`）**、`CRuler`（`PolyPolyline`）、`CEditView::CaretUnderLineON`、`CTextDrawer`（桁線・ノート線・折り返し・ブックマーク）、`CMenuDrawer`（**`BitBlt`** 含む）/`CPrintPreview`（**`BitBlt`/`StretchBlt`**）、`CEditView_Paint` / `CEditView_Scroll`（互換 **`BitBlt`**）、`MyFillRect`（`CGraphics.h` の **`PatBlt`**）、`CCaret` / `CImageListMgr`、`CDlgAbout`、`SetViewportOrgEx`/`SetWindowOrgEx`（設定時）などに適用。**`Win32GdiClamp.h`** に **`ClampRectWidthHeightForGdi`**（`RECT` 差分の `long long` 算出・非負・上限）、**`ClampIntExprForGdi`**（`long long` → `int` 飽和）を追加。**`Win32GdiClamp.h` は UTF-8 BOM 必須**（上記 CI）。

#### P2 残タスク（次の候補）

- **GDI**: [x] `CMenuDrawer` / `CPrintPreview`（`Rectangle` / `MoveToEx` / `LineTo`、`SetWindowOrgEx` / `SetViewportOrgEx`（設定時）、**`BitBlt`/`StretchBlt`**）、`CGraphics::DrawDropRect`（**`PatBlt`**）、`CEditView_Paint` / `CEditView_Scroll`（互換 **`BitBlt`**）、`MyFillRect`、`CCaret` / `CImageListMgr`、`CDlgAbout`（**`PatBlt`** の幅・高さ修正含む）に **`ClampIntToLongForGdi` / `ClampRectWidthHeightForGdi` / `ClampIntExprForGdi`** を適用済み。**残り**: `CAutoScrollWnd` の固定 32×32 `BitBlt` など（影響小）。
- **幅・高さ系**: `PatBlt` / `BitBlt` / `FillRect` 等で **矩形サイズ**（`rc.right - rc.left` 等）が極端に大きい場合のオーバーフローや GDI の実効範囲は、座標飽和とは別に要検討（P2/P4 境界）。
- **外部コマンド**: `CEditView_ExecCmd` 等の **子プロセス出力の蓄積**が無制限に伸びる経路があれば、上限・ストリーミング・失敗時の扱いを検討（メモリ・応答性）。

#### ローカル検証の例

- **ビルド**: `build-sln.bat x64 Debug`（または `Win32 Debug` / `x64 Release`）。ログは `msbuild-x64-Debug.log` など。
- **単体テスト**: `x64\Debug\tests1.exe`（作業ディレクトリは `tests1.exe` と同じ）。短時間だけなら `--gtest_filter=-*WinMain*`（WinMain 連動 4 件を除外）。
- **エンコーディング**: `python checkEncoding.py` は **`origin/master` が取得済み**で merge base が取れる前提（`fetch-depth: 0` は CI 側）。手元で全 `.cpp`/`.h` を総当たりする `checkEncoding.py all` は **`build\` 等の生成物まで拾う**ため、通常は差分モードか、対象ファイルを限定して使う。
