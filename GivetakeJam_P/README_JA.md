# GivetakeJam P
# IchigoJam P のソースコードを変更して４K版にしたものです。よって、まずはIchigoJam Pがコンパイルできる環境を作成します。

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
    - "src": config.h
    - IchigoJam P の時の様にビルドして、`IchigoJam_P.uf2`ファイルが作成できたらpicoに書き込みます。
    - IchigoJam_P.uf2: 4K版のファームウェアファイルです。
    - チェックサム
      SHA-256: a044e888e0edd73ac1f650f108d5fdd786ef4a8424b7397830a7a624533260af
      MD5: 8196bbe7a1f56b6a123e00c665f0e81c
      SHA-1: 11d5b289829b82127225dc637d84c12d71d6dfef
    
    - ARRAY_VAR_TOTAL_TEST.BAS: 配列変数のテストプログラムです。このテストが ALL OK で通ることを確認しています。

## ライセンス / license
    - Copyright 2014-2025 the IchigoJam authors. All rights reserved. MIT license.  https://ichigojam.net/license

## 商標 / Trademark
    - IchigoJamは、株式会社jig.jpの登録商標です。(IchigoJam is a registered trademark of jig.jp co., ltd.) 
    - Raspberry Pi は、Raspberry Pi財団の登録商標です。(Raspberry Pi is a registered trademark of the Raspberry Pi Foundation)  

## 利用規約に同意の上、ダウンロードしてください。第三者への配布にはライセンスが必要になります。詳しくはこちら。https://pcn.club/sp/ijp/
    - 利用規約（IchigoJamロイヤリティフリープログラム利用規約） https://ichigojam.net/ichigojam-license.pdf
    

ギブテクウインウイン
2026/2/7 作成  
