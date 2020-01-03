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

#include "fpioa.h"

#define MPU6886_IMPL
#include "pal_mpu6886.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sipeed_i2c.h"
#include "type_defines.h"


int MPU6886_i2c_send_data(uint8_t cmd, size_t send_buf_len, const uint8_t *send_buf)
{
	uint8_t buf[send_buf_len+1];
	buf[0] = cmd;
	memcpy(buf+1, send_buf, send_buf_len);
	return maix_i2c_send_data(MPU6886_I2C_BUS, MPU6886_I2C_SLA, buf, send_buf_len+1, 10);
}

int MPU6886_i2c_send_byte(uint8_t cmd, const uint8_t sendData)
{
	uint8_t buf[2];
	buf[0] = cmd;
	buf[1] = sendData;
	return maix_i2c_send_data(MPU6886_I2C_BUS, MPU6886_I2C_SLA, buf, 2, 10);
}

int MPU6886_i2c_recv_data(uint8_t cmd, size_t receive_buf_len, uint8_t *receive_buf)
{
	return maix_i2c_recv_data(MPU6886_I2C_BUS, MPU6886_I2C_SLA, &cmd, 1, receive_buf, receive_buf_len, 10);
}

uint8_t MPU6886_i2c_recv_byte(uint8_t cmd)
{
	uint8_t buf;
	int ret = MPU6886_i2c_recv_data(cmd, 1, &buf);

	return ret ? 0 : buf;
}


int MPU6886_Init(void)
{
	// setup GPIOs for I2C & the I2C
	fpioa_set_function(MPU6886_I2C_PIN_SCL, FUNC_I2C0_SCLK);
	fpioa_set_function(MPU6886_I2C_PIN_SDA, FUNC_I2C0_SDA);
	maix_i2c_init(MPU6886_I2C_BUS, 7, 400000);

	if( MPU6886_i2c_recv_byte(MPU6886_WHOAMI) != MPU6886_WHOAMI_VALUE ){
		maix_i2c_deinit(MPU6886_I2C_BUS);
		printf("This device doesn't have MPU6886.\r\n");
		// WHO AM I's register (117) will return 0x19. Otherwise, it's not MPU6886.
		return -1;
	}

	delay(1);

	// Device reset to default
	MPU6886_i2c_send_byte(MPU6886_PWR_MGMT_1, MPU6886_PWR_MGMT_1_RESET);
	delay(10);

	// Device CLKSEL=0 (Internal 20MHz, Enable TEMP sensor, Gyro, Not Sleep)
	MPU6886_i2c_send_byte(MPU6886_PWR_MGMT_1, 0);
	delay(10);

	// Enable X/Y/Z accelerometer, X/Y/Z gyro
	MPU6886_i2c_send_byte(MPU6886_PWR_MGMT_2, 0);
	delay(10);

	// setup accelerometer sensor
	MPU6886_i2c_send_byte(MPU6886_ACCEL_CONFIG, MPU6886_ACCEL_CONFIG_8G);
	delay(1);
	// accelerometer : 3-DB BW 218.1Hz, Noise BW:235Hz, RATE:1KHz
	MPU6886_i2c_send_byte(MPU6886_ACCEL_CONFIG2, 0);
	delay(1);

	// setup gyro sensor
	MPU6886_i2c_send_byte(MPU6886_GYRO_CONFIG, MPU6886_GYRO_CONFIG_2000DPSG);
	delay(1);
	// gyro : Disable FSYNC, Loop FIFO, 3-DB BW 176Hz, Noise BW 177Hz, RATE:1KHz
	MPU6886_i2c_send_byte(MPU6886_CONFIG, 0x01);
	delay(1);

	// Set SAMPLE RATE DIVIDER
	MPU6886_i2c_send_byte(MPU6886_SMPLRT_DIV, 0x05);
	delay(1);

	// DISABLE ALL INTERRUPT from MPU6886
	MPU6886_i2c_send_byte(MPU6886_INT_ENABLE, 0);
	delay(1);

	// Disable FIFO_EN
	MPU6886_i2c_send_byte(MPU6886_USER_CTRL, 0);
	delay(1);

	// Disable each sensor's FIFO_EN
	MPU6886_i2c_send_byte(MPU6886_FIFO_EN, 0);
	delay(1);

	// LATCH_INT_EN=1
	MPU6886_i2c_send_byte(MPU6886_INT_PIN_CFG, MPU6886_INT_PIN_CFG_VAL);
	delay(1);

	// Enable DATA_RDY_INT_EN
	MPU6886_i2c_send_byte(MPU6886_INT_ENABLE, MPU6886_INT_ENABLE_VAL);
	delay(10);

	return 0;
}

void MPU6886_getGyroData(float* gyroX, float* gyroY, float* gyroZ)
{
	uint8_t buf[6];
	MPU6886_i2c_recv_data(MPU6886_ACCEL_XOUT_H, 6, buf);

	*gyroX=(float)(((int16_t)buf[0]<<8) | buf[1]) * g_gyroK;
	*gyroY=(float)(((int16_t)buf[2]<<8) | buf[3]) * g_gyroK;
	*gyroZ=(float)(((int16_t)buf[4]<<8) | buf[5]) * g_gyroK;
}

void MPU6886_getAccelData(float* accelX, float* accelY, float* accelZ)
{
	uint8_t buf[6];
	MPU6886_i2c_recv_data(MPU6886_GYRO_XOUT_H, 6, buf);

	*accelX=(float)(((int16_t)buf[0]<<8) | buf[1]) * g_accelK;
	*accelY=(float)(((int16_t)buf[2]<<8) | buf[3]) * g_accelK;
	*accelZ=(float)(((int16_t)buf[4]<<8) | buf[5]) * g_accelK;
}

void MPU6886_getAhrsData(float* pitch, float* roll, float* yaw)
{

}



#ifdef __cplusplus
}
#endif /* __cplusplus */
