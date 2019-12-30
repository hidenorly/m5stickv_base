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
#include "FreeRTOS.h"
#include "task.h"
#include "board_setup.h"
#include "pal_gpio.h"
#include "pal_led.h"
#include "type_defines.h"


static volatile uint8_t mDuties[4] = {0, 0, 0, 0};
static volatile uint8_t mLedPins[4] = {LED_R, LED_G, LED_B, LED_W};

#define DUTY_CYCLE 16

void task_led(void* arg)
{
    while(TRUE)
    {
    	int pin = ((int)arg) & 3; // mask 0b11 for <4
    	if(mDuties[pin]==0){
	    	digitalWrite(mLedPins[pin], HIGH);
	        vTaskDelay(pdMS_TO_TICKS(DUTY_CYCLE));
    	} else {
	    	digitalWrite(mLedPins[pin], LOW);
	    	int delay = ((float)DUTY_CYCLE * (float)mDuties[pin] / 255.0f);
	        vTaskDelay(pdMS_TO_TICKS((int)delay));
	    	delay = DUTY_CYCLE - delay;
	    	if(delay>0){
		    	digitalWrite(mLedPins[pin], HIGH);
		        vTaskDelay(pdMS_TO_TICKS(delay));
		    }
    	}
    }
}

void rgbw_led_setduty(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
	mDuties[0] = r;
	mDuties[1] = g;
	mDuties[2] = b;
	mDuties[3] = w;
}

void rgbw_led_init(void)
{
	for(int i=0; i<sizeof(mLedPins); i++){
	    digitalWrite(mLedPins[i], HIGH); pinMode(mLedPins[i], OUTPUT);
	}

    xTaskCreateAtProcessor(1, task_led, "Led_R", 256, (void*)0, tskIDLE_PRIORITY, NULL );
    xTaskCreateAtProcessor(1, task_led, "Led_G", 256, (void*)1, tskIDLE_PRIORITY, NULL );
    xTaskCreateAtProcessor(1, task_led, "Led_B", 256, (void*)2, tskIDLE_PRIORITY, NULL );
    xTaskCreateAtProcessor(1, task_led, "Led_W", 256, (void*)3, tskIDLE_PRIORITY, NULL );
}
