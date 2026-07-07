#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "all_includes.h"

#define CHAR_MAX_COLS 32
#define CHAR_MAX_ROWS 24

#ifdef PICO_RP2350

// -----------------------------------------------------------------------------
// Raspberry Pi Pico 2 / RP2350 / HSTX DVI

void video_on() {
    SCREEN_W = CHAR_MAX_COLS >> _g.screen_big;
    SCREEN_H = CHAR_MAX_ROWS >> _g.screen_big;

    // HSTX開始は pico_init() 側で一度だけhstx_video_init()を行う。;
}

void video_off(int clkdiv) {
    (void)clkdiv;
    // まだ停止処理hstx_video_stop()を呼ばない。;
}

inline int video_active() {
    return hstx_video_active();
}

void video_waitSync(uint num) {
    // まだ hstx_video_wait_sync(num)を呼ばない。;
    (void)num;
}

#else

// -----------------------------------------------------------------------------
// Raspberry Pi Pico / RP2040 / PicoDVI

struct dvi_inst dvi0;

void video_on() {
    SCREEN_W = CHAR_MAX_COLS >> _g.screen_big;
    SCREEN_H = CHAR_MAX_ROWS >> _g.screen_big;

    dvi_start(&dvi0);
}

// クロックの変更機能はつけない
void video_off(int clkdiv) {
    (void)clkdiv;
    dvi_stop(&dvi0);
}

inline int video_active() {
    return dvi0.started;
}

void video_waitSync(uint num) {
    // 現状は何もしない
    (void)num;
}

#endif // PICO_RP2350

INLINE void IJB_lcd(uint mode) {
    // SWITCHで呼ばれる。LCD機能は未実装。
    (void)mode;
}

#endif // __DISPLAY_H__

