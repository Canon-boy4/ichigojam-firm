#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "all_includes.h"

#define PICO_LED_PIN 25

bool is_deep_sleeping = false;

int getSleepFlag(void) {
    return IJB_btn(0);
}

// Pico 2 DVIなし第1段階では、SLEEP機能を停止する。
static void IJB_sleep(void) {
    // 何もしない
}

// basic.h 側の S_INLINE 宣言と一致させる。
S_INLINE void IJB_reset(void) {
    watchdog_reboot(0, SRAM_END, 0);

    for (;;) {
        __wfi();
    }
}

// WAIT n は60Hz単位として通常待機する。
// WAIT -n はDVI/VSYNCが無いため未対応。
static int IJB_wait(int n, int active) {
    if (n <= 0) {
        return 0;
    }

    uint64_t start = time_us_64();
    uint64_t wait_us = (uint64_t)n * 1000000u / 60u;

    while ((time_us_64() - start) < wait_us) {
        if (active && stopExecute()) {
            return 1;
        }
        tight_loop_contents();
    }

    return 0;
}

#endif // __SYSTEM_H__
