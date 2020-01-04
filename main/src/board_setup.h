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

#ifndef __BOARD_SETUP_H__
#define __BOARD_SETUP_H__

#define BUTTON_A    36
#define BUTTON_B    37
#define LED_W		7
#define LED_R		6
#define LED_G		9
#define LED_B		8

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void board_setup(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#define FREQ_PLL0_DEFAULT    832000000UL
#define FREQ_PLL1_DEFAULT    400000000UL
#define FREQ_PLL2_DEFAULT    45158400UL
#define FREQ_CPU_MAX     600000000UL
#define FREQ_CPU_DEFAULT (FREQ_PLL0_DEFAULT/2)


#endif // __BOARD_SETUP_H__
