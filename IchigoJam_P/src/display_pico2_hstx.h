#ifndef __DISPLAY_PICO2_HSTX_H__
#define __DISPLAY_PICO2_HSTX_H__

#ifdef PICO_RP2350

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/resets.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/hstx_ctrl.h"
#include "hardware/structs/hstx_fifo.h"
#include "pico/multicore.h"
#include "pico/flash.h"

// -----------------------------------------------------------------------------
// 640x480@60Hz DVI timing

#define HSTX_MODE_H_FRONT_PORCH 16
#define HSTX_MODE_H_SYNC_WIDTH 96
#define HSTX_MODE_H_BACK_PORCH 48
#define HSTX_MODE_H_ACTIVE_PIXELS 640

#define HSTX_MODE_V_FRONT_PORCH 10
#define HSTX_MODE_V_SYNC_WIDTH 2
#define HSTX_MODE_V_BACK_PORCH 33
#define HSTX_MODE_V_ACTIVE_LINES 480

#define HSTX_MODE_H_TOTAL_PIXELS                        \
    (HSTX_MODE_H_FRONT_PORCH + HSTX_MODE_H_SYNC_WIDTH + \
     HSTX_MODE_H_BACK_PORCH + HSTX_MODE_H_ACTIVE_PIXELS)

#define HSTX_MODE_V_TOTAL_LINES                         \
    (HSTX_MODE_V_FRONT_PORCH + HSTX_MODE_V_SYNC_WIDTH + \
     HSTX_MODE_V_BACK_PORCH + HSTX_MODE_V_ACTIVE_LINES)

// -----------------------------------------------------------------------------
// TMDS control symbols

#define HSTX_TMDS_CTRL_00 0x354u
#define HSTX_TMDS_CTRL_01 0x0abu
#define HSTX_TMDS_CTRL_10 0x154u
#define HSTX_TMDS_CTRL_11 0x2abu

#define HSTX_SYNC_V0_H0 \
    (HSTX_TMDS_CTRL_00 | (HSTX_TMDS_CTRL_00 << 10) | (HSTX_TMDS_CTRL_00 << 20))

#define HSTX_SYNC_V0_H1 \
    (HSTX_TMDS_CTRL_01 | (HSTX_TMDS_CTRL_00 << 10) | (HSTX_TMDS_CTRL_00 << 20))

#define HSTX_SYNC_V1_H0 \
    (HSTX_TMDS_CTRL_10 | (HSTX_TMDS_CTRL_00 << 10) | (HSTX_TMDS_CTRL_00 << 20))

#define HSTX_SYNC_V1_H1 \
    (HSTX_TMDS_CTRL_11 | (HSTX_TMDS_CTRL_00 << 10) | (HSTX_TMDS_CTRL_00 << 20))

// -----------------------------------------------------------------------------
// HSTX commands

#define HSTX_CMD_RAW (0x0u << 12)
#define HSTX_CMD_RAW_REPEAT (0x1u << 12)
#define HSTX_CMD_TMDS (0x2u << 12)
#define HSTX_CMD_TMDS_REPEAT (0x3u << 12)
#define HSTX_CMD_NOP (0xfu << 12)

// -----------------------------------------------------------------------------
// DMA channels
//
// HSTX表示は公式サンプルと同じDMAチャネル0/1を使用する。
// RP2350版IR.INのPIO DMAはチャネル10を使用し、HSTXとは競合させない。
#define HSTX_DMACH_PING 0
#define HSTX_DMACH_PONG 1

// -----------------------------------------------------------------------------
// RGB332 framebuffer
//
// 640 x 480 x 1 byte = 307,200 bytes

static uint8_t hstx_framebuf[HSTX_MODE_H_ACTIVE_PIXELS * HSTX_MODE_V_ACTIVE_LINES];

static uint32_t hstx_vblank_line_vsync_off[] = {
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_FRONT_PORCH,
    HSTX_SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_SYNC_WIDTH,
    HSTX_SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT |
        (HSTX_MODE_H_BACK_PORCH + HSTX_MODE_H_ACTIVE_PIXELS),
    HSTX_SYNC_V1_H1,
    HSTX_CMD_NOP};

static uint32_t hstx_vblank_line_vsync_on[] = {
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_FRONT_PORCH,
    HSTX_SYNC_V0_H1,
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_SYNC_WIDTH,
    HSTX_SYNC_V0_H0,
    HSTX_CMD_RAW_REPEAT |
        (HSTX_MODE_H_BACK_PORCH + HSTX_MODE_H_ACTIVE_PIXELS),
    HSTX_SYNC_V0_H1,
    HSTX_CMD_NOP};

