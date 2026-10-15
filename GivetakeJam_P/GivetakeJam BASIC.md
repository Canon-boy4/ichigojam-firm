# GivetakeJam BASIC  
### Extended IchigoJam BASIC for Raspberry Pi Pico

![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![Version](https://img.shields.io/badge/VER()-16131-brightgreen)
![Status](https://img.shields.io/badge/status-RP2350%20hardware%20tested-success)

## Overview / 概要

**GivetakeJam BASIC** is an extended version of IchigoJam BASIC for Raspberry Pi Pico.

This firmware enhances program capacity, expands array variables, improves external EEPROM handling, and adds NEC infrared reception, environmental sensing, color display / palette / attribute commands, and program storage on SD cards while maintaining compatibility with the original IchigoJam BASIC design.

**GivetakeJam BASIC** は Raspberry Pi Pico 向け IchigoJam BASIC の拡張版です。  
プログラム容量の拡張、配列変数の拡張、外部 EEPROM 対応の改善、NEC 赤外線受信コマンド、環境測定コマンド、カラー表示・パレット変更・色属性コマンドの追加、およびSDカードへのプログラムの保存機能の追加を行い、従来の IchigoJam BASIC との互換性を維持しています。

## Version / バージョン

The original IchigoJam BASIC version remains **1.6.1**. The current extension identifiers are `16116` for RP2040 and `16131` for RP2350.

- `16100` : original base version
- `16112` : infrared receiver build
- `16114` : environmental measurement build
- `16115` : fixed `WS.LED` array access for extended arrays and added bounds checking.
- `16120` : RP2350 build supports HSTX DVI video output.
- `16121` : RP2350 build extended arrays to `[358]..[1125]`
- `16122` : RP2350 HSTX DVI dirty-row rendering, DRAW redraw fix, VIDEO resume redraw fix, and Japanese keyboard default when unset.
- `16123` : RP2350 build fixed `WS.LED` array access for extended arrays and added bounds checking.
- `16124` : RP2350 build added MSX-style `COLOR f[,b[,c]]` command for HSTX DVI text output.
- `16125` : Added `PAL`, `PAL RESET`, `PAL(n)`, `ATTR`, and `ATTR(x,y)` for HSTX DVI palette and text attribute control.
- `16126` : RP2350 build refreshes HSTX DVI display after `POKE` writes to VRAM and PCG.
- `16127` : RP2350 build refreshes all wrapped display rows when editing long listed lines.
- `16116` : RP2040 build fixed `STOP` command to report `Break in line` instead of `Syntax error in line`.
- `16128` : RP2350 build fixed `STOP` command to report `Break in line` instead of `Syntax error in line`.
- `16129` : Added `SDON` and `SDOFF` for program storage on SD cards (Pico 2 / RP2350 hardware tested; RP2040 SD support unverified).
- `16130` : Changed CS to IN3 / GPIO6 with direct active-LOW control and a 10kΩ pull-up.
- `16131` : Added BAS export/import, single-file-per-slot format replacement, deletion on empty SAVE, FILES format marks, and IJB-priority loading for duplicate slots.

## Highlights / 主な特徴

- 4096-byte program size
- Internal storage slots RP2040:`25`, RP2350:`100`
- Extended arrays RP2040:`[102]..[357]`, RP2350:`[102]..[1125]`
- `VAR2` area at `#C00`
- `LIST` area at RP2040:`#E00`, RP2350:`#1400`
- `IR.IN` NEC infrared receiver command
- `ENV.IN` AHT20 + BMP280 environment sensor command
- `COLOR` MSX-style text color command for RP2350 HSTX DVI output
- External EEPROM support (`24LC64` / `24LC256` / `24FC1025`)
- RP2350 build: default keyboard layout is Japanese when the keyboard setting flash area is unset.
- SD expansion: FAT32 program slots `200..299`, 4096-byte IJB files or variable-length BAS files (up to 8192 bytes; converted programs up to 4096 bytes), 100kHz SPI, and runtime `SDON` / `SDOFF` switching. Pico 2 / RP2350 hardware tested; RP2040 SD support remains unverified.

## SD Program Formats / SD保存形式

`SAVE200` saves binary `P200.IJB`; `SAVE200,1` saves editable text `P200.BAS`. Successful SAVE removes the other format for the same slot. Saving an empty program deletes both formats for that slot.

LOAD/LRUN automatically read IJB when present, otherwise BAS. If a PC has placed both formats on the card, LOAD/LRUN prefer IJB and retain both files. FILES shows BAS with `*`, IJB with a space, and a duplicate slot with `! [duplicate]`.

英数字はPCで編集できます。日本語・独自記号の文字コード変換は未対応です。詳しい形式・制限は日英READMEを参照してください。

## Memory Map / メモリーマップ

```text
#000 CHAR
#700 PCG
#800 VAR
#900 VRAM
#C00 VAR2
#E00 LIST (RP2040)
#1400 LIST (RP2350)
```

## Screenshots / スクリーンショット

### Runtime Environment
![Runtime Environment](./docs/IMG_4677up.png)

### Display Examples
![Display Examples](./docs/IMG_4679up.png)

### Compatible Board
![Compatible Board](./docs/IMG_4684up.png)

### COLOR command used game
![BASIC Game](./docs/IMG_4685up.png)
![BASIC Game](./docs/IMG_4690up.png)

### SD Card Expansion Board Circuit
![SD Card Expansion Board Circuit](./docs/MSD_MemCard101.png)

## Main Documents

- English: `README.md`
- 日本語: `README_JA.md`
