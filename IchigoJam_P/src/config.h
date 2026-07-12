// created by Taisuke Fukuno, @taisukef, http://fukuno.jig.jp/
// license CC BY-NC-SA 4.0 http://creativecommons.org/licenses/by-nc-sa/4.0/deed.ja
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "all_includes.h"

#define ICHIGOJAM

//#define IJB_BETA // beta or not

// #define PSG_TRUE_TONE_UNIT (109 * 1)
// #define PSG_TICK_PER_SEC 60

#define PLATFORM_RP2040 8               //### ここに書くべきか、それが問題だ
#define PLATFORM_RP2350 9

#ifdef PICO_RP2350
#define VER_PLATFORM PLATFORM_RP2350
#else
#define VER_PLATFORM PLATFORM_RP2040
#endif

#define IJB_VER_STR 1.6.1

#ifdef IJB_BETA
#define IJB_TITLE "IchigoJam BASIC 1.5b rv jig.jp\n"
#else
// ===== MODIFIED (2025-12-30) =====
// プログラム領域4096 Givetake版
// #define IJB_TITLE "IchigoJam BASIC " STRING2(IJB_VER_STR) " by jig.jp forked by na-s.jp\n"

#define IJB_TITLE "GivetakeJam BASIC " STRING2(IJB_VER_STR) " Pico-4K forked by Givetakewinwin\n"
#endif

#define ENABLE_I2C_BPS

// ===== MODIFIED (2025-12-30) =====
// 変更内容: 保存可能なプログラム本数を 100本 -> 25本 に変更
// 目的    : 1本あたりのプログラム容量を 4096バイトへ拡張するため、保存本数を削減
// ===== MODIFIED (2026-7-10) =====
// 変更内容: 保存可能なプログラム本数をPico_RP2350使用時に 25本 -> 100本 に変更
#ifdef PICO_RP2350
#define N_FLASH_STORAGE 100
#else
#define N_FLASH_STORAGE 25
#endif

// Pico / RP2040: internal flash slots 0-24
// Pico 2 / RP2350: internal flash slots 0-99

#define VERSION15

#define EXT_IOT
// #define KEY_INIT_DELAY
#endif