static uint32_t hstx_vactive_line[] = {
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_FRONT_PORCH,
    HSTX_SYNC_V1_H1,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_SYNC_WIDTH,
    HSTX_SYNC_V1_H0,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | HSTX_MODE_H_BACK_PORCH,
    HSTX_SYNC_V1_H1,
    HSTX_CMD_TMDS | HSTX_MODE_H_ACTIVE_PIXELS};

static volatile bool hstx_dma_pong = false;
static volatile uint hstx_v_scanline = 2;
static volatile bool hstx_vactive_cmdlist_posted = false;
static volatile uint32_t hstx_frame_count = 0;
static volatile uint32_t hstx_irq_count = 0;
static volatile bool hstx_started = false;
static volatile bool hstx_core1_ready = false;

// -----------------------------------------------------------------------------
// RGB332 helper

static inline uint8_t hstx_colour_rgb332(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xc0u) >> 6) |
           ((g & 0xe0u) >> 3) |
           ((b & 0xe0u) >> 0);
}

// -----------------------------------------------------------------------------
// DMA interrupt

void __scratch_x("") hstx_dma_irq_handler(void)
{
    uint ch_num = hstx_dma_pong ? HSTX_DMACH_PONG : HSTX_DMACH_PING;
    dma_channel_hw_t *ch = &dma_hw->ch[ch_num];

    dma_channel_acknowledge_irq1(ch_num);
    hstx_dma_pong = !hstx_dma_pong;

    if (hstx_v_scanline >= HSTX_MODE_V_FRONT_PORCH &&
        hstx_v_scanline <
            HSTX_MODE_V_FRONT_PORCH + HSTX_MODE_V_SYNC_WIDTH)
    {

        ch->read_addr = (uintptr_t)hstx_vblank_line_vsync_on;
        ch->transfer_count = count_of(hstx_vblank_line_vsync_on);
    }
    else if (hstx_v_scanline <
             HSTX_MODE_V_FRONT_PORCH +
                 HSTX_MODE_V_SYNC_WIDTH +
                 HSTX_MODE_V_BACK_PORCH)
    {

        ch->read_addr = (uintptr_t)hstx_vblank_line_vsync_off;
        ch->transfer_count = count_of(hstx_vblank_line_vsync_off);
    }
    else if (!hstx_vactive_cmdlist_posted)
    {

        ch->read_addr = (uintptr_t)hstx_vactive_line;
        ch->transfer_count = count_of(hstx_vactive_line);
        hstx_vactive_cmdlist_posted = true;
    }
    else
    {
        uint active_line =
            hstx_v_scanline -
            (HSTX_MODE_V_TOTAL_LINES - HSTX_MODE_V_ACTIVE_LINES);

        ch->read_addr =
            (uintptr_t)&hstx_framebuf[active_line * HSTX_MODE_H_ACTIVE_PIXELS];

        ch->transfer_count =
            HSTX_MODE_H_ACTIVE_PIXELS / sizeof(uint32_t);

        hstx_vactive_cmdlist_posted = false;
    }

    if (!hstx_vactive_cmdlist_posted)
    {
        hstx_v_scanline =
            (hstx_v_scanline + 1) % HSTX_MODE_V_TOTAL_LINES;

        if (hstx_v_scanline == 0)
        {
            hstx_frame_count++;
        }
    }
}

// -----------------------------------------------------------------------------
// HSTX initialization

