/* 
 Copyright (C) 2019, 2020 hidenorly

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include <bsp.h>
#include "board_setup.h"
#include "boards.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    #include "lcd.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */
#include "sysctl.h"
#include "plic.h"
#include "timer.h"
#include "fpioa.h"
#include "gpiohs.h"
#include "uart.h"
#include "uarths.h"
#include "rtc.h"
#include "w25qxx.h"
#include "pal_gpio.h"
#include "pal_led.h"


void setup_lcd(uint16_t fillColor, lcd_dir_t lcdDirection)
{
    fpioa_set_function(21, FUNC_GPIOHS0 + RST_GPIONUM); // RST
    fpioa_set_function(20, FUNC_GPIOHS0 + DCX_GPIONUM); // RS
    fpioa_set_function(22, FUNC_SPI0_SS0+LCD_SPI_SLAVE_SELECT); // CS
    fpioa_set_function(19, FUNC_SPI0_SCLK); //SCL
    fpioa_set_function(18, FUNC_SPI0_D0); // SDA

    lcd_init(CONFIG_LCD_DEFAULT_FREQ, false, 52, 40, 40, 52, true, CONFIG_LCD_DEFAULT_WIDTH, CONFIG_LCD_DEFAULT_HEIGHT);
    lcd_set_direction(lcdDirection);
    lcd_clear(fillColor);
}


void setup_buttons(void)
{
    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
}

bool flash_init(uint8_t* manuf_id, uint8_t* device_id)
{
    w25qxx_init_dma(3, 0);
    w25qxx_enable_quad_mode_dma();
    w25qxx_read_id_dma(manuf_id, device_id);
    return true;
}


void board_setup(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, FREQ_PLL0_DEFAULT);
    sysctl_pll_set_freq(SYSCTL_PLL1, FREQ_PLL1_DEFAULT);
    sysctl_pll_set_freq(SYSCTL_PLL2, FREQ_PLL2_DEFAULT);
    sysctl_cpu_set_freq(FREQ_CPU_MAX);//FREQ_CPU_DEFAULT);
    sysctl_clock_set_threshold(SYSCTL_THRESHOLD_AI, 1);
    sysctl_clock_enable(SYSCTL_CLOCK_AI);
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);

    fpioa_set_function(4, FUNC_UARTHS_RX);
    fpioa_set_function(5, FUNC_UARTHS_TX);
    uarths_init();
    uarths_config(115200, 1);

    dmac_init();
    plic_init();
    sysctl_enable_irq();
    rtc_init();
    rtc_timer_set(2020,1, 1,0, 0, 0);
    uint8_t manuf_id, device_id;
    flash_init(&manuf_id, &device_id);

    boards_init();

    setup_buttons();
    setup_lcd(BLACK, DIR_YX_LRUD);
}
