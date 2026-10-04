# Phase 19 — Screenshot capture

## User flow

The validation application enables:

```text
OPTN
```

When no control currently owns keyboard focus:

```text
OPTN
  -> PromptPopup "Screenshot"
  -> default name: image
  -> EXE
  -> underlying GUI is redrawn
  -> BMP is saved
```

The prompt is not included in the screenshot.

If a widget owns focus, OPTN stays available to that widget (for example Graph).
Press EXIT first, then OPTN.

## Output

Screenshots are saved below the safe application storage root:

```text
/personalised/screenshots/<name>.bmp
```

Default:

```text
/personalised/screenshots/image.bmp
```

## Format

Standard uncompressed 24-bit BMP.

The GRAPH 90+E VRAM is read as RGB565 and converted to BGR888. The writer
streams one scanline at a time, so it does not allocate a complete ~266 KiB
screen buffer.

## API

```cpp
casio::GUI gui;

gui.enableScreenshotCapture(
    KEY_OPTN,
    "image"
);
```

Optional result callback:

```cpp
gui.setScreenshotResultCallback(
    [](bool ok, const std::string& path)
    {
    }
);
```

Manual prompt:

```cpp
gui.openScreenshotPrompt();
```

Low-level capture:

```cpp
std::string path;

auto result =
    casio::Screenshot::saveBmp(
        "image",
        &path
    );
```

Names are sanitized and the path cannot escape the dedicated screenshots
directory.
