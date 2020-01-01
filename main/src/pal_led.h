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

#ifndef __PAL_LED_H__
#define __PAL_LED_H__

#include <bsp.h>

#ifndef LED_W
	#define LED_W		7
#endif // LED_W
#ifndef LED_R
	#define LED_R		6
#endif // LED_R
#ifndef LED_G
	#define LED_G		9
#endif // LED_G
#ifndef LED_B
	#define LED_B		8
#endif // LED_B

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void rgbw_led_init(void);
void rgbw_led_setduty(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // __PAL_LED_H__
