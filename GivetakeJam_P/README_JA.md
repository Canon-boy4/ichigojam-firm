# GivetakeJam BASIC
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-blue)
![Language](https://img.shields.io/badge/language-BASIC-orange)
![License](https://img.shields.io/badge/license-IchigoJam-green)
![Version](https://img.shields.io/badge/version-v1.6.1-brightgreen)

##  プログラム容量の拡張、配列変数の拡張、外部EEPROM対応の改善を行い、従来のIchigoJam P BASICの互換性を維持しながら機能強化しています。

## 機能 / 変更点
- プログラム容量を 1024 → 4096バイト に拡張
- 内部保存プログラム本数を 100 → 25 に変更
- 配列変数を拡張：
  - [0] ～ [357]（#C00 に VAR2 追加）
- FILESコマンド改善：
  - FILES → 0～24
  - FILES0 → 外部EEPROMがあれば 100～131
  - FILES n → 0～n（25～99はスキップ）
- 外部EEPROM検出を I2Cアドレス 0x50 のACKで実装
- 24LC64 / 24LC256 / 24FC1025 に単一バイナリで対応
- EEPROM書き込みを32バイト分割にして安全性向上
- 配列関連の不具合修正：
  - FOR/NEXT
  - CLV / CLEAR
- 間接参照 [[x]] に対応
- メモリマップ更新：
  - #000 CHAR
  - #700 PCG
  - #800 VAR
  - #900 VRAM
  - #C00 VAR2
  - #E00 LIST（4096バイト）
##
#  IchigoJam P BASICのソースコードを変更して４K版にしたものです。よって、まずはIchigoJam P BASICがコンパイルできる環境を作成します。

## 環境構築
- "CMake"と"GCC"をインストールする  

## IchigoJam_P のコンパイル環境 
　　-  IchigoJam_P のディレクトリーで作成します。

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
    - "IchigoJam_BASIC": basic.h , ram.h
    - "src": config.h , i2ceeprom.h , storage.h
    - IchigoJam P の時の様にビルドして、`IchigoJam_P.uf2`ファイルが作成できたらpicoに書き込みます。
    - IchigoJam_P.uf2: 4K版のファームウェアファイルです。
    - チェックサム
      SHA-256: 147a8c38c820bc1399f5d3fc848c316a912785f8a60ee5850782d401200a3496
      MD5: 24850280e6a6ea221cf4c470f14a4cde
      SHA-1: d206d96de20d9b40307e0606dd1c22b978ba9c06
    
    - ARRAY_VAR_TOTAL_TEST.BAS: 配列変数のテストプログラムです。このテストが ALL OK で通ることを確認しています。

## Screenshot

### 動作環境
![screenshot](./docs/IMG_2095up.png)

### 表示画面 ( 名称, HELP表示：メモリマップ, FILES表示 )
![screenshot](./docs/IMG_2159up.png)

### IchigoJam_P 互換基板 ( MECHTARIAT PJ 凸 )
![screenshot](./docs/IMG_2168up.png)

## ライセンス / license
    - Copyright 2014-2025 the IchigoJam authors. All rights reserved. MIT license.  https://ichigojam.net/license

## 商標 / Trademark
    - IchigoJamは、株式会社jig.jpの登録商標です。(IchigoJam is a registered trademark of jig.jp co., ltd.) 
    - Raspberry Pi は、Raspberry Pi財団の登録商標です。(Raspberry Pi is a registered trademark of the Raspberry Pi Foundation)  

## 利用規約に同意の上、ダウンロードしてください。第三者への配布にはライセンスが必要になります。詳しくはこちら。https://pcn.club/sp/ijp/
    - 利用規約（IchigoJamロイヤリティフリープログラム利用規約） https://ichigojam.net/ichigojam-license.pdf

## 既知の制限 / 注意事項
- プログラム最大4096バイト
- 外部EEPROM表示は100～131固定
- 実際の使用可能プログラム本数はEEPROM容量依存
- EEPROM検出はI2C 0x50で実施


ギブテクウインウイン
2026/3/27 作成  
![Release](https://img.shields.io/github/v/release/yourrepo)
![Downloads](https://img.shields.io/github/downloads/yourrepo/total)
