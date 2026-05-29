# GivetakeJam BASIC
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![License](https://img.shields.io/badge/license-IchigoJam-green)
![Version](https://img.shields.io/badge/version-v1.6.1-brightgreen)
![Status](https://img.shields.io/badge/status-stable-success)


This version enhances functionality while maintaining compatibility with the original IchigoJam P BASIC by expanding program capacity, expanding array variables, improving external EEPROM support, and adding NEC-style infrared reception commands and environmental measurement commands.

---

## Features / Changes

- Expanded program size from 1024 bytes to 4096 bytes per program.

- Reduced internal storage slots from 100 to 25 (Flash-based).

- Added extended array variable area:
  - Original: [0] – [101]
  - Extended: [102] – [357]
  - New array region mapped at #C00

- Fixed array-related issues:
  - Correct handling of extended array area
  - Fixed FOR/NEXT behavior with array variables
  - CLV / CLEAR now properly reset extended array region
  - Added support for indirect array access ([[x]])

- Improved FILES command behavior:
  - FILES     → internal storage only (0–24)
  - FILES0    → internal + external EEPROM (100–131 if detected)
  - FILES n   → shows 0–n (skips unused 25–99 range)

- External EEPROM support enhanced:
  - Detection via I2C address 0x50 (ACK-based)
  - Unified handling for 24LC64 / 24LC256 / 24FC1025

- EEPROM write reliability improved:
  - Uses safe 32-byte write chunks
  - Prevents page boundary corruption across different devices

- Moved LIST area to #E00

- Internal memory layout updated:
  - #000 CHAR
  - #700 PCG
  - #800 VAR
  - #900 VRAM
  - #C00 VAR2 (new)
  - #E00 LIST (4096 bytes)

- Added IR.IN command for NEC infrared reception with HX1838-compatible modules
  - Syntax
    ```sh
      IR.IN port,[n]
  - Description
   This command waits for a NEC-format infrared signal from the specified input port, decodes it, and stores the result into array variables starting at [n].
  - Result Array Layout
    - [n+0] = raw byte 1
    - [n+1] = raw byte 2
    - [n+2] = raw byte 3
    - [n+3] = raw byte 4
    - [n+4] = repeat flag
    - [n+5] = error code
    - [n+6] = mode
      - 0 = standard NEC
      - 1 = NEC extended
  - Example IR_IN_TEST.BAS
  - Example Output 807F01FE
  - Notes
    - HX1838 output is assumed to be idle HIGH
    - The 38kHz carrier is already demodulated by the receiver module
    - For stable operation, error filtering and repeat filtering are recommended
    - The NEC decode section is protected after leader detection for timing stability


- Added the command ENV.IN to measure temperature, humidity, and atmospheric pressure using the AHT20 and BMP280 modules.

- Syntax:
``sh
      ENV.IN [n]
  - Description:
   Receives measurement data from the AHT20 and BMP280 connected to the I2C line and stores the decoded result in an array variable starting with [n].
  - Result Storage Location
    - [n+0] = Temperature (AHT20, 0.1°C units)
    - [n+1] = Humidity (AHT20, 0.1%RH units)
    - [n+2] = Atmospheric Pressure (BMP280, 0.1hPa units)
    - [n+3] = Temperature (BMP280, 0.1°C units)
    - [n+4] = Error Code
      - 0 = Normal
      - 1 = AHT20 not found
      - 2 = BMP280 not found (※Not a fatal error)
      - 3 = AHT20 measurement failed
      - 4 = BMP280 read failed
      - 5 = BMP280 chip ID invalid
    - [n+5] = Status Flag
      - bit0 = AHT20 Normal
      - bit1 = BMP280 Normal
- Example Program: ENV_IN_TEST.BAS

- Updated VER() to 16112 for build identification
  - Example BASIC
    ```sh 
     ? VER()
     16114
    ```
 ---

#GivetakeJam P is a "4K version of IchigoJam P", created by modifying the original IchigoJam P source code.  
 Therefore, you must first prepare a build environment where "IchigoJam P can be compiled correctly".

## Build Environment
- Install "CMake" and "GCC"

---

## IchigoJam_P Build Environment
- Create the build environment inside the "IchigoJam_P" directory.

- Place the following libraries "directly under the `IchigoJam_P` directory":
    - "IchigoJam_BASIC"  
        Copy the `IchigoJam_BASIC` directory located one level above.
    - "pico-sdk"  
        https://github.com/NaturalStyle/pico-sdk.git  (must use commit `196662b`, and update submodules ``` git submodule update --init --recursive ```)(Using a different commit may cause incorrect behavior)  
    - "pico-extras"  
        https://github.com/raspberrypi/pico-extras.git  (development used commit `09c64d5`) 
    - "PicoDVI"  
        https://github.com/mlorenzati/PicoDVI.git  (must use commit "579eecc"  )(Using a different commit may cause incorrect behavior)

---

## Environment Variables
- Set the following paths:
    ```sh
    PICO_SDK_PATH=foo/IchigoJam_P/pico-sdk
    PICO_EXTRAS_PATH=foo/IchigoJam_P/pico-extras
    ````

For more details, see the official Raspberry Pi documentation:
[Getting started with Raspberry Pi Pico] https://datasheets.raspberrypi.com/pico/getting-started-with-pico-JP.pdf

---

## Build Instructions
```sh
cd IchigoJam_P
mkdir build
cd build
cmake ..
make
```

When `IchigoJam_P.uf2` is generated, write it to the Raspberry Pi Pico.

---

## Troubleshooting
If something goes wrong, delete the `build` directory and start over  
This is particularly effective if video output issues occur  

---

## TODO
Add support for higher resolutions (to enable use with TVs)  

---

## Applying the 4K Version (GivetakeJam P)
To convert IchigoJam P into the 4K version, "overwrite the following files" in each corresponding directory of IchigoJam_P with the files from the GivetakeJam_P directory:

* "IchigoJam_BASIC"
  * `basic.h`
  * `ram.h`
  * `tokens_v1.5.h`

* "src"

  * `config.h`
  * `i2ceeprom.h`
  * `storage.h`

Build in the same way as IchigoJam P.
When `IchigoJam_P.uf2` is created, write it to the Pico.

---

## Firmware File
* "IchigoJam_P.uf2"
    4K version firmware file

### Checksums

* "SHA-256"
  `8C5F00629A0658B1B815B549B754F257A34DDB45CAFC131F4E4AB596C037569B`

* "MD5"
  `327331C46DDF7C06408C4E95AA98D395`

* "SHA-1"
  `C8544B114B293DBF27BD3738DC0F289F317FA6C4`

---

## Test Program

* "ARRAY_VAR_TOTAL_TEST.BAS"
  Array variable test program
  This firmware has been verified by confirming that this test program finishes with:

```
ALL OK
```
## Screenshot

### Overall system configuration
![screenshot](./docs/IMG_2095up.png)

### - Display details ( name, HELP command：Memory map, FILES command )
![screenshot](./docs/IMG_2159up.png)

### IchigoJam_P compatible board (MECHTARIAT PJ convex)
![screenshot](./docs/IMG_2168up.png)

---

## License

* Copyright © 2014–2025 the IchigoJam authors.
  All rights reserved.
  MIT License.
        https://ichigojam.net/license

---

## Trademarks

* "IchigoJam" is a registered trademark of "jig.jp Co., Ltd."
* "Raspberry Pi" is a registered trademark of the "Raspberry Pi Foundation"

---

## Terms of Use

Please download this software only after agreeing to the terms of use.
Distribution to third parties requires a license.

For details, see:
        https://pcn.club/sp/ijp/

* IchigoJam Royalty-Free Program License
        https://ichigojam.net/ichigojam-license.pdf

---

## Known limitations / Notes

- Program size is limited to 4096 bytes per slot.

- Internal storage provides 25 program slots (0–24).

- External EEPROM file numbers are fixed to 100–131 for display (FILES0).
  The actual number of usable slots depends on the EEPROM capacity:
  - 24LC64   → approx. 2 slots
  - 24LC256  → approx. 8 slots
  - 24FC1025 → up to 32 slots

- External EEPROM detection is performed by checking I2C address 0x50.
  If the device responds (ACK), FILES0 will include the external range.

- EEPROM write operations use safe 32-byte chunks to support multiple devices
  (24LC64 / 24LC256 / 24FC1025) with a single firmware build.

- For stable IR reception, repeat filtering is recommended in BASIC
---
![Release](https://img.shields.io/github/v/release/IchigoJam/ichigojam-firm)
![Downloads](https://img.shields.io/github/downloads/IchigoJam/ichigojam-firm/total)

Givetakewinwin
Created on May 29, 2026
