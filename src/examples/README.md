# CASIO GUI 1.0.0 — Final examples

These examples were rewritten from scratch after the v1 API freeze. They do
not depend on the older phase-oriented examples.

Every source uses the public facade:

```cpp
#include "CASIO_GUI/casio.hpp"
```

and avoids implementation names such as `item_*`.

## Examples

| File | Focus |
|---|---|
| `00_minimal_gui.cpp` | Minimal GUI, Label, Button, StatusBar |
| `01_pages.cpp` | Pages and global SoftKeyBar navigation |
| `02_tabs.cpp` | Tab and TabView |
| `03_controls.cpp` | TextBox, Numeric, Slider, Checkbox, RadioGroup, ComboBox, LED |
| `04_layouts.cpp` | Panel, Grid, ScrollView, nested focus |
| `05_graph.cpp` | Graph, series, cursors, pan/zoom |
| `06_spreadsheet.cpp` | Table / Spreadsheet editing |
| `07_tree_list_combo.cpp` | Tree, ListBox, ComboBox |
| `08_canvas_shapes.cpp` | Canvas retained drawing and standalone shape |
| `09_instruments.cpp` | ColorWheel, Numeric, LED, Gauge, Dial, ProgressBar |
| `10_popups.cpp` | Prompt, Bool, Numeric and Options popups |
| `11_images.cpp` | ImageItem, ImageButton, fxconv asset |
| `12_storage.cpp` | Safe INI/config storage |
| `13_screenshot.cpp` | OPTN screenshot to `/Capt` |
| `14_navigation.cpp` | MenuBar, SoftKeyBar, themes |
| `15_complete_dashboard.cpp` | Multi-page integrated example |

## Build a specific example

The root `CMakeLists.txt` now has an optional example selector.

Build the normal validation application:

```bash
rm -rf build-cg
fxsdk build-cg
```

Build an example instead:

```bash
rm -rf build-cg
fxsdk build-cg -D CASIO_GUI_EXAMPLE=05_graph
```

If your local fxSDK wrapper does not forward `-D` arguments, configure with
CMake manually or temporarily copy the desired example to `src/main.cpp`.

The selector value is the filename without `.cpp`, for example:

```text
00_minimal_gui
01_pages
02_tabs
05_graph
06_spreadsheet
15_complete_dashboard
```

## Runtime contracts used by the examples

- GUI/container item pointers are non-owning.
- Child controls must outlive their container.
- `EXIT` closes/releases widget focus; it does not terminate the GUI loop.
- `MENU` remains controlled by the calculator/OS.
- A nested Checkbox does not take keyboard focus.
- ScrollView scrollbars are explicit focus targets.
- TextBox MIXED mode uses `ALPHA` for text/numeric and `SHIFT` for Caps Lock.
- Screenshot output is written under `/Capt`.
- Safe application storage uses the world-switch-safe storage layer.
