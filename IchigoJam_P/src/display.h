#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include "all_includes.h"

#define CHAR_MAX_COLS 32
#define CHAR_MAX_ROWS 24

#ifndef PICO_RP2350
struct dvi_inst dvi0;
#endif

void video_on() {
    SCREEN_W = CHAR_MAX_COLS >> _g.screen_big;
    SCREEN_H = CHAR_MAX_ROWS >> _g.screen_big;

#ifndef PICO_RP2350
    dvi_start(&dvi0);
#endif
}

#ifndef PICO_RP2350
//クロックの変更機能はつけない
void video_off(int clkdiv) {
    dvi_stop(&dvi0);
}

inline int video_active() {
    return dvi0.started;
}

#else

void video_off(int clkdiv) {
    (void)clkdiv;
}

inline int video_active() {
    return 0;
}
#endif

INLINE void IJB_lcd(uint mode) {
    //SWITCHで呼ばれる、何もしない
}

void video_waitSync(uint num) {
    //何もしない
}

#endif
