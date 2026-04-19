# Plan: 64bit 完全対応 — 残タスク実装

## Context

ブランチ `research/test-2gb-or-more` では 71 コミット・226 ファイルにわたる 64-bit 対応リファクタを実施済み。AGENTS.md で定義した P0〜P4 のうち P0（バッファ型）・P2 GDI クランプ・P2 ファイル I/O チャンク化はほぼ完了。以下の未チェック項目を本計画で実装する。

---

## 残タスク一覧（優先度順）

### A. P2 — `fseek` → `_fseeki64`（中程度リスク）

**対象:** `sakura_core/dlg/CDlgTagJumpList.cpp:1391`

```cpp
// Before
fseek(fp, 0, SEEK_END);

// After
_fseeki64(fp, 0LL, SEEK_END);
```

tags ファイルが 2GB を超える場合、`fseek` は 32-bit `long offset` しか受け付けないため SEEK_END が正しく動作しない。`_fseeki64` に差し替えるだけでよい（続く `fgetpos`/`fsetpos` は `fpos_t` を使うため変更不要）。

---

### B. P1 — `CGrepAgent.cpp` 縮小キャスト除去

**対象:** `sakura_core/agent/CGrepAgent.cpp:213-217`

```cpp
// Before
const int nPathLen = static_cast<int>( strPath.length() );
int nPathPos = 0;
while( nullptr != (token = my_strtok<WCHAR>( strPath.data(), nPathLen, &nPathPos, L";"))) ){

// After
const ssize_t nPathLen = static_cast<ssize_t>( strPath.length() );
ssize_t nPathPos = 0;
while( nullptr != (token = my_strtok<WCHAR>( strPath.data(), nPathLen, &nPathPos, L";"))) ){
```

事前確認が必要: `my_strtok` のシグネチャ（`sakura_core/util/string_ex.h`）の `nStrLen`/`nCurPos` が `int` の場合は同ファイルも `ssize_t` に変更する。

---

### C. P1 — `CColor_Quote.cpp` + `Match_QuoteStr` 縮小キャスト除去

**対象:**
- `sakura_core/view/colors/CColor_Quote.cpp:135` と `:218`
- `sakura_core/view/colors/CColor_Quote.h` — `Match_QuoteStr` 宣言
- 実装内シグネチャ

```cpp
// Before (2 箇所)
Match_QuoteStr( m_tag.c_str(), static_cast<int>(m_tag.size()), ... )

// After
Match_QuoteStr( m_tag.c_str(), static_cast<ssize_t>(m_tag.size()), ... )
```

`Match_QuoteStr` の第 2 引数 `int nStrLen` を `ssize_t nStrLen` に変更し、実装内の対応する変数型も揃える。

---

### D. P1 — `CEditView_ExecCmd.cpp` 不要な `int` キャスト整理

**対象:** `sakura_core/view/CEditView_ExecCmd.cpp:390-394`

```cpp
// Before
int read_cntw;
read_cntw = (int)read_cnt/sizeof(wchar_t);
if( read_cnt % (int)sizeof(wchar_t) ){

// After
DWORD read_cntw;
read_cntw = read_cnt / sizeof(wchar_t);
if( read_cnt % sizeof(wchar_t) ){
```

バッファは定数 `MAX_WORK_READ = 5120` バイト固定なので実害はないが、`(int)` キャストが冗長。`read_cnt` は `DWORD` なので `read_cntw` も `DWORD` が正確。

---

### E. AGENTS.md 更新

実装完了後、以下を更新：

1. **P2 fseek タスク（行 34）** — CDlgTagJumpList.cpp の `_fseeki64` 対応を進捗メモに追記してチェック
2. **P1 縮小キャスト（行 15）** — `CGrepAgent`・`CColor_Quote`・`CEditView_ExecCmd` の修正を追記
3. **P3（行 57）** — `CStrictInteger` は内部 `ssize_t` ベース（確認済み）と注記し、「厳格整数ビルドでの end-to-end 確認」を残タスクとして残す
4. **P4 手動テスト手順（行 61）** — 以下を追記：
   ```
   手動確認手順（x64 Release）:
   - 2GB 超ファイルを生成して sakura.exe で開く
   - スクロール・行末移動・検索・保存が正常動作すること
   - タグジャンプ（.tags ファイル）が大容量でも正常動作すること
   ```

---

## 実装前に確認が必要なシグネチャ

| 関数 | ファイル | 確認事項 |
|------|----------|----------|
| `my_strtok<WCHAR>` | `sakura_core/util/string_ex.h` | 第 2・第 3 引数の型が `int` なら `ssize_t` へ変更 |
| `Match_QuoteStr` | `sakura_core/view/colors/CColor_Quote.h` | 第 2 引数 `nStrLen` が `int` なら `ssize_t` へ変更 |

---

## 変更ファイル一覧

| ファイル | 変更内容 |
|----------|----------|
| `sakura_core/dlg/CDlgTagJumpList.cpp` | `fseek` → `_fseeki64`（1 行） |
| `sakura_core/agent/CGrepAgent.cpp` | `int nPathLen/nPathPos` → `ssize_t`（2 行） |
| `sakura_core/util/string_ex.h` / `string_ex.cpp` | `my_strtok` シグネチャ変更（要確認） |
| `sakura_core/view/colors/CColor_Quote.cpp` | `static_cast<int>` → `static_cast<ssize_t>`（2 箇所） |
| `sakura_core/view/colors/CColor_Quote.h` | `Match_QuoteStr` 宣言の型変更 |
| `sakura_core/view/CEditView_ExecCmd.cpp` | `int read_cntw` → `DWORD`、`(int)sizeof` キャスト除去（3〜4 行） |
| `AGENTS.md` | 進捗・P4 手動テスト手順の追記 |

---

## 検証方法

```cmd
build-sln.bat x64 Debug
x64\Debug\tests1.exe
build-sln.bat x64 Release
```

- ビルドエラーなし（特に `my_strtok` / `Match_QuoteStr` の型変更による不一致がないこと）
- `tests1.exe` が全テスト通過
- `python checkEncoding.py` で新規変更ファイルのエンコーディング確認（`.cpp`/`.h` は UTF-8 BOM 必須）
