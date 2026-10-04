# Phase 21 — Correct filesystem world-switch handling

## What Capt.zip proved

The Phase 20 `Capt/image.bmp` is exactly 54 bytes:

```text
BMP header: valid
Declared size: 266166 bytes
Actual size: 54 bytes
Pixel rows written: 0
```

Phase 19 had reached seven scanlines before rebooting. Phase 20 reached the BMP
header and then rebooted.

That changing failure point is consistent with unsafe Fugue/POSIX access from
the gint world rather than a deterministic BMP-format bug.

## Root cause

On GRAPH 90+E / fx-CG50, filesystem operations must be executed while the OS
owns the hardware state:

```cpp
gint_world_switch(
    GINT_CALL(...)
);
```

Calls such as:

```cpp
open()
write()
read()
mkdir()
unlink()
close()
```

can appear to work without a world switch and then reboot the calculator.

The previous screenshot implementation did not world-switch.

## Screenshot architecture

The screenshot now has two strictly separated phases.

### Gint world

```text
draw GUI
  -> memcpy current RGB565 gint_vram
     into a 177408-byte RAM snapshot
```

No filesystem access occurs here.

### OS world

One single world switch executes:

```text
mkdir /Capt
open /Capt/image.bmp
write BMP header
write 224 BMP scanlines
close
```

No gint display, keyboard, timer or VRAM functions are called in the OS world.

The RAM snapshot is passed to the world-switch callback and converted to BGR888
while being written.

## Why the snapshot is necessary

Calling `dgetpixel()` or reading gint-managed hardware state from inside the OS
world would be invalid.

Conversely, calling Fugue/POSIX file functions from the gint world is unsafe.

The RAM copy is the boundary between the two environments.

## Generic storage fix

The same issue also applied to the library's generic storage layer.

Phase 21 wraps low-level storage operations in a world switch as well:

```text
exists
readText
writeText
appendText
readBinary
writeBinary
appendBinary
remove
createDirectory
```

This also addresses the earlier intermittent reboot observed with
`Test file I/O`.

## Screenshot location

Still:

```text
/Capt/<name>.bmp
```

Default:

```text
/Capt/image.bmp
```
