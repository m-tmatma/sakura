# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Sakura Editor is a Japanese Windows text editor written in C++20 using Win32 APIs. It supports multi-byte character sets, multiple language syntaxes (40+ types), regex search (Oniguruma), Migemo phonetic search, ctags navigation, and a plugin system (DLL + Windows Script Host).

## Build Commands

**Prerequisites:** Visual Studio 2019+, CMake, PowerShell Core (`pwsh.exe`), 7-Zip

```cmd
# Build executable only (x64 or Win32, Release or Debug)
build-sln.bat x64 Release
build-sln.bat Win32 Debug

# Full build: executable + help + installer
build-all.bat x64 Release
```

Output lands in `Win32\Release\` or `x64\Debug\` etc.

To override Visual Studio version: `set ARG_VSVERSION=16` before calling build scripts.

## Test Commands

Unit tests use **GoogleTest**, built as `tests1.exe`:

```cmd
# Build includes tests; run directly after build:
x64\Release\tests1.exe

# Run a single test:
x64\Release\tests1.exe --gtest_filter=TestSuiteName.TestName

# Via CTest (CMake build):
ctest --test-dir build --output-on-failure
```

Test sources live in `src/test/cpp/tests1/`. Test data/fixtures in `src/test/resources/testing/`.

## Architecture

### Process Model

Sakura uses a multi-process architecture:
- **CControlProcess** — the first invisible process; owns the shared configuration and IPC
- **CNormalProcess** — each editor window is a separate process; connects to CControlProcess on startup

Both process types are bootstrapped from `sakura_core/_main/`.

### Document Model (MVC + Observer)

```
CEditDoc  ←→  CDocLineMgr (logical lines)
          ←→  CLayoutMgr  (visual/wrapped lines)
          ←→  CEditView × 4 (split-pane views, each is a CDocListener)
          ←→  Agents × 7   (each is a CDocListener)
```

**CDocSubject / CDocListener** (`doc/CDocListener.h`) — Observer pattern that drives the document lifecycle. Any class inheriting `CDocListener` receives ordered callbacks:

- Load: `OnCheckLoad → OnBeforeLoad → OnLoad → OnLoading → OnAfterLoad → OnFinalLoad`
- Save: `OnCheckSave → OnPreBeforeSave → OnBeforeSave → OnSave → OnSaving → OnAfterSave → OnFinalSave`
- Close: `OnBeforeClose`

### Agent Pattern

Async/decoupled operations are implemented as `CDocListenerEx` subclasses in `sakura_core/agent/`:

| Agent | Responsibility |
|---|---|
| CLoadAgent | File read + encoding detection |
| CSaveAgent | File write |
| CSearchAgent | Incremental search |
| CGrepAgent | Directory grep |
| CAutoSaveAgent | Periodic auto-save |
| CAutoReloadAgent | Detect external file changes |
| CBackupAgent | Backup on save |

### Commands

`CViewCommander` (`cmd/CViewCommander.h`) is the central command dispatcher (~200 commands). All editor actions (insert, delete, search, format, etc.) are routed through it. Commands are identified by `EFunctionCode` enums defined in `func/`. Undo/redo uses `COpe / COpeBlk / COpeBuf`.

### Character Encoding

`charset/` contains codec classes for every supported encoding. `CCodeFactory` instantiates the correct `CCodeBase` subclass. `CCodeMediator` handles conversion between encodings. All I/O passes through these to ensure correct multi-byte handling.

### Language Type System

`types/` contains ~40 `CType_*` classes (one per language). `CDocTypeManager` (`env/`) is the registry. Each type defines syntax highlighting rules, comment styles, indent behaviour, and which outline analyzer to use. Outline analyzers live in `outline/`.

### Plugin System

`plugin/` — DLL-based (`CDllPlugin`) and WSH-based (`CWSHPlugin`) plugins. `CJackManager` provides the hook/event mechanism. Plugin interfaces: `COutlineIfObj`, `CComplementIfObj`, `CSmartIndentIfObj`.

### Key Singletons / Holders

- `CEditApp` — `TSingleton<CEditApp>`: application-level singleton, owns global managers
- `CEditDoc` — `TInstanceHolder<CEditDoc>`: per-document instance management
- Templates defined in `util/design_template.h`

### Major Subsystem Map

| Folder | Purpose |
|---|---|
| `_main/` | Process startup, CCommandLine parsing |
| `agent/` | Async doc operation agents (see above) |
| `charset/` | All character encoding codecs |
| `cmd/` | CViewCommander + undo/redo (COpe*) |
| `doc/` | CEditDoc, CDocLineMgr, CLayoutMgr, CDocListener |
| `docplus/` | Bookmarks, diff, function list, modify tracking |
| `dlg/` | 50+ dialog classes |
| `env/` | CDocTypeManager, CPropertyManager, profile/settings |
| `extmodule/` | Wrappers for bregonig, cmigemo, ctags |
| `io/` | Low-level file I/O (CBinaryStream etc.) |
| `macro/` | Macro recording and WSH macro execution |
| `outline/` | Code outline parsers per language |
| `plugin/` | Plugin loading, interfaces, jack (hook) system |
| `types/` | Per-language type definitions |
| `uiparts/` | CGraphics, CMenuDrawer, icons, sounds |
| `view/` | CEditView, rendering, caret, ruler, selection |
| `window/` | CEditWnd, toolbar, statusbar, tabs, splitter |

### External Dependencies (git submodules in `externals/`)

- **bregonig** — Oniguruma regex engine
- **cmigemo** — Migemo phonetic search (Japanese input)
- **ctags** — Symbol/tag navigation
- **diffutils** — File comparison

## CI

GitHub Actions (`.github/workflows/`):
- `build-sakura.yml` — matrix build (Win32/x64 × Debug/Release), runs cppcheck and unit tests
- `check-encoding.yml` — validates source file encodings
- `sonarscan.yml` — SonarQube static analysis

Add `[ci skip]` or `[skip ci]` to a commit message to bypass CI runs.
