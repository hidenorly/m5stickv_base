/* 
 Copyright (C) 2019 hidenorly

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
#include <sysctl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    #include "lcd.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */
#include "fpioa.h"
#include "gpiohs.h"
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

void board_setup(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000);
    boards_init();

    setup_buttons();
    setup_lcd(BLACK, DIR_YX_LRUD);
}
