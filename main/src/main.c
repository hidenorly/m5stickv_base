/* Copyright 2019 hidenorly
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <bsp.h>
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board_setup.h"
#include "lcd.h"
#include "pal_gpio.h"

void hello_core(void)
{
    uint64_t core = current_coreid();
    printf("Core %ld Hello world\n", core);
}

int core_task_scheduler_enable(void *ctx)
{
    hello_core();
    vTaskStartScheduler();
    while(1){
        asm ("wfi");
    }

    return 0;
}

void task_func1(void* arg)
{
    while(1)
    {
        static int lastBtnStatus = -1;
        int curStatus = digitalRead(BUTTON_A);
        if( lastBtnStatus != curStatus){
            lastBtnStatus = curStatus;
            printf("Button A: %s\r\n", curStatus ? "Released" : "Pushed");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void task_func2(void* arg)
{
    while(1)
    {
        static int lastBtnStatus = -1;
        int curStatus = digitalRead(BUTTON_B);
        if( lastBtnStatus != curStatus){
            lastBtnStatus = curStatus;
            printf("Button B: %s\r\n", curStatus ? "Released" : "Pushed");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


int main(void)
{
    board_setup();

    // LCD
    lcd_draw_string(0,0, "hello world", WHITE);

    // enable 2nd core (=core1)
    register_core1(core_task_scheduler_enable, NULL);

    xTaskCreateAtProcessor(0, task_func1, "1", 256, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreateAtProcessor(1, task_func2, "2", 256, NULL, tskIDLE_PRIORITY+1, NULL );

    core_task_scheduler_enable(NULL); // infinite loop

    return 0;
}

