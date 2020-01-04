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

#include "fpioa.h"

#define SH200Q_IMPL
#include "pal_sh200q.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "i2c.h"
#include "type_defines.h"

#define ENABLE_MPU6886_I2C_DEBUG 0


int SH200Q_i2c_send_data(uint8_t cmd, size_t send_buf_len, const uint8_t *send_buf)
{
	uint8_t buf[send_buf_len+1];
	buf[0] = cmd;
	memcpy(buf+1, send_buf, send_buf_len);
	return i2c_send_data(SH200Q_I2C_BUS, buf, send_buf_len+1);
}

int SH200Q_i2c_send_byte(uint8_t cmd, const uint8_t sendData)
{
	uint8_t buf[2];
	buf[0] = cmd;
	buf[1] = sendData;
	return i2c_send_data(SH200Q_I2C_BUS, buf, 2);
}

int SH200Q_i2c_recv_data(uint8_t cmd, size_t receive_buf_len, uint8_t *receive_buf)
{
	int ret = 0;
	uint8_t buf[receive_buf_len+1];
	memset(buf, 0, receive_buf_len+1);
	buf[0] = cmd;
	ret = i2c_recv_data(SH200Q_I2C_BUS, NULL, 0, buf, receive_buf_len+1);
#if ENABLE_MPU6886_I2C_DEBUG
	printf("i2c_data read %x:", cmd);
	for(int i=0; i<receive_buf_len+1; i++){
		printf("%x ", buf[i]);
	}
	printf("(ret:%d)\r\n", ret);
#endif
	memcpy(receive_buf, buf+1, receive_buf_len);
	return ret;

}

uint8_t SH200Q_i2c_recv_byte(uint8_t cmd)
{
	uint8_t buf = 0;
	int ret = SH200Q_i2c_recv_data(cmd, 1, &buf);

	return ret ? 0 : buf;
}


int SH200Q_Init(void)
{
	// setup GPIOs for I2C & the I2C
	fpioa_set_function(SH200Q_I2C_PIN_SCL, FUNC_I2C0_SCLK);
	fpioa_set_function(SH200Q_I2C_PIN_SDA, FUNC_I2C0_SDA);
	i2c_init(SH200Q_I2C_BUS, SH200Q_I2C_SLA, 7, 400000);

	if( SH200Q_i2c_recv_byte(SH200Q_WHOAMI) != SH200Q_WHOAMI_VALUE ){
		printf("This device doesn't have SH200Q.\r\n");
		// WHO AM I's register will return 0x18. Otherwise, it's not SH200Q.
		return -1;
	}

	delay(1);

	// Device reset to default
	SH200Q_i2c_send_byte(SH200Q_PWR_MGMT_1, SH200Q_PWR_MGMT_1_RESET);
	delay(10);

	// Device CLKSEL=0 (Internal 20MHz, Enable TEMP sensor, Gyro, Not Sleep)
	SH200Q_i2c_send_byte(SH200Q_PWR_MGMT_1, 0);
	delay(10);

	// Enable X/Y/Z accelerometer, X/Y/Z gyro
	SH200Q_i2c_send_byte(SH200Q_PWR_MGMT_2, 0);
	delay(10);

	// setup accelerometer sensor
	SH200Q_i2c_send_byte(SH200Q_ACCEL_CONFIG, SH200Q_ACCEL_CONFIG_8G);
	delay(1);
	// accelerometer : 3-DB BW 218.1Hz, Noise BW:235Hz, RATE:1KHz
	SH200Q_i2c_send_byte(SH200Q_ACCEL_CONFIG2, 0);
	delay(1);

	// setup gyro sensor
	SH200Q_i2c_send_byte(SH200Q_GYRO_CONFIG, SH200Q_GYRO_CONFIG_2000DPSG);
	delay(1);
	// gyro : Disable FSYNC, Loop FIFO, 3-DB BW 176Hz, Noise BW 177Hz, RATE:1KHz
	SH200Q_i2c_send_byte(SH200Q_CONFIG, 0x01);
	delay(1);

	// Set SAMPLE RATE DIVIDER
	SH200Q_i2c_send_byte(SH200Q_SMPLRT_DIV, 0x05);
	delay(1);

	// DISABLE ALL INTERRUPT from MPU6886
	SH200Q_i2c_send_byte(SH200Q_INT_ENABLE, 0);
	delay(1);

	// Disable FIFO_EN
	SH200Q_i2c_send_byte(SH200Q_USER_CTRL, 0);
	delay(1);

	// Disable each sensor's FIFO_EN
	SH200Q_i2c_send_byte(SH200Q_FIFO_EN, 0);
	delay(1);

	// LATCH_INT_EN=1
	SH200Q_i2c_send_byte(SH200Q_INT_PIN_CFG, SH200Q_INT_PIN_CFG_VAL);
	delay(1);

	// Enable DATA_RDY_INT_EN
	SH200Q_i2c_send_byte(SH200Q_INT_ENABLE, SH200Q_INT_ENABLE_VAL);
	delay(10);

	return 0;
}

void SH200Q_getGyroData(float* gyroX, float* gyroY, float* gyroZ)
{
	uint8_t buf[6] = {0,0,0,0,0,0};
	SH200Q_i2c_recv_data(SH200Q_ACCEL_XOUT_H, 6, buf);

	*gyroX=(float)(((int16_t)buf[0]<<8) | buf[1]) * g_gyroK;
	*gyroY=(float)(((int16_t)buf[2]<<8) | buf[3]) * g_gyroK;
	*gyroZ=(float)(((int16_t)buf[4]<<8) | buf[5]) * g_gyroK;
}

void SH200Q_getAccelData(float* accelX, float* accelY, float* accelZ)
{
	uint8_t buf[6] = {0,0,0,0,0,0};
	SH200Q_i2c_recv_data(SH200Q_GYRO_XOUT_H, 6, buf);

	*accelX=(float)(((int16_t)buf[0]<<8) | buf[1]) * g_accelK;
	*accelY=(float)(((int16_t)buf[2]<<8) | buf[3]) * g_accelK;
	*accelZ=(float)(((int16_t)buf[4]<<8) | buf[5]) * g_accelK;
}

void SH200Q_getAhrsData(float* pitch, float* roll, float* yaw)
{

}



#ifdef __cplusplus
}
#endif /* __cplusplus */
