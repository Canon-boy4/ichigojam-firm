# Givetake BASIC  
### Extended IchigoJam BASIC for Raspberry Pi Pico

![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![Version](https://img.shields.io/badge/VER()-16112-brightgreen)
![Status](https://img.shields.io/badge/status-stable-success)

---

## Overview

GivetakeJam BASIC is an extended version of IchigoJam BASIC for Raspberry Pi Pico.

This firmware enhances program capacity, expands array variables, improves external EEPROM handling, and adds a new infrared receiver command while maintaining compatibility with the original IchigoJam BASIC design.

---

## 概要

Givetake BASIC は Raspberry Pi Pico 向け IchigoJam BASIC の拡張版です。

プログラム容量の拡張、配列変数の拡張、外部EEPROM対応の改善、赤外線リモコン受信コマンドの追加を行い、従来の IchigoJam BASIC との互換性を維持しています。

---

## Version

`VER()` returns **16112** in this extended build.

### Version information
- `16100` : original base version
- `16112` : Givetake BASIC extended build

---

## バージョン

この拡張版では、`VER()` の返り値は **16112** です。

### バージョン情報
- `16100` : 元のベース版
- `16112` : Givetake BASIC 拡張版

---

## Features / Changes
- IchigoJam BASIC 1.6.1 command:  full compatible
- Expanded program size from 1024 bytes to 4096 bytes
- Reduced internal storage slots from 100 to 25
- Extended array variables from [0]–[101] to [0]–[357]
  - `[0..101]`  : VAR (#800-#8CA)
  - `[102..357]`: VAR2 (#C00-#DFE)
- Moved LIST area to #E00
- Fixed FOR/NEXT behavior for extended array variables
- Fixed CLV / CLEAR so that extended array area is also cleared
- External EEPROM support (I2C)
- Improved FILES command behavior
- Added IR.IN command for NEC infrared reception with HX1838-compatible modules
- Updated VER() to 16112 for build identification
- Built-in HELP memory map updated:
  - #000 CHAR
  - #700 PCG
  - #800 VAR
  - #900 VRAM
  - #C00 VAR2
  - #E00 LIST

---

## 機能 / 変更点
- IchigoJam BASIC 1.6.1 コマンドは、すべて互換性があります。
- プログラム容量を 1024バイト から 4096バイト に拡張しています。
- 結果的に、内部保存本数が 100本 から 25本 に変更になります。
- 配列変数を [0]–[101] から [0]–[357] に拡張しています。
  - `[0..101]`  : VAR (#800-#8CA)
  - `[102..357]`: VAR2 (#C00-#DFE)
- LIST 領域を #E00 へ移動しています。
- 拡張配列での FOR/NEXT 動作を修正しています。
- CLV / CLEAR で拡張配列領域も初期化されるよう修正しています。
- 外部拡張 EEPROM をサポートしています。 (I2C)
- FILES コマンドの挙動を改善しています。
- HX1838系モジュール向け NEC赤外線受信コマンド IR.IN を追加しています。
- 拡張版識別のため VER() を 16112 に変更しています。
- HELP コマンドによるメモリーマップ表示を修正しています。
  - #000 CHAR
  - #700 PCG
  - #800 VAR
  - #900 VRAM
  - #C00 VAR2
  - #E00 LIST
