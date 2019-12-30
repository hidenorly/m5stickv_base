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

#include "pal_gpio.h"
#include "fpioa.h"
#include "gpiohs.h"
#include "gpio.h"

#define MAX_GPIOHS 32

void pinMode(uint8_t pin, uint8_t gpioMode)
{
    fpioa_set_function(pin, FUNC_GPIOHS0 + pin);
    if(pin<MAX_GPIOHS){
        switch(gpioMode){
            case OUTPUT:
                gpiohs_set_drive_mode(pin, GPIO_DM_OUTPUT);
                break;
            case INPUT:
                gpiohs_set_drive_mode(pin, GPIO_DM_INPUT);
                break;
            case INPUT_PULLUP:
                gpiohs_set_drive_mode(pin, GPIO_DM_INPUT_PULL_UP);
                break;
            case INPUT_PULLDOWN:
                gpiohs_set_drive_mode(pin, GPIO_DM_INPUT_PULL_DOWN);
                break;
            default:;
        }
    } else {
        pin -= MAX_GPIOHS;
        switch(gpioMode){
            case OUTPUT:
                gpio_set_drive_mode(pin, GPIO_DM_OUTPUT);
                break;
            case INPUT:
                gpio_set_drive_mode(pin, GPIO_DM_INPUT);
                break;
            case INPUT_PULLUP:
                gpio_set_drive_mode(pin, GPIO_DM_INPUT_PULL_UP);
                break;
            case INPUT_PULLDOWN:
                gpio_set_drive_mode(pin, GPIO_DM_INPUT_PULL_DOWN);
                break;
            default:;
        }

    }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    gpio_pin_value_t pinVal = (val == HIGH) ? GPIO_PV_HIGH : GPIO_PV_LOW;
    if(pin<MAX_GPIOHS){
        gpiohs_set_pin(pin, pinVal);
    } else {
        gpio_set_pin(pin - MAX_GPIOHS, pinVal);
    }
}

int digitalRead(uint8_t pin)
{
    int result = LOW;
    result = (pin<MAX_GPIOHS) ? gpiohs_get_pin(pin) : gpio_get_pin(pin-MAX_GPIOHS);
	return (result == GPIO_PV_HIGH) ? HIGH : LOW;
}

