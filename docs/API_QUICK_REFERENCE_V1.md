# CASIO GUI v1 — quick reference

## Core

```cpp
casio::GUI gui;
casio::Cursor;
casio::Item;
casio::Position;
casio::Point;
casio::Color;
```

Pages:

```cpp
gui.createPage(...)
gui.setPage(...)
gui.addItem(...)
gui.addItemToPage(...)
gui.addGlobalItem(...)
```

Focus/cursor:

```cpp
gui.setFocusById(...)
gui.clearFocus()
gui.focusNext()
gui.focusPrevious()
gui.showCursor()
gui.hideCursor()
```

Themes:

```cpp
gui.setThemePreset(casio::ThemePreset::LIGHT);
gui.setThemePreset(casio::ThemePreset::DARK);
gui.setThemePreset(casio::ThemePreset::HIGH_CONTRAST);
```

## Controls

```text
Button              ToggleButton
TextBox             Numeric
Slider              HorizontalSlider / VerticalSlider
Checkbox
ProgressBar         HorizontalProgressBar / VerticalProgressBar
ScrollBar           HorizontalScrollBar / VerticalScrollBar
LED
ColorWheel          HueSelector
Gauge               NeedleMeter / Meter
Dial
Tab
MenuBar             SoftKeyBar / BottomMenu
RadioButton         RadioGroup
ListBox             ComboBox / DropDown
Tree
Table               Spreadsheet
ImageItem           ImageButton / IconButton
TimeItem            TimerItem
```

## Layout

```text
Container
Panel
GroupBox
ScrollView
TabView / TabPages
LayoutMode
Margins
ContainerChild
```

Anchors:

```cpp
casio::ANCHOR_LEFT
casio::ANCHOR_RIGHT
casio::ANCHOR_TOP
casio::ANCHOR_BOTTOM
```

## Drawing / shapes

```text
Canvas / DrawingCanvas
Rectangle / RoundedRectangle / Square
Circle / Ellipse
Triangle / RightTriangle / Trapezoid
Polygon / RegularPolygon
Line / Polyline / Arc
Sector / Pie
Transform2D
```

## Text / chrome

```text
Label
Separator
Toolbar
StatusBar
TextAlign
```

## Data / graph

```text
Graph
GraphAxis
GraphPoint
GraphSeries
GraphScale
GraphCursor
GraphCursorStyle
```

## Popups

```text
Popup
PromptPopup
BoolPopup / ChoicePopup
NumericPopup
OptionsPopup
CheckboxPopup
TogglePopup
```

## Persistence

```cpp
casio::IniFile
casio::ConfigFile
casio::SaveFile
casio::DataFile
casio::GuiStateFile
casio::NamedObjectFile
casio::ObjectFile
casio::ObjectSerializer
casio::NamedSaveFile
```

Namespaces:

```cpp
casio::storage
casio::safeStorage
```

## Screenshot

```cpp
gui.enableScreenshotCapture(KEY_OPTN, "image");
gui.openScreenshotPrompt();

std::string path;
auto result =
    casio::Screenshot::saveBmp(
        "image",
        &path
    );
```

## Low-level facade

`casio.hpp` also re-exports selected gint screen, keyboard, timer, RTC, image and
OS-bridge functions. The exact frozen list is recorded in
`PUBLIC_API_MANIFEST_V1.md`.
