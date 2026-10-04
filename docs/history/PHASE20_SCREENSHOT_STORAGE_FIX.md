# Phase 20 — Screenshot storage / reboot fix

## Diagnosis from the calculator-generated BMP

The failing `image.bmp` had:

```text
BMP header declared size : 266166 bytes
Actual file size         : 8370 bytes
```

For a 396x224 24-bit BMP:

```text
row size = 396 * 3 = 1188 bytes
54 + 7 * 1188 = 8370 bytes
```

So the calculator successfully wrote the BMP header and exactly seven complete
scanlines, then rebooted while extending the file for the next scanline.

The BMP format/header itself is therefore coherent; the file is truncated
because execution stopped during the write.

## Storage location

Screenshots no longer use the application's `/personalised` configuration root.

Default screenshot path:

```text
/Capt/<name>.bmp
```

Example:

```text
/Capt/image.bmp
```

This is storage memory, not `@MainMem`.

`@MainMem` is the USB representation of calculator main memory and is not used
for arbitrary BMP screenshots.

## Preallocation

Phase 19 grew the file one scanline at a time:

```text
54 bytes
+ 1188
+ 1188
+ ...
```

Phase 20 first reserves the final BMP size with `lseek()` + one byte write:

```text
266166 bytes reserved once
```

then seeks back to byte zero and writes the BMP header and scanlines into the
already-sized file.

This avoids repeated file-system growth while the GUI is running.

## RAM

The implementation is still streaming and only allocates one 1188-byte
scanline. It does not allocate a complete 266 KiB screenshot in RAM.
