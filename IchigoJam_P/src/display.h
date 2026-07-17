#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "all_includes.h"

#define CHAR_MAX_COLS 32
#define CHAR_MAX_ROWS 24

#ifdef PICO_RP2350

// -----------------------------------------------------------------------------
// Raspberry Pi Pico 2 / RP2350 / HSTX DVI

// VIDEO 0 はHSTX/DVI信号を止めるためではなく、
// VRAMからHSTXフレームバッファへの画面更新を止め、
// BASIC実行速度を上げるために使う。
// HSTX信号自体はcore1側で出し続ける。
static volatile uint8_t hstx_screen_active = 1;

void video_on()
{
    SCREEN_W = CHAR_MAX_COLS >> _g.screen_big;
    SCREEN_H = CHAR_MAX_ROWS >> _g.screen_big;

    // VIDEO 1/2/3/4 で画面更新を再開する。
    // HSTX開始は pico_init() 側で一度だけ行う。
    hstx_screen_active = 1;
}

void video_off(int clkdiv)
{
    (void)clkdiv;

    // VIDEO 0 ではHSTX/DVI信号は止めない。
    // 互換性のため、まず画面を黒で消してから、
    // timer() からの画面更新を止める。
    hstx_video_fill(0x00);
    hstx_screen_active = 0;
}

static inline int video_active()
{
    return hstx_screen_active;
}

void video_waitSync(uint num)
{
    // 現状は何もしない。
    (void)num;
}

#else

// -----------------------------------------------------------------------------
// Raspberry Pi Pico / RP2040 / PicoDVI

struct dvi_inst dvi0;

void video_on()
{
    SCREEN_W = CHAR_MAX_COLS >> _g.screen_big;
    SCREEN_H = CHAR_MAX_ROWS >> _g.screen_big;

    dvi_start(&dvi0);
}

// クロックの変更機能はつけない
void video_off(int clkdiv)
{
    (void)clkdiv;
    dvi_stop(&dvi0);
}

static inline int video_active()
{
    return dvi0.started;
}

void video_waitSync(uint num)
{
    // 現状は何もしない
    (void)num;
}

#endif // PICO_RP2350

INLINE void IJB_lcd(uint mode)
{
    // SWITCHで呼ばれる。LCD機能は未実装。
    (void)mode;
}

#endif // __DISPLAY_H__