void hstx_video_init(void)
{
    if (hstx_started)
    {
        return;
    }

    // HSTX TMDS encoder: RGB332
    hstx_ctrl_hw->expand_tmds =
        2 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
        0 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
        2 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
        29 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
        1 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
        26 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;

    hstx_ctrl_hw->expand_shift =
        4 << HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB |
        8 << HSTX_CTRL_EXPAND_SHIFT_ENC_SHIFT_LSB |
        1 << HSTX_CTRL_EXPAND_SHIFT_RAW_N_SHIFTS_LSB |
        0 << HSTX_CTRL_EXPAND_SHIFT_RAW_SHIFT_LSB;

    hstx_ctrl_hw->csr = 0;

    hstx_ctrl_hw->csr =
        HSTX_CTRL_CSR_EXPAND_EN_BITS |
        5u << HSTX_CTRL_CSR_CLKDIV_LSB |
        5u << HSTX_CTRL_CSR_N_SHIFTS_LSB |
        2u << HSTX_CTRL_CSR_SHIFT_LSB |
        HSTX_CTRL_CSR_EN_BITS;

    // Pico DVI Sock compatible arrangement:
    //
    // GP12 D0+   GP13 D0-
    // GP14 CLK+  GP15 CLK-
    // GP16 D2+   GP17 D2-
    // GP18 D1+   GP19 D1-

    hstx_ctrl_hw->bit[2] = HSTX_CTRL_BIT0_CLK_BITS;
    hstx_ctrl_hw->bit[3] =
        HSTX_CTRL_BIT0_CLK_BITS | HSTX_CTRL_BIT0_INV_BITS;

    for (uint lane = 0; lane < 3; ++lane)
    {
        static const int lane_to_output_bit[3] = {0, 6, 4};

        int bit = lane_to_output_bit[lane];

        uint32_t lane_data_sel_bits =
            (lane * 10) << HSTX_CTRL_BIT0_SEL_P_LSB |
            (lane * 10 + 1) << HSTX_CTRL_BIT0_SEL_N_LSB;

        hstx_ctrl_hw->bit[bit] = lane_data_sel_bits;
        hstx_ctrl_hw->bit[bit + 1] =
            lane_data_sel_bits | HSTX_CTRL_BIT0_INV_BITS;
    }

    for (int i = 12; i <= 19; ++i)
    {
        gpio_set_function(i, GPIO_FUNC_HSTX);
    }

    dma_channel_config c;

    c = dma_channel_get_default_config(HSTX_DMACH_PING);
    channel_config_set_chain_to(&c, HSTX_DMACH_PONG);
    channel_config_set_dreq(&c, DREQ_HSTX);

    dma_channel_configure(
        HSTX_DMACH_PING,
        &c,
        &hstx_fifo_hw->fifo,
        hstx_vblank_line_vsync_off,
        count_of(hstx_vblank_line_vsync_off),
        false);

    c = dma_channel_get_default_config(HSTX_DMACH_PONG);
    channel_config_set_chain_to(&c, HSTX_DMACH_PING);
    channel_config_set_dreq(&c, DREQ_HSTX);

    dma_channel_configure(
        HSTX_DMACH_PONG,
        &c,
        &hstx_fifo_hw->fifo,
        hstx_vblank_line_vsync_off,
        count_of(hstx_vblank_line_vsync_off),
        false);

    dma_channel_acknowledge_irq1(HSTX_DMACH_PING);
    dma_channel_acknowledge_irq1(HSTX_DMACH_PONG);

    dma_channel_set_irq1_enabled(HSTX_DMACH_PING, true);
    dma_channel_set_irq1_enabled(HSTX_DMACH_PONG, true);

    irq_set_exclusive_handler(DMA_IRQ_1, hstx_dma_irq_handler);

    // HSTX走査線DMAをUSBより高優先度で処理する
    irq_set_priority(DMA_IRQ_1, 0x00);
    irq_set_enabled(DMA_IRQ_1, true);

    bus_ctrl_hw->priority =
        BUSCTRL_BUS_PRIORITY_DMA_W_BITS |
        BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    hstx_v_scanline = 2;
    hstx_vactive_cmdlist_posted = false;
    hstx_dma_pong = false;

    dma_channel_start(HSTX_DMACH_PING);

    hstx_started = true;
}

void hstx_video_stop(void)
{
    if (!hstx_started)
    {
        return;
    }

    // HSTXの出力を先に止める
    hstx_ctrl_hw->csr = 0;

    // ping / pong DMAを停止
    dma_channel_abort(HSTX_DMACH_PING);
    dma_channel_abort(HSTX_DMACH_PONG);

    // DMA IRQ 1の通知を止める
    dma_channel_set_irq1_enabled(HSTX_DMACH_PING, false);
    dma_channel_set_irq1_enabled(HSTX_DMACH_PONG, false);

    // 保留中の完了フラグを消す
    dma_channel_acknowledge_irq1(HSTX_DMACH_PING);
    dma_channel_acknowledge_irq1(HSTX_DMACH_PONG);

    // CPU側のDMA IRQ 1を止める
    irq_set_enabled(DMA_IRQ_1, false);

    // 次回開始時に必ず初期化し直す
    hstx_dma_pong = false;
    hstx_v_scanline = 2;
    hstx_vactive_cmdlist_posted = false;
    hstx_started = false;
}

