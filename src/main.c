// VGA for Amstrad CPC by gregg

#include <string.h>
#include "pico/stdlib.h"
#include "test_screen.h"
#include "mode_switch.h"
#include "vga_dma.h"
#include "cpc_dma.h"
#include "vga_pio.h"
#include "cpc_pio.h"
#include "constants.h"
#include "hardware/clocks.h"


// Just a copy of amstrad screen. Each byte corresponds to one pixel (mode 2). Only 6 bits are used - 2 for each rgb color.
// 00 means no color, 01 or 10 is half brightness, and 11 is full brightness.
uint8_t screen[CPC_SCREEN_WIDTH * CPC_SCREEN_HEIGHT];


int main() {
    // It is way more important to accurately time reading from CPC then writing to VGA.
    // Because of that, system clock should be divisible by CPC pixel clock.
    // CPC pixel clock in mode 2 is 16Mhz. When we set system clock to 128Mhz (instead of standard 125Mhz),
    // we can read one pixel in exactly 8 cycles.
	
	
    // old way (removed on RP2350):
    // set_sys_clock_khz(133000, true);

    // new way (works on both RP2040 and RP2350):
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    0,
                    128 * MHZ,
                    128 * MHZ);
    // plus any PLL setup you need, etc.

    setup_vfreq_and_switch();
    fill_test_screen(screen);
    setup_vga_pio(is_50hz());
    setup_cpc_pio();
    setup_vga_dma(screen, is_50hz());
    start_vga_pio();
    start_vga_dma();
    start_cpc_dma_loop(screen);

    return 0;
}
