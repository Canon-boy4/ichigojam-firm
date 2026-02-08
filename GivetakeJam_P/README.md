# GivetakeJam P
GivetakeJam P is a "4K version of IchigoJam P", created by modifying the original IchigoJam P source code.  
Therefore, you must first prepare a build environment where "IchigoJam P can be compiled correctly".

---

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

* "src"

  * `config.h`

Build in the same way as IchigoJam P.
When `IchigoJam_P.uf2` is created, write it to the Pico.

---

## Firmware File
* "IchigoJam_P.uf2"
    4K version firmware file

### Checksums

* "SHA-256"
  `a044e888e0edd73ac1f650f108d5fdd786ef4a8424b7397830a7a624533260af`

* "MD5"
  `8196bbe7a1f56b6a123e00c665f0e81c`

* "SHA-1"
  `11d5b289829b82127225dc637d84c12d71d6dfef`

---

## Test Program

* "ARRAY_VAR_TOTAL_TEST.BAS"
  Array variable test program
  This firmware has been verified by confirming that this test program finishes with:

```
ALL OK
```

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

Givetakewinwin
Created on February 7, 2026