int hstx_video_active(void)
{
    return hstx_started;
}

void hstx_video_wait_sync(uint num)
{
    uint32_t start = hstx_frame_count;

    while ((uint32_t)(hstx_frame_count - start) < num)
    {
        tight_loop_contents();
    }
}

void hstx_video_fill(uint8_t colour)
{
    memset(
        hstx_framebuf,
        colour,
        sizeof(hstx_framebuf));
}

void hstx_core1_main(void);

void hstx_video_test_pattern(void)
{
    for (uint y = 0; y < HSTX_MODE_V_ACTIVE_LINES; ++y)
    {
        for (uint x = 0; x < HSTX_MODE_H_ACTIVE_PIXELS; ++x)
        {
            uint8_t colour;

            if (x < 213)
            {
                colour = hstx_colour_rgb332(255, 0, 0);
            }
            else if (x < 426)
            {
                colour = hstx_colour_rgb332(0, 255, 0);
            }
            else
            {
                colour = hstx_colour_rgb332(0, 0, 255);
            }

            hstx_framebuf[y * HSTX_MODE_H_ACTIVE_PIXELS + x] = colour;
        }
    }
}

// SAVE時に flash_safe_execute() により core 1 が一時停止すると、
// HSTX DMA IRQ が止まり、DVI同期が失われる。
// SAVE完了後、core 1 と HSTX/DMA を初回起動相当の状態へ戻す。
//
// 戻り値:
//   0: core 1再起動、HSTX初期化、フレーム進行まで成功
//   1: core 1再起動または hstx_core1_main() 到達に失敗
//   2: core 1は起動したが、HSTX DMA IRQが進まず frame_count が増えない
int hstx_core1_reboot(void)
{
    // core 1を止める。HSTX DMA IRQもcore 1側で動いているため、
    // ここでcore 1側のIRQ処理を完全に止める。
    multicore_reset_core1();

    // HSTX出力停止
    hstx_ctrl_hw->csr = 0;

    // HSTX用DMAを停止
    dma_channel_abort(HSTX_DMACH_PING);
    dma_channel_abort(HSTX_DMACH_PONG);

    // DMA IRQ通知を止め、保留フラグを消す
    dma_channel_set_irq1_enabled(HSTX_DMACH_PING, false);
    dma_channel_set_irq1_enabled(HSTX_DMACH_PONG, false);
    dma_channel_acknowledge_irq1(HSTX_DMACH_PING);
    dma_channel_acknowledge_irq1(HSTX_DMACH_PONG);
    irq_clear(DMA_IRQ_1);

    // DMAとHSTXハードウェアブロックを初期状態へ戻す。
    // SAVE後にDMA完了IRQが再開しないため、DMAチャネル0/1だけでなく
    // DMAブロック全体をリセットする。
    reset_block(RESETS_RESET_DMA_BITS | RESETS_RESET_HSTX_BITS);
    unreset_block_wait(RESETS_RESET_DMA_BITS | RESETS_RESET_HSTX_BITS);

    // HSTX状態変数を初期化
    hstx_dma_pong = false;
    hstx_v_scanline = 2;
    hstx_vactive_cmdlist_posted = false;
    hstx_started = false;

    // core 1を再起動し、初回起動と同じ経路でHSTXを開始する
    hstx_core1_ready = false;
    hstx_frame_count = 0;

    multicore_launch_core1(hstx_core1_main);

    // core 1が hstx_core1_main() に入り、HSTX初期化を終えるまで待つ
    for (uint32_t i = 0; i < 1000000; i++)
    {
        if (hstx_core1_ready)
        {
            break;
        }
        tight_loop_contents();
    }

    if (!hstx_core1_ready)
    {
        return 1; // core 1再起動失敗
    }

    // HSTX DMA IRQが再開し、フレームが進むまで待つ
    for (uint32_t i = 0; i < 3000000; i++)
    {
        if (hstx_frame_count >= 2)
        {
            return 0; // core 1再起動 + HSTX走査再開 OK
        }
        tight_loop_contents();
    }

    return 2; // core 1は起動したが、HSTXフレームが進まない
}

// -----------------------------------------------------------------------------
// Core 1: HSTX DMA / DMA IRQ専用

void hstx_core1_main(void)
{
    flash_safe_execute_core_init();

    hstx_video_init();
    hstx_core1_ready = true;

    while (true)
    {
        __wfi();
    }
}

#endif // PICO_RP2350

#endif // __DISPLAY_PICO2_HSTX_H__
