# Phase 18 — nested focus + real CG image asset

## Focus

Containers now use a recursive `wantsPointerFocus(x, y)` rule.

A Checkbox inside:

```text
TabView -> Panel -> Checkbox
```

behaves exactly like a Checkbox directly on a page:

```text
EXE -> toggle
      no focus
      cursor remains free
```

This also applies to Grid and ScrollView child Checkboxes.

Focusable children (TextBox, Numeric, Slider, etc.) keep normal focus behavior.

Special cases remain:
- TabView header: focus
- ScrollView scrollbar: focus

## Image

The uploaded project put `gui_test_image.png` in `assets-fx`, while
`fxsdk build-cg` only compiled `ASSETS_cg`.

Phase 18 copies the image to `assets-cg`, adds CG fxconv metadata, and registers
it in CMake for both fx and cg targets.

The validation main now uses a strong `img_gui_test` symbol. If fxconv does not
link it, the build fails instead of silently showing "Image asset: MISSING".

A successful `fxsdk build-cg` should therefore display the real image in both
ImageItem and ImageButton.
