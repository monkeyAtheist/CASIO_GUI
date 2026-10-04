# Runtime contracts — v1

## Ownership

GUI items and container children are **non-owning raw pointers**.

The caller must keep every referenced control alive for at least as long as the
GUI/container uses it. Do not add the same child both as a top-level GUI item
and as a container child.

## MENU / application exit

The framework does not implement an application-level MENU exit.

The calculator/OS remains responsible for MENU/application switching. Do not
add custom `KEY_MENU` logic to terminate the GUI loop.

## EXIT

`EXIT` is a widget/navigation action only:

- close popup;
- leave focus;
- back out of a focused interaction.

`EXIT` must not terminate the GUI/application loop.

## Focus

There is one top-level keyboard focus at a time.

Pointer focus follows the deepest control under the cursor. A nested Checkbox
behaves like a top-level Checkbox and does not acquire focus merely because it
is inside Panel/TabView/Grid/ScrollView.

Documented exceptions:

- TabView header can take focus;
- ScrollView scrollbar can take focus;
- controls that require keyboard interaction (TextBox, Numeric, Slider, Tree,
  ComboBox, ListBox, etc.) can take focus.

The focused cursor uses the theme/focus visual state.

## TextBox

Public modes:

```cpp
casio::TextBoxInputMode::MIXED
casio::TextBoxInputMode::TEXT_ONLY
casio::TextBoxInputMode::NUMERIC_ONLY
```

In `MIXED` mode:

- `ALPHA` toggles text/numeric entry;
- `SHIFT` toggles Caps Lock while in text mode.

The GUI requests physical SHIFT/ALPHA events rather than letting gint consume
them as modifier-only state.

## Themes

Three built-in presets are frozen:

```cpp
casio::ThemePreset::LIGHT
casio::ThemePreset::DARK
casio::ThemePreset::HIGH_CONTRAST
```

Compile-time default theme macros are documented in the public manifest.

## Rendering baseline

The stable global item draw path is intentionally conservative and
allocation-free. Do not reintroduce the earlier global scratch-vector/sort
optimization without a new physical regression campaign.

## Filesystem

On GRAPH 90+E, filesystem operations are executed through
`gint_world_switch()`.

General app configuration/saves default below:

```text
/personalised
```

Screenshots default below:

```text
/Capt
```

The screenshot implementation copies VRAM while gint owns the hardware, then
performs filesystem writes in the OS world.

## Screenshots

Default GUI shortcut in the validation app:

```text
OPTN
```

Default filename:

```text
image
```

Default output:

```text
/Capt/image.bmp
```

The output is a 24-bit BMP. The filename prompt closes before the framebuffer
is captured.
