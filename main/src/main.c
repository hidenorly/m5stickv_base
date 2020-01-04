/* Copyright 2019, 2020 hidenorly
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
#include "FreeRTOS.h"
#include "task.h"
#include "board_setup.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    #include "lcd.h"
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "pal_gpio.h"
#include "pal_led.h"
#include "IMU.h"

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
        static int lastBtnStatus[2] = {-1, -1};
        int curStatus[2];
        for(int i=0; i<2; i++){
            curStatus[i] = digitalRead((i==0) ? BUTTON_A : BUTTON_B);
            if( lastBtnStatus[i] != curStatus[i]){
                lastBtnStatus[i] = curStatus[i];
                if(curStatus[i] == LOW){
                    if( i == 0){
                        rgbw_led_setduty(128, 0, 0, 0);
                    } else {
                        rgbw_led_setduty(128, 128, 128, 0);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

#include "boards.h"


void task_func2(void* arg)
{
    float accX = 0.0f;
    float accY = 0.0f;
    float accZ = 0.0f;
     
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    float gyroZ = 0.0f;
     
    float pitch = 0.0f;
    float roll  = 0.0f;
    float yaw   = 0.0f;

    IMU_Init();

    while(1)
    {
        IMU_getAccelData(&accX, &accY, &accZ);
        IMU_getGyroData(&gyroX, &gyroY, &gyroZ);
        IMU_getAhrsData(&pitch, &roll, &yaw);

        lcd_clear(BLACK);
        char buf[64];
        snprintf(buf, sizeof(buf), "a:%6.2f,%6.2f,%6.2f", accX, accY, accZ);
        lcd_draw_string(0,0, buf, WHITE); printf("%s\r\n",buf);
        snprintf(buf, sizeof(buf), "g:%6.2f,%6.2f,%6.2f", gyroX, gyroY, gyroZ);
        lcd_draw_string(0,20, buf, WHITE); printf("%s\r\n",buf);
        snprintf(buf, sizeof(buf), "h:%6.2f,%6.2f,%6.2f", pitch, roll, yaw);
        lcd_draw_string(0,40, buf, WHITE); printf("%s\r\n",buf);

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

    // LED
    rgbw_led_init();

    xTaskCreateAtProcessor(0, task_func1, "1", 512, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreateAtProcessor(0, task_func2, "2", 512, NULL, tskIDLE_PRIORITY+1, NULL );

    core_task_scheduler_enable(NULL); // infinite loop

    return 0;
}

