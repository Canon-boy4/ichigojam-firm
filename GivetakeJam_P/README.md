# GivetakeJam BASIC

![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![License](https://img.shields.io/badge/license-IchigoJam-green)
![Version](https://img.shields.io/badge/version-16124-brightgreen)
![Status](https://img.shields.io/badge/status-stable-success)

Extended IchigoJam BASIC for Raspberry Pi Pico with a 4096-byte program area, expanded arrays, improved external EEPROM support, NEC infrared reception, and environmental sensing.

## Highlights

- Full compatibility with IchigoJam BASIC 1.6.1 commands
- Program size expanded from 1024 bytes to 4096 bytes
- Internal flash storage slots
  - Raspberry Pi Pico / RP2040: 25 slots
  - Raspberry Pi Pico 2 / RP2350: 100 slots
- Array variables expanded from `[0]..[101]` to RP2040:`[0]..[357]`, RP2350:`[0]..[1125]`
- Added `VAR2` area at `#C00`
- LIST area moved to RP2040:`#E00`, RP2350:`#1400`
- Improved external EEPROM handling
- Added `IR.IN` command for NEC infrared reception
- Added `ENV.IN` command for AHT20 + BMP280 environment sensing
- Fixed `WS.LED` array access for extended arrays and added bounds checking
- `VER()` returns `16124` on Raspberry Pi Pico 2 / RP2350
- Added MSX-style `COLOR f[,b[,c]]` command for RP2350 HSTX DVI text output

## Memory Map

```text
#000 CHAR
#700 PCG
#800 VAR
#900 VRAM
#C00 VAR2
#E00 LIST (RP2040)
#1400 LIST (RP2350)
```

## Features / Changes

- Expanded program size from 1024 bytes to 4096 bytes per program
- Reduced internal storage slots from 100 to 25
- Extended array variables
  - Original: `[0]..[101]`
  - Extended: RP2040: `[102]..[357]`, RP2350: `[102]..[1125]`
  - `VAR`  = `#800..#8CA`
  - `VAR2` = RP2040: `#C00..#DFE`, RP2350: `#C00..#13FE`
- Fixed array-related issues
  - Correct handling of extended array area
  - Fixed `FOR/NEXT` behavior with array variables
  - `CLV` / `CLEAR` now reset the extended array region
  - Added support for indirect array access `[[x]]`
  - Fixed `WS.LED` array access to use the extended BASIC array area correctly
  -  On the RP2350 build, up to 375 WS2812B LEDs can be driven from array elements `[0]..[1124]`
  - `WS.LED` now reports `Index out of range` when the requested LED count exceeds the available BASIC array range.
- Improved `FILES` command behavior
  - `FILES` → internal storage only RP2040(`0..24`), RP2350(`0..99`)
  - `FILES0` → internal + external EEPROM (`100..131` if detected)
  - `FILES n` → shows `0..n` and skips unused `25..99`
- Improved external EEPROM support
  - Detection by I2C address `0x50`
  - Works with `24LC64`, `24LC256`, and `24FC1025`
  - Safe 32-byte write chunks for multi-device support
- Added `IR.IN` command for HX1838-compatible NEC infrared receiver modules
- Added `ENV.IN` command for AHT20 + BMP280 environment sensing
- Built-in `HELP` memory map updated
- `VER()` returns `16115` on Raspberry Pi Pico / RP2040
- `VER()` returns `16124` on Raspberry Pi Pico 2 / RP2350
- Platform identification by `VER(1)`
  - Raspberry Pi Pico / RP2040: `8`
  - Raspberry Pi Pico 2 / RP2350: `9`

## IR.IN Command

`IR.IN` receives NEC-format infrared codes from an HX1838-compatible receiver.

### Syntax

```basic
IR.IN port,[n]
```

### Result Layout

- `[n+0]` = raw byte 0
- `[n+1]` = raw byte 1
- `[n+2]` = raw byte 2
- `[n+3]` = raw byte 3
- `[n+4]` = repeat flag
- `[n+5]` = error code
- `[n+6]` = mode
  - `0` = standard NEC
  - `1` = NEC extended

### Example

```basic
10 ' IR.IN Test Program
20 IR.IN 1,[0]
30 LED 0: ' not busy
40 IF [5]<>0 GOTO 20
50 IF [4]<>0 GOTO 20
60 LED 1: ' busy
70 ? HEX$([0],2);HEX$([1],2);HEX$([2],2);HEX$([3],2)
80 WAIT 5
90 GOTO 20
```

### Example Output

```text
807F01FE
```

### Notes

- HX1838 output is assumed to be idle HIGH
- The 38kHz carrier is already demodulated by the receiver module
- For stable operation, error filtering and repeat filtering are recommended
- The timing-critical decode section is protected after leader detection

## Raspberry Pi Pico 2 / RP2350 Support

The Raspberry Pi Pico 2 / RP2350 build supports HSTX DVI video output.

Confirmed features on Pico 2:

- HSTX DVI video output
  - Optimized HSTX DVI text rendering by updating only dirty screen rows.
  - Fixed DRAW screen update with dirty-row rendering.
- USB keyboard input
  - Default keyboard layout is Japanese when the keyboard setting flash area is unset.
- `BEEP` / `PLAY` sound output on GPIO20
- `VIDEO 0` / `VIDEO 1` display control
  - `VIDEO 0` clears the screen and stops screen updates
  - the HSTX DVI signal itself continues running
  - `VIDEO 1` resumes screen updates and fixed screen redraw after resume.
- `IR.IN` using RP2350 PIO + DMA
  - confirmed on IN1 to IN4
  - result array: `[0]` to `[6]`
  - IR DMA uses channel 10
- Internal flash `SAVE` / `LOAD` / `FILES`
  - Internal flash slots `0` to `99`
- External I2C EEPROM slots `100` to `131`
- `LRUN` from external EEPROM

### Pico 2 IR.IN Example

```basic
10 CLS
20 IR.IN 4,[0]:IF [5]<>0 CONT
30 A=[2] & #FF
40 ?HEX$(A,2)
```

For the Pico 2 HSTX DVI build, HSTX video uses DMA channels 0 and 1.  
The RP2350 `IR.IN` PIO DMA implementation uses DMA channel 10, so it does not conflict with HSTX video DMA.

## COLOR Command

`COLOR` sets text colors for the RP2350 / Pico 2 HSTX DVI build.

### Syntax

```basic
COLOR f
COLOR f,b
COLOR f,b,c
```

- `f`: foreground color, 0-15
- `b`: background color, 0-15
- `c`: border/surrounding color, 0-15

The current implementation affects newly printed text and `CLS`; existing text is not recolored retroactively.

### Example

```basic
CLS
COLOR 15,1,1
?"WHITE"
COLOR 10,1,1
?"YELLOW"
COLOR 8,1,1
?"RED"
```

## ENV.IN Command

`ENV.IN` reads temperature and humidity from AHT20 and pressure from BMP280 over I2C.

### Syntax

```basic
ENV.IN [n]
```

### Result Layout

- `[n+0]` = temperature from AHT20 in 0.1°C units
- `[n+1]` = humidity from AHT20 in 0.1%RH units
- `[n+2]` = pressure from BMP280 in 0.1 hPa units
- `[n+3]` = temperature from BMP280 in 0.1°C units
- `[n+4]` = error code
- `[n+5]` = status flag
  - bit0 = AHT20 OK
  - bit1 = BMP280 OK

### Error Codes

- `0` = success
- `1` = AHT20 not found
- `2` = BMP280 not found (not fatal if AHT20 is available)
- `3` = AHT20 measurement failed
- `4` = BMP280 read failed
- `5` = BMP280 chip ID mismatch

### Example

```basic
10 ' ENV.IN Test Program
20 ENV.IN [0]
30 IF [4]<>0 THEN ? "ERR=";[4]:GOTO 20
40 T=[0]
50 ? "T=";
60 IF T<0 ? "-";:T=-T
70 ? T/10;".";T%10;"C"
80 ? "H=";[1]/10;".";[1]%10;"%"
90 IF [5]=3 THEN ? "P=";[2]/10;".";[2]%10;"hPa"
100 IF [5]=1 THEN ? "P=N/A"
110 WAIT 60
120 GOTO 20
```

## Build Environment

Create the build environment inside the `IchigoJam_P` directory.

### Required tools

- CMake
- GCC

### Required libraries

Place the following under the `IchigoJam_P` directory:

- `IchigoJam_BASIC`
  - Copy the `IchigoJam_BASIC` directory from one level above
- `pico-sdk`
  - Repository: `https://github.com/NaturalStyle/pico-sdk.git`
  - Use commit `196662b`
  - Update submodules:
    ```sh
    git submodule update --init --recursive
    ```
- `pico-extras`
  - Repository: `https://github.com/raspberrypi/pico-extras.git`
  - Development used commit `09c64d5`
- `PicoDVI`
  - Repository: `https://github.com/mlorenzati/PicoDVI.git`
  - Use commit `579eecc`

### Environment variables

```sh
PICO_SDK_PATH=foo/IchigoJam_P/pico-sdk
PICO_EXTRAS_PATH=foo/IchigoJam_P/pico-extras
```

See the Raspberry Pi documentation for details.

## Build

```sh
cd IchigoJam_P
mkdir build
cd build
cmake ..
make
```

When `IchigoJam_P.uf2` is generated, write it to the Raspberry Pi Pico.

## If Something Goes Wrong

Try deleting the `build` directory and rebuilding from scratch.

This is especially effective when video output becomes unstable.

## Files to Overwrite for the 4K Version

Copy the following files from the `GivetakeJam_P` directory into the corresponding `IchigoJam_P` directories:

- `IchigoJam_BASIC`
  - `basic.h`
  - `ram.h`
  - `tokens_v1.5.h`
- `src`
  - `config.h`
  - `i2ceeprom.h`
  - `storage.h`
  - `io.h`

Then build as usual. When `IchigoJam_P.uf2` is generated, write it to the Pico.

## Raspberry Pi Pico / RP2040 Firmware / Checksum
### Write this firmware to Raspberry Pi Pico / RP2040.
- `IchigoJam_P.uf2`
  - `SHA-256`: `CAEC5D43C6B2C485FFE31619B1CC472A64A82E77C53030E415C03E43CB604D6B`
  - `MD5`: `98BDA73D4EEF1A4CE3BE179E88A93098`
  - `SHA-1`: `A11FCDCB20C2FABF54DDD968EB3895AFF7537FD6`

## Raspberry Pi Pico 2 / RP2350 Firmware / Checksum
### Write this firmware to Raspberry Pi Pico 2 / RP2350.
- `GivetakeJam_P.uf2`
  - `SHA-256`: `957883406A068C553708B3829337DE71A38213B1E2C342FF8B3439CA481A9D91`
  - `MD5`: `054450F11C4857C4F008643B7DDAD7F1`
  - `SHA-1`: `EDA776EB7D51E19A2E03DE18BA171DBD2CF82BC4`

## Test Programs

- `ARRAY_VAR_TOTAL_TEST.BAS`
  - Comprehensive array-variable test
  - Verified to finish with `ALL OK`
- `IR_IN_TEST.BAS`
  - NEC infrared receiver test
- `ENV_IN_TEST.BAS`
  - AHT20 + BMP280 environment sensor test

## Screenshots

### Runtime Environment
![Runtime Environment](./docs/IMG_2095up.png)

### Display Examples
![Display Examples](./docs/IMG_2159up.png)

### IchigoJam_P Compatible Board (MECHTARIAT PJ 凸)
![Compatible Board](./docs/IMG_2168up.png)

## Known Limitations / Notes

- Program size is limited to 4096 bytes
- Internal storage provides 25 slots
- External EEPROM display range is fixed to `100..131`
  - `24LC64` → 2 slots
  - `24LC256` → 8 slots
  - `24FC1025` → 32 slots
- Actual usable slots depend on EEPROM capacity
- EEPROM detection uses I2C address `0x50`
- 16-bit signed integers only (`-32768..32767`)
- No floating point support
- Labels ending with digits may cause syntax errors
- Division is integer-only
- For stable IR reception, repeat filtering is recommended in BASIC

## License

Copyright 2014-2025 the IchigoJam authors. All rights reserved.  
MIT license.  
https://ichigojam.net/license

## Trademark

- IchigoJam is a registered trademark of jig.jp Co., Ltd.
- Raspberry Pi is a registered trademark of the Raspberry Pi Foundation.

## Terms of Use

Please download after agreeing to the terms of use.  
A license is required for redistribution to third parties.

- Terms of use: https://pcn.club/sp/ijp/
- IchigoJam Royalty-Free Program License: https://ichigojam.net/ichigojam-license.pdf

---

Givetakewinwin

- Created: 2026-05-29
- Updated on 2026-07-18 for Raspberry Pi Pico 2 / RP2350 HSTX DVI support.
- Updated on 2026-07-29 for Raspberry Pi Pico 2 / Array variables 1125.
- Updated on 2026-08-10: Fixed `WS.LED` array access and added bounds checking.
- Updated on 2026-08-14 for Raspberry Pi Pico 2 / Added MSX-style `COLOR` command.