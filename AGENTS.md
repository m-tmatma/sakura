# Agent / contributor notes

## 64bit 完全対応・メモリ内部表現（64bit 変数）

以下はコード調査に基づくタスク一覧。優先度は概ね上から。

### P0: メモリバッファ層

- [ ] `CMemory`: `m_nRawLen` / `m_nDataBufSize` を 32bit `unsigned` から **`size_t`（または同等の 64bit 幅）**へ変更し、巨大バッファを正しく表現できるようにする。
- [ ] `CMemory::AllocBuffer`: **`nAllocSize <= INT_MAX` 制限**と `malloc` / `realloc` の扱いを見直し、64bit プロセスで実用的な上限まで確保できるようにする（失敗時のメッセージ・リセット挙動も確認）。
- [ ] `CStringRef`: `m_nDataLen` が **`unsigned`（32bit）**のため、巨大データの参照長を **`size_t` 等**に揃える。

### P1: インデックス・ループの一貫性

- [ ] 行・文字位置のループを **`int` から `CLogicInt` / `size_t` / 方針で決めた単一のインデックス型**へ置換する（例: `CLayout::CalcLayoutOffset` の `int nLineLen` / `for (int i = ...)` など）。
- [ ] `static_cast<int>(長さ)` や **`GetStringLength()` / `GetLengthWithEOL()` 周りの `int` 縮小**を洗い出し、必要箇所を 64bit 安全にする。
- [ ] **`Int` / `ssize_t` / `CLogicInt` の使い分け**をドキュメント化し、新規コードの基準を揃える。

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
- 一方 **`CMemory` の `INT_MAX` ガードと `unsigned` メンバ**が、実質的な上限の主因になりうる（`sakura_core/mem/CMemory.cpp` / `CMemory.h`）。
