# API Freeze — CASIO GUI v1

## Frozen baseline

The public API is frozen at:

```text
Semantic version : 1.0.0
Feature level    : 21
API stage        : stable
```

The canonical entry point is:

```cpp
#include "CASIO_GUI/casio.hpp"
```

## What is public

The v1 compatibility contract covers:

1. Names exported by the `casio::` namespace in `casio.hpp`.
2. Public constructors and public methods of classes exposed through those aliases.
3. Public enums/structs used in those signatures.
4. `casio::storage`, `casio::safeStorage`, and `casio::Screenshot`.
5. The compile-time configuration macros listed in `PUBLIC_API_MANIFEST_V1.md`.

Implementation names such as `item_*`, private/protected members, `detail`
namespaces, test code, the validation `main.cpp`, and phase-history files are
not separate compatibility promises. Applications should use the `casio::`
facade.

## Semantic-versioning rule

### 1.0.x

Patch releases may fix bugs, rendering, stability, storage handling or
documentation without removing or changing a public v1 signature.

### 1.x

Minor releases may add new controls, methods, overloads, enum values or optional
features. Existing v1 source must continue to compile unless a documented
platform/toolchain change makes this impossible.

### 2.0

Removing/renaming a public symbol, changing a public method signature
incompatibly, changing ownership rules incompatibly, or changing a documented
core interaction contract requires a major version.

## Deprecation policy

Before removing a v1 public symbol in a future major release, prefer:

1. keep the old API working;
2. mark/document it as deprecated;
3. provide the replacement;
4. remove only at the next major version.

No v1 API is deprecated in 1.0.0.

## Freeze enforcement

`docs/API_BASELINE_V1.sha256` stores hashes of the public headers.

Run:

```bash
python3 tools/check_api_freeze.py
```

Any public-header drift must be reviewed deliberately. A changed hash does not
automatically mean a breaking change; it means the API surface requires review.
