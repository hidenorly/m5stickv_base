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
#include <sysctl.h>
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"

void hello_core(void)
{
    uint64_t core = current_coreid();
    printf("Core %ld Hello world\n", core);
}

int core_task_scheduler_enable(void *ctx)
{
    hello_core();
    vTaskStartScheduler();
    while(1);
}

void task_func1(void* arg)
{
    while(1)
    {
        printf("task1\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_func2(void* arg)
{
    while(1)
    {
        printf("task2\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    sysctl_pll_set_freq(SYSCTL_PLL0, 800000000);

    // UART1 with baud rate 115200 , 8 data bits , 1 stop bit , no parity
    uart_init(UART_DEVICE_1);
    uart_config(UART_DEVICE_1, 115200, UART_BITWIDTH_8BIT, UART_STOP_1, UART_PARITY_NONE);

    // enable 2nd core (=core1)
    register_core1(core_task_scheduler_enable, NULL);

    xTaskCreateAtProcessor(0, task_func1, "1", 256, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreateAtProcessor(1, task_func2, "2", 256, NULL, tskIDLE_PRIORITY+1, NULL );

    core_task_scheduler_enable(NULL); // infinite loop

    return 0;
}

