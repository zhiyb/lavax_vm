# LavaX VM bytecode documentation

This is documentation about the .lav LavaX VM file format.

This information is extracted from:\
https://github.com/leesoft-mirage/LavaXVM

## File header

First 16-bytes in the file are the special file header.

| Byte index | Example HEX | Description |
|---|---|---|
| 0-3 | 4c 41 56 12 | File signature (`b'LAV\x12'`) |
| 4-7 | 00 00 00 00 | Reserved, keep as 0 |
| 8 | 40 | Configuration byte 0 |
| 9 | 14 | Window size: width |
| 10 | 0f | Window size: height |
| 11 | 00 | Reserved, keep as 0 |
| 12-15 | 00 00 00 00 | Reserved, keep as 0 |

### Configuration byte 0

| Bit index | Description |
|---|---|
| 7,4 | RamBits |
| 6,5 | Graphic mode |
| 0 | Pen input (1) / Keyboard input (0) |

#### RamBits

| Value | Description |
|---|---|
| 0x00 | RamBits = 16 |
| 0x10 | RamBits = 32 |
| 0x80 | RamBits = 24 |

This limits pointer value.

#### Graphic mode

| Value | Description |
|---|---|
| 0x00 | Mode 1 (Monochromatic), FG colour 15 |
| 0x40 | Mode 4 (16-colour), FG colour 15 |
| 0x60 | Mode 8 (256-colour), FG colour 255 |

### Window size

    width = (byte 9) * 16

`width` clamps to `[160, 320]`

    height = (byte 10) * 16

`height` clamps to `[80, 240]`

## General information

(V)RAM size 0x1000000 / 16MiB.

Harvard architecture: Separate instrcution & data memory.

Instruction memory address pointer size: `u24`.\
Data memory address pointer size: `u16` or `u24`.

Little-endianness.

Text buffer offset `0x1400`.

(Eval/generic) stack offset `0x1b00`.\
Stack is 4-byte aligned.

String stack offset `0x1c00`.\
String stack is 1-byte aligned.\
String stack is restricted to 768 bytes, wraps around.

Boolean values:\
`LTRUE = 0xffffffff = -1`\
`LFALSE = 0`

## Reset initialisation

PC set to offset 16 (just after file header).

Clear screen.
