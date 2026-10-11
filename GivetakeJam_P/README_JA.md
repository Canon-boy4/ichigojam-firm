# GivetakeJam BASIC
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![License](https://img.shields.io/badge/license-IchigoJam-green)
![Version](https://img.shields.io/badge/version-16130-brightgreen)
![Status](https://img.shields.io/badge/status-RP2350%20hardware%20tested-success)
## プログラム容量の拡張、配列変数の拡張、外部EEPROM対応の改善、NEC方式の赤外線受信コマンド、環境測定コマンド、カラー表示・パレット変更・色属性コマンドの追加、およびSDカードへのプログラム保存機能追加を行い、従来のIchigoJam P BASICの互換性を維持しながら機能強化しています。
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
  - SDOFF中のFILES → 内蔵Flash（RP2040版:0～24、RP2350版:0～99）
  - SDOFF中のFILES0 → 内蔵Flash＋応答する外部EEPROM（100～131）
  - RP2350版でSDON中のFILES / FILES0 → SDの保存済み番号200～299
  - FILES n / FILES m,n → 明示した範囲（未割り当て番号をスキップ）
  - FILES200,299 → SDの保存済み番号と先頭行。RAM上のプログラムとFILE()は変更しません。
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
    - Pico 2 HSTX DVIビルドでは、HSTX表示にDMAチャネル0/1を使用します。
    - RP2350版`IR.IN`のPIO DMAはDMAチャネル10を使用するため、HSTX表示用DMAとは競合しません。
  - 内蔵フラッシュの`SAVE` / `LOAD` / `FILES`
    - 内蔵フラッシュのスロット`0`〜`99`
    - 外部I2C EEPROMのスロット`100`〜`131`
    - 外部EEPROMからの`LRUN`
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

- SDカードへのプログラム保存機能を追加
  - `SDON` 命令 : カード初期化・FAT32確認。成功時は `SD ready (FAT32, 100kHz)`
  - `SDOFF` 命令 : CSをHIGHにしてからIN3を内部プルアップ付き入力へ戻し、OUT1～OUT4を出力LOWへ復帰。カード電源は切りません。

  | 命令 | 機能 |
| --- | --- |
| `SAVE200`～`SAVE299` | SDへの新規保存・上書き保存 |
| `LOAD200`～`LOAD299` | 読み取り・形式検査後にRAMのプログラムを置き換え |
| `LRUN200`～`LRUN299` | SDから読み込んで実行 |
| `FILES200,299` | SDの保存済み番号と先頭行を一覧表示 |

  - 使用例 BASIC
    ```basic
    CLS
    SDON
    NEW
    10 CLS
    20 PRINT "SD TEST OK"
    30 END
    SAVE202
    NEW
    LOAD202
    LIST
    RUN
    ```
  - 今回のSD実機確認は **Pico 2 / RP2350・HSTX DVI版** が対象です。RP2040でのSD対応は未検証です。SD処理は現在共通ソースに追加されており、機種別の完全分離は行っていません。
  - カード最上位の `P200.IJB`～`P299.IJB` に保存します。各ファイルは **4096バイトのバイナリ内部形式** で、テキストの `.BAS` とは異なります。Saved／Loaded表示はプログラム使用量です。SD読み込み失敗時は現在のプログラムを保持します。
  - SDON中のFILES／FILES0はSD一覧、SDOFF中は従来のFlash／EEPROM表示です。番号や範囲を明示すると指定先を表示し、FILES20は内蔵Flashの0～20、FILES90,220はSDON中にFlash・EEPROM・SDをまとめて表示します。SDOFF中にSD範囲を指定するとFile errorです。SD未保存番号と未割り当て番号は省略します。

- 拡張版識別のため `VER()` を変更
  - Raspberry Pi Pico / RP2040版: `16116`
  - Raspberry Pi Pico 2 / RP2350版: `16130`
  - 使用例 BASIC
    - Raspberry Pi Pico / RP2040版:
    ```sh
    ? VER()
    16116
    ? VER(1)
    8
    ```
    - Raspberry Pi Pico 2 / RP2350版:
    ```sh
    ? VER()
    16130
    ? VER(1)
    9
    ```
  - `VER(1)` によるプラットフォーム識別
    - Raspberry Pi Pico / RP2040版: `8`
    - Raspberry Pi Pico 2 / RP2350版: `9`
---
#  IchigoJam P BASICのソースコードを変更して４K版にしたものです。よって、まずはIchigoJam P BASICがコンパイルできる環境を作成します。(RP2040版のビルド説明)
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
  - `all_includes.h`（SD統合）
  - `sd_card.h`（日本語コメント付きSDドライバー）
- ルートの `CMakeLists.txt`：SD用 `hardware_spi` のリンク設定
- IchigoJam P の時の様にビルドして、`IchigoJam_P.uf2`ファイルが作成できたらPicoに書き込みます。
---
## RP2350版のビルド（Raspberry Pi Pico 2／macOS）

RP2350版はHSTX DVIを使用するため、PicoDVIライブラリは使用しません。
上記のRP2040用ライブラリ設定とは別に、RP2350用の
`pico-sdk-rp2350`、`pico-extras-rp2350` とArm GNU Toolchainを用意します。

リポジトリ最上位のフォルダーから、次を実行します。

```sh
cd IchigoJam_P
./build_pico2_mac.sh
```

ビルドに成功すると、`IchigoJam_P/build-pico2/IchigoJam_P.uf2`
が生成されます。このファイルをPico 2へ書き込みます。

配布用には、このUF2を `GivetakeJam_P.uf2` という名前で保存しています。
## Raspberry Pi Pico / RP2040 Firmware / Checksum
### Raspberry Pi Pico / RP2040用の4K版ファームウェアファイルです。
- `IchigoJam_P.uf2`
  - SHA-256: `9E9B7D4D1E7E776BC3B4F8DE3DE078C1FAEA978F5BBCE3385320402F1F92B396`
  - MD5: `1865CF3376720FB8050E2158087A2D16`
  - SHA-1: `72C3A41A9D976858646DFE3103C6973098AAC5C8`
## Raspberry Pi Pico 2 / RP2350 Firmware / Checksum
### Raspberry Pi Pico 2/ RP2350用の4K版ファームウェアファイルです。
- `GivetakeJam_P.uf2`
  - SHA-256: `F64CEEB28BCBE274E6EA3DA329590D4A67A3FF924AABC2E8CBCAFD36B56E51E2`
  - MD5: `C890CF1365426FBE0A5D994A445ED5A6`
  - SHA-1: `D917DE20A65255F7B7851C86528B24B83C94E942`

## Test Programs
- `ARRAY_VAR_TOTAL_TEST.BAS`
  - 配列変数のテストプログラムです。このテストが ALL OK で通ることを確認しています。
- `IR_IN_TEST.BAS`
  - NEC方式の赤外線受信のテストプログラムです。
- `ENV_IN_TEST.BAS`
  - AHT20 + BMP280 環境測定のテストプログラムです。
- `ICHIGO_EATER_COLOR.BAS`
  - COLOR コマンドを使った BASIC ゲームです。
- `PAL_RGB332_CHANGE.BAS`
  - RP2350 HSTX DVI 用の PAL / RGB332 パレット変更サンプルです。
  - カーソルキーで色番号を選択し、RGB332 パレット値を変更できます。
  - `R` キーで `PAL RESET` を実行し、ファームウェア内蔵のデフォルトパレットに戻します。
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
### PAL RGB332 変更サンプル
![screenshot](./docs/IMG_4984up.png)
`PAL_RGB332_CHANGE.BAS` は、RP2350 HSTX DVI 版の RGB332 パレットを確認・変更するためのサンプルプログラムです。
選択中の色番号、RGB332値、R/G/B成分、前景色サンプル、背景色サンプルを表示します。
カーソルキーで色番号の選択とパレット値の変更を行います。
`R` キーで `PAL RESET` を実行し、ファームウェア内蔵のデフォルトパレットに戻します。
### SDカード拡張基板回路例
![SD card expander PCB](./docs/MSD_MemCard101.png)
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
- SDカードへのアクセス機能
  - 保存番号は200～299の100本です。これはファームウェアで設定した範囲で、SDカードの容量やRAM容量による制限ではありません。保存本数を増やすには、ファームウェアの変更が必要です。
  - 試作基板の結果に基づき、初期化・通信とも **100kHz** に固定しています。ポーリング方式でDMAを使わず、SD処理全体で割り込みを禁止しません。SDON中はGP6とSPIのGP8・GP10・GP11を予約し、競合するOUT／PWM／IR.IN／CLO操作はFile errorです。SDON中はIN3をCS制御用の出力端子として使用します。IN(3)はCSのHIGH／LOWを返すため、外部信号の入力には使えません。
  - FAT32形式で、1セクターが512バイトのSDカードに対応します。MBR方式で区切った基本パーティション内のFAT32領域、またはパーティションを区切らずカード全体をFAT32にした構成を読み書きできます。プログラムはフォルダーの中ではなく、カードを開いた最上位の場所に保存します。ファイル名は、名前が最大8文字・拡張子が最大3文字の形式で、`P200.IJB`～`P299.IJB`を使用します。FAT12/FAT16・exFAT・GPT・拡張パーティション・サブフォルダー・ルート自動拡張は未対応です。ルート探索は256セクター／32クラスタ、既存の上書き対象は4096バイト／8クラスタまでです。
  - 読み取りCRCを検査します。保存は新規クラスタへ書いて各セクターを読み戻し照合した後、参照先を更新し旧領域を解放します。ただし、保存の途中で電源が切れたりカードが抜けたりすると、プログラムファイルやカード上のファイル管理情報が破損する可能性があります。保存中は電源を切ったりカードを抜いたりせず、重要なデータはバックアップしてください。RTC未使用のためファイル日付は1980-01-01です。
---
ギブテクウインウイン
## 変更履歴
- 2026/05/29 作成
- 2026/07/18 Pico 2 / RP2350 HSTX DVI版を追加
- 2026/07/29 Pico 2 配列変数を 1125 まで増加
- 2026/08/10 `WS.LED` の拡張配列変数へのアクセスと上限チェックを修正
- 2026/08/14 Pico 2 / RP2350 HSTX DVI テキスト出力向けに MSX風 `COLOR` 命令を追加
- 2026/08/22 Pico 2 / RP2350 HSTX DVI テキスト出力向けにパレット変更・色属性命令を追加
- 2026/08/23 Pico 2 / RP2350 HSTX DVI で `POKE` によるVRAM・PCG直接書き換え後の画面更新に対応
- 2026/08/24 Pico 2 / RP2350 HSTX DVI で LIST 後の折り返し複数行編集時の画面更新を修正
- 2026/08/28 Pico / RP2040版および Pico 2 / RP2350版で、`STOP` コマンド実行時に `Syntax error in 行番号` ではなく `Break in 行番号` と表示するよう修正
- 2026/10/11 SDカードによるプログラム保存ができるように `SDON` と `SDOFF` を追加
- 2026/10/11 16130版：CSをIN3／GP6の直結方式に変更。10kΩプルアップを残してMOSFETを省略。