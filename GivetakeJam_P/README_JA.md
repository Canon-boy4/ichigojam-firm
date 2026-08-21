# GivetakeJam BASIC
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![License](https://img.shields.io/badge/license-IchigoJam-green)
![Version](https://img.shields.io/badge/version-16125-brightgreen)
![Status](https://img.shields.io/badge/status-stable-success)
## プログラム容量の拡張、配列変数の拡張、外部EEPROM対応の改善、NEC方式の赤外線受信コマンド、環境測定コマンド、カラー表示・パレット変更・色属性コマンドの追加を行い、従来のIchigoJam P BASICの互換性を維持しながら機能強化しています。
### Pico 2 / RP2350版の配列領域拡張
Pico 2 / RP2350版では、増加RAMを利用して配列変数領域 VAR2 を拡張しています。
## 機能 / 変更点
- プログラム容量を 1024 → 4096バイト に拡張
- 内部保存プログラム本数
  - Raspberry Pi Pico / RP2040版: 25本
  - Raspberry Pi Pico 2 / RP2350版: 100本
- 配列変数を拡張：
  - オリジナル：[0] ～ [101]
  - 追加した変数：RP2040版[102] ～ [357]、RP2350版[102] ～ [1125]
    - #C00 に VAR2 追加
- 配列関連の不具合修正：
  - FOR/NEXT 動作を修正
  - CLV / CLEAR で拡張配列領域も初期化されるよう修正
  - 間接参照 [[x]] に対応
  - `WS.LED` が拡張配列領域を正しく参照するよう修正。
  - RP2350版では最大375個のWS2812Bを配列 `[0]..[1124]` から送信可能。
  - `WS.LED` のLED数指定が配列上限を超える場合、`Index out of range` を返すよう修正。
- FILESコマンド改善：
  - FILES → RP2040版:0～24、RP2350版:0～99
  - FILES0 → 外部EEPROMがあれば 100～131
  - FILES n → 0～n（RP2040版は25～99をスキップ）
- 外部EEPROM 対応の改善
  - 外部EEPROM検出を I2Cアドレス 0x50 のACKで実装
  - 24LC64 / 24LC256 / 24FC1025 に単一バイナリで対応
  - EEPROM書き込みを32バイト分割にして安全性向上
- LIST エリアを RP2040版:#E00 に移動、RP2350版:#1400 に移動
- メモリマップ更新：
  - #000 CHAR
  - #700 PCG
  - #800 VAR
  - #900 VRAM
  - #C00 VAR2
  - #E00 LIST (4096バイト) RP2040版
  - #1400 LIST (4096バイト) RP2350版
- HX1838系赤外線リモコン受信モジュール向け NEC方式赤外線受信コマンド IR.IN を追加
  - 書式
    ```sh
      IR.IN port,[n]
    ```
  - 説明
    - 指定した入力ポートから NEC形式の赤外線信号を受信し、デコード結果を [n] から始まる配列変数に格納します。
  - 結果の格納先
    - [n+0] = 生データ 1 バイト目
    - [n+1] = 生データ 2 バイト目
    - [n+2] = 生データ 3 バイト目
    - [n+3] = 生データ 4 バイト目
    - [n+4] = repeat フラグ
    - [n+5] = エラー コード
    - [n+6] = モード
      - 0 = NEC 標準
      - 1 = NEC 拡張
  - 使用例
    ```basic
    10 CLS
    20 IR.IN 1,[0]:IF [5]<>0 CONT
    30 A=[2] & #FF
    40 ?HEX$(A,2)
    ```
  - 表示結果は使用するリモコンのボタンにより変わります。
  - 注意
    - HX1838の出力は待機時 HIGH を前提としています
    - 38kHz搬送波は受光モジュール側で復調済みです
    - 安定動作のため、エラー除外と repeat除外を推奨します
    - リーダーコード検出後の NECデコード区間では、安定化のため割り込みの影響を抑えています
- Raspberry Pi Pico 2 / RP2350 対応
  - Pico 2 / RP2350 ビルドでは、HSTX DVI表示に対応しています。
  - Pico 2で確認済みの機能
    - HSTX DVI表示をdirty行更新方式に変更し、画面更新処理を軽量化。
    - DRAW命令による描画がdirty行更新で反映されるよう修正。
    - USBキーボードを使用、KBD設定が未設定の場合は日本語キーボード設定に変更。
    - GPIO20での`BEEP` / `PLAY`音声出力
    - `VIDEO 0` / `VIDEO 1`表示制御
      - `VIDEO 0`で画面を消し、画面更新を停止。
      - HSTX DVI信号自体は出力を継続。
      - `VIDEO 1`で画面更新を再開、画面全体を再描画するよう補強。
    - RP2350 PIO + DMAによる`IR.IN`
      - IN1〜IN4で確認済み
      - 結果配列は`[0]`〜`[6]`
      - IR用DMAはチャネル10を使用
    - 内蔵フラッシュの`SAVE` / `LOAD` / `FILES`
      - 内蔵フラッシュのスロット`0`〜`99`
    - 外部I2C EEPROMのスロット`100`〜`131`
    - 外部EEPROMからの`LRUN`
  - `IR.IN`使用例
    ```basic
    10 CLS
    20 IR.IN 4,[0]:IF [5]<>0 CONT
    30 A=[2] & #FF
    40 ?HEX$(A,2)
    ```
  - Pico 2 HSTX DVIビルドでは、HSTX表示にDMAチャネル0/1を使用します。
  - RP2350版`IR.IN`のPIO DMAはDMAチャネル10を使用するため、HSTX表示用DMAとは競合しません。
- RP2350 / Pico 2 HSTX DVI テキスト出力向けに、MSX風の `COLOR f[,b[,c]]` 命令を追加
  - Raspberry Pi Pico / RP2040版では非対応で、実行すると `Syntax error` になります。
  - `f`: 前景色 0〜15
  - `b`: 背景色 0〜15
  - `c`: 周辺色 0〜15
  - 現在の実装では、以後に表示する文字と `CLS` に反映されます。既に表示済みの文字色は変更しません。
  - 使用例
    ```basic
    CLS
    COLOR 15,1,1
    ?"WHITE"
    COLOR 10,1,1
    ?"YELLOW"
    COLOR 8,1,1
    ?"RED"
    ```
- RP2350 / Pico 2 HSTX DVI テキスト出力向けに、`PAL` 命令を追加
  - `PAL` は、色番号に割り当てられている RGB332 パレット値を変更または取得します。
  - Raspberry Pi Pico / RP2040版では非対応で、実行すると `Syntax error` になります。
  - 書式
    ```basic
    PAL n,v
    V=PAL(n)
    PAL RESET
    ```
  - `n`: 色番号 0〜15
  - `v`: RGB332パレット値 0〜255
  - `PAL n,v`: 色番号 `n` の RGB332 パレット値を設定
  - `PAL(n)`: 色番号 `n` の現在の RGB332 パレット値を取得
  - `PAL RESET`: ファームウェア内蔵のデフォルトパレットに戻す
  - RGB332形式
    ```text
    bit7..5 : 赤 0..7
    bit4..2 : 緑 0..7
    bit1..0 : 青 0..3
    ```
  - 使用例
    ```basic
    10 CLS
    20 ?PAL(10)
    30 PAL 10,#E0
    40 ?PAL(10)
    50 PAL RESET
    60 ?PAL(10)
    70 COLOR 10,1,1
    80 ?"PAL RESET OK"
    ```
- RP2350 / Pico 2 HSTX DVI テキスト出力向けに、`ATTR` 命令を追加
  - `ATTR` は、すでに表示されている文字セルの色属性を変更または取得します。
  - Raspberry Pi Pico / RP2040版では非対応で、実行すると `Syntax error` になります。
  - 書式
    ```basic
    ATTR x,y,a
    A=ATTR(x,y)
    ```
  - `x,y`: 画面座標
  - `a`: 色属性値 0〜255
  - 下位4bit: 前景色
  - 上位4bit: 背景色
  - 属性値
    ```basic
    A=FG+BG*16
    FG=A&#F
    BG=A>>4
    ```
  - 使用例
    ```basic
    10 CLS
    20 COLOR 15,1,1
    30 LC 5,5:?"A"
    40 ?ATTR(5,5)
    50 ATTR 5,5,15+8*16
    60 ?ATTR(5,5)
    ```
  - この例では、表示済みの `A` を赤背景・白文字に変更します。
- AHT20とBMP280モジュールを使って温湿度と気圧を測定するコマンド ENV.IN を追加
  - 書式
    ```sh
      ENV.IN [n]
    ```
  - 説明
    - I2CラインにつないだAHT20とBMP280から 測定データを受信し、デコード結果を [n] から始まる配列変数に格納します。
  - 結果の格納先
    - [n+0] = 温度 (AHT20, 0.1℃単位)
    - [n+1] = 湿度 (AHT20, 0.1%RH単位)
    - [n+2] = 気圧 (BMP280, 0.1hPa単位)
    - [n+3] = 温度 (BMP280, 0.1℃単位)
    - [n+4] = エラー コード
      - 0 = 正常
      - 1 = AHT20 が見つからない
      - 2 = BMP280 が見つからない（※致命エラーにはしない）
      - 3 = AHT20 測定失敗
      - 4 = BMP280 読み出し失敗
      - 5 = BMP280 チップID不正
    - [n+5] = 状態フラグ
      - bit0 = AHT20 正常
      - bit1 = BMP280 正常
  - 使用例 プログラム ENV_IN_TEST.BAS
- 拡張版識別のため `VER()` を変更
  - Raspberry Pi Pico / RP2040版: `16115`
  - Raspberry Pi Pico 2 / RP2350版: `16125`
- `COLOR f[,b[,c]]` によるHSTX DVIテキストカラー指定
- `PAL n,v` / `PAL RESET` / `PAL(n)` によるRGB332パレット制御
- `ATTR x,y,a` / `ATTR(x,y)` による画面文字セルの色属性制御
  使用例 BASIC
  Raspberry Pi Pico / RP2040版:
  ```sh
  ? VER()
  16115
  ? VER(1)
  8
  ```
  Raspberry Pi Pico 2 / RP2350版:
  ```sh
  ? VER()
  16125
  ? VER(1)
  9
  ```
- `VER(1)` によるプラットフォーム識別
  - Raspberry Pi Pico / RP2040版: `8`
  - Raspberry Pi Pico 2 / RP2350版: `9`
---
#  IchigoJam P BASICのソースコードを変更して４K版にしたものです。よって、まずはIchigoJam P BASICがコンパイルできる環境を作成します。
## 環境構築
- "CMake"と"GCC"をインストールする
## IchigoJam_P のコンパイル環境
- IchigoJam_P のディレクトリーで作成します。
- 以下のライブラリを`IchigoJam_P`ディレクトリ直下に置く
    - "IchigoJam_BASIC": 1つ上の階層にある`IchigoJam_BASIC`ディレクトリをコピーする
    - "pico-sdk": https://github.com/NaturalStyle/pico-sdk.git `196662b`のコミットを使用(別のコミットを使用すると正しく動作しない)
    - "pico-extras": https://github.com/raspberrypi/pico-extras.git 開発時は`09c64d5`のコミットを使用
    - "PicoDVI": https://github.com/mlorenzati/PicoDVI.git `579eecc`のコミットを使用(別のコミットを使用すると正しく動作しない)
- 以下のパスを通す
    ```sh
    PICO_SDK_PATH=foo/IchigoJam_P/pico-sdk
    PICO_EXTRAS_PATH=foo/IchigoJam_P/pico-extras
    ```
詳細は以下を参照
[Raspberry Pi Picoをセットアップしよう](https://datasheets.raspberrypi.com/pico/getting-started-with-pico-JP.pdf)
## ビルド
```sh
cd IchigoJam_P
mkdir build
cd build
cmake ..
make
```
`IchigoJam_P.uf2`ファイルが作成できたらpicoに書き込む
## 動作がおかしくなったら
`build`ディレクトリを消して作り直してみる
特に映像の出力が正常に行われなくなることが多い
## TODO
高解像度に対応する(TVでも使えるようにするため)
## ４K版にするために GivetakeJam_P のディレクトリーにある以下のファイルを IchigoJam_P の各のディレクトリーに上書きコピーします。
- `IchigoJam_BASIC`
  - `basic.h`
  - `ram.h`
  - `tokens_v1.5.h`
- `src`
  - `config.h`
  - `i2ceeprom.h`
  - `storage.h`
  - `io.h`
- IchigoJam P の時の様にビルドして、`IchigoJam_P.uf2`ファイルが作成できたらPicoに書き込みます。
## Raspberry Pi Pico / RP2040 Firmware / Checksum
### Raspberry Pi Pico / RP2040用の4K版ファームウェアファイルです。
- `IchigoJam_P.uf2`
  - SHA-256: `6F290119F925896602041F9F3A6D4E5E966A5D7A855424FEBC09CBB938215E5E`
  - MD5: `EBE5EF75F4A9350468FCD01FE41C4498`
  - SHA-1: `2DDD653B6BE92F7A082F66D29C4EC83D9F008C04`
## Raspberry Pi Pico 2 / RP2350 Firmware / Checksum
### Raspberry Pi Pico 2 / RP2350用の4K版ファームウェアファイルです。
- `GivetakeJam_P.uf2`
  - SHA-256: `578E1A79E2B885955993356FE32A95CFF68447D564705215EFE04D75124A4591`
  - MD5: `0AC79B4DDC9D2D33B5AFA05795FA7838`
  - SHA-1: `3D329E4141EA872FF1F100AE4074ED8DE4A1E86F`
## Test Programs
- `ARRAY_VAR_TOTAL_TEST.BAS`
  - 配列変数のテストプログラムです。このテストが ALL OK で通ることを確認しています。
- `IR_IN_TEST.BAS`
  - NEC方式の赤外線受信のテストプログラムです。
- `ENV_IN_TEST.BAS`
  - AHT20 + BMP280 環境測定のテストプログラムです。
- `ICHIGO_EATER_COLOR.BAS`
  - COLOR コマンドを使った BASIC ゲームです。
## Screenshot
### 動作環境
![screenshot](./docs/IMG_4677up.png)
### 表示画面 ( 名称, HELP表示：メモリマップ, FILES表示 )
![screenshot](./docs/IMG_4679up.png)
### IchigoJam_P 互換基板 ( GivetakeJam-P )
![screenshot](./docs/IMG_4684up.png)
### COLOR コマンド使用画面 ( ICHIGO EATER COLOR画面 )
![screenshot](./docs/IMG_4685up.png)
![screenshot](./docs/IMG_4690up.png)
## ライセンス / license
    - Copyright 2014-2025 the IchigoJam authors. All rights reserved. MIT license.  https://ichigojam.net/license
## 商標 / Trademark
    - IchigoJamは、株式会社jig.jpの登録商標です。(IchigoJam is a registered trademark of jig.jp co., ltd.)
    - Raspberry Pi は、Raspberry Pi財団の登録商標です。(Raspberry Pi is a registered trademark of the Raspberry Pi Foundation)
## 利用規約に同意の上、ダウンロードしてください。第三者への配布にはライセンスが必要になります。詳しくはこちら。https://pcn.club/sp/ijp/
    - 利用規約（IchigoJamロイヤリティフリープログラム利用規約） https://ichigojam.net/ichigojam-license.pdf
## 既知の制限 / 注意事項
- プログラム最大 4096 バイト
- 内部保存プログラム本数
  - Raspberry Pi Pico / RP2040版: 25本
  - Raspberry Pi Pico 2 / RP2350版: 100本
- 外部EEPROM表示は 100～131固定
  - 24LC64 追加本数 2本
  - 24LC256 追加本数 8本
  - 24FC1025 追加本数 32本
- 実際の使用可能プログラム本数はEEPROM容量依存
- EEPROM検出はI2C アドレス 0x50 の応答で判定
- IR受信を安定させるため、BASIC側で repeat 除外を推奨します
---
ギブテクウインウイン
## 変更履歴
- 2026/05/29 作成
- 2026/07/18 Pico 2 / RP2350 HSTX DVI版を追加
- 2026/07/29 Pico 2 配列変数を 1125 まで増加
- 2026/08/10 `WS.LED` の拡張配列変数へのアクセスと上限チェックを修正
- 2026/08/14 Pico 2 / RP2350 HSTX DVI テキスト出力向けに MSX風 `COLOR` 命令を追加
- 2026/08/22 Pico 2 / RP2350 HSTX DVI テキスト出力向けにパレット変更・色属性命令を追加
![Release](https://img.shields.io/github/v/release/IchigoJam/ichigojam-firm)
![Downloads](https://img.shields.io/github/downloads/IchigoJam/ichigojam-firm/total)
