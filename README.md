# CASIO GUI 1.0.0 — GRAPH 90+E

Frozen v1 public API for the C++ GUI framework validated on the Casio GRAPH 90+E.

## Release status

```cpp
casio::GUI_VERSION       // "1.0.0"
casio::GUI_API_LEVEL     // 21
casio::GUI_API_FROZEN    // true
casio::GUI_API_STAGE     // "stable"
```

The physically validated runtime baseline includes:

- controls, shapes, graph, table, tree, layouts and containers;
- Light / Dark / High Contrast themes;
- pointer/focus routing, nested focus rules and TextBox input modes;
- image items and fxconv assets;
- persistence and safe storage;
- screenshot capture to `/Capt/*.bmp`;
- filesystem operations protected by `gint_world_switch()`.

## Recommended include

```cpp
#include "CASIO_GUI/casio.hpp"
```

Application code should use the `casio::` facade rather than directly depending
on implementation names such as `item_*`.

## Build

The included validation application is built with:

```bash
rm -rf build-cg
fxsdk build-cg
```

The CMake project links `Gint::Gint` and `stdc++` and uses C++17.

## Documentation

- `docs/API_FREEZE_V1.md` — compatibility contract.
- `docs/API_QUICK_REFERENCE_V1.md` — public API overview.
- `docs/PUBLIC_API_MANIFEST_V1.md` — frozen names and configuration macros.
- `docs/RUNTIME_CONTRACTS_V1.md` — ownership, focus, MENU/EXIT, files, themes.
- `docs/RELEASE_NOTES_V1.0.0.md` — release summary.
- `docs/API_BASELINE_V1.sha256` — frozen public-header hashes.

Run:

```bash
python3 tools/check_api_freeze.py
```

before future releases to detect public-header drift.


## Final examples

The old phase-oriented examples have been replaced by a clean v1 example suite
under:

```text
src/examples/
```

See `src/examples/README.md`.

A specific example can be selected at CMake configure time with:

```text
CASIO_GUI_EXAMPLE=<filename without .cpp>
```

The default remains `src/main.cpp`, so existing validation/build behavior is
unchanged when the option is not set.
