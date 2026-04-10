# Agent / contributor notes

## 64bit 完全対応・メモリ内部表現（64bit 変数）

以下はコード調査に基づくタスク一覧。優先度は概ね上から。

### P0: メモリバッファ層

- [x] `CMemory`: `m_nRawLen` / `m_nDataBufSize` を 32bit `unsigned` から **`size_t`（または同等の 64bit 幅）**へ変更し、巨大バッファを正しく表現できるようにする。
- [x] `CMemory::AllocBuffer`: **`nAllocSize <= INT_MAX` 制限**と `malloc` / `realloc` の扱いを見直し、64bit プロセスで実用的な上限まで確保できるようにする（失敗時のメッセージ・リセット挙動も確認）。
- [x] `CStringRef`: `m_nDataLen` が **`unsigned`（32bit）**のため、巨大データの参照長を **`size_t` 等**に揃える。

### P1: インデックス・ループの一貫性

- [x] `CLayout::CalcLayoutOffset`（`CLayout.cpp`）— 行長・ループを `CLogicInt` / `size_t` に。
- [ ] その他の **`int` ループ**や **`static_cast<int>(長さ)` / `GetStringLength()` / `GetLengthWithEOL()` 周りの `int` 縮小**を洗い出し、必要箇所を 64bit 安全にする。（進捗例: `CNativeW::Compare`、`CGrepAgent` のパス走査ループ）
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

- [ ] **ドキュメント座標（64bit）**と **Win32 の `POINT` / `LONG`（32bit）**の混在箇所を整理し、変換境界を明示する（`CMyPoint` 等）。
- [ ] ファイル I/O・クリップボード・外部コマンド等で **`DWORD` や 32bit 長前提**の箇所を洗い出し、2GB 超を扱う経路ではチャンク処理または 64bit API に切り替える。

### P3: 厳格整数・ビルド設定

- [ ] `USE_STRICT_INT` / `CStrictInteger` 利用時も **土台のバッファ型が 64bit であること**を前提に、デバッグビルドでの型チェックが意味を持つようにする。

### P4: 検証

- [ ] 巨大行・巨大ファイル（**2GB 境界付近および超**）の回帰テストを追加または手動手順を記載する。
- [ ] メモリ使用量・主要操作のパフォーマンスを計測し、退行がないことを確認する。

### 参考（現状の把握）

- `CLogicInt` / `CLayoutInt` は通常 **`ssize_t`**（`sakura_core/basis/SakuraBasis.h`）。
- **P0 適用後**は `CMemory` の長さが **`size_t`**、`AllocBuffer` は **`INT_MAX` 未満のみ確保**していた制限を撤廃（オーバーフロー時はメッセージして `Reset`）。

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
- x64 **Release ビルド成功**（`build-sln.bat x64 Release`）。
