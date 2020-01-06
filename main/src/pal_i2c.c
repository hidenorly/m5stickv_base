/* 
 Copyright (C) 2020 hidenorly

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fpioa.h"
#include "FreeRTOS.h"
#include "task.h"

#include "pal_i2c.h"

#include "sipeed_i2c.h"
#include "type_defines.h"

#define ENABLE_I2C_DEBUG 1


void i2c_initialize(uint8_t i2cBusChannel, uint8_t pinSDA, uint8_t pinSCL, uint32_t clock)
{
	// setup GPIOs for I2C & the I2C
	fpioa_set_function(i2cBusChannel, FUNC_I2C0_SCLK + i2cBusChannel * 2);
	fpioa_set_function(i2cBusChannel, FUNC_I2C0_SDA + i2cBusChannel * 2);

	maix_i2c_init(i2cBusChannel, 7, clock);
}

void i2c_finalize(uint8_t i2cBusChannel)
{
	maix_i2c_deinit(i2cBusChannel);
}

int i2c_send_multiple_data(uint8_t i2cBusChannel, int8_t slaveAddress, uint8_t cmd, const uint8_t *send_buf, size_t send_buf_len)
{
	int ret = 0;
	uint8_t buf[send_buf_len+1];
	buf[0] = cmd;
	memcpy(buf+1, send_buf, send_buf_len);
	ret = maix_i2c_send_data(i2cBusChannel, slaveAddress, buf, send_buf_len+1, 20);
#if ENABLE_I2C_DEBUG
	printf("i2c_data write %x:", cmd);
	for(int i=1; i<send_buf_len+1; i++){
		printf("%x ", buf[i]);
	}
	printf("(ret:%d)\r\n", ret);
#endif
	return ret;
}

int i2c_send_byte(uint8_t i2cBusChannel, uint8_t slaveAddress, uint8_t cmd, const uint8_t sendData)
{
	uint8_t buf[2];
	buf[0] = cmd;
	buf[1] = sendData;
	return i2c_send_multiple_data(i2cBusChannel, slaveAddress, cmd, buf, 2);
}

int i2c_recv_multiple_data(uint8_t i2cBusChannel, uint8_t slaveAddress, uint8_t cmd, uint8_t *receive_buf, size_t receive_buf_len)
{
	int ret = 0;
	ret = maix_i2c_recv_data(i2cBusChannel, slaveAddress, &cmd, 1, receive_buf, receive_buf_len, 20);
#if ENABLE_I2C_DEBUG
	printf("i2c_data read %x:", cmd);
	for(int i=0; i<receive_buf_len; i++){
		printf("%x ", receive_buf[i]);
	}
	printf("(ret:%d)\r\n", ret);
#endif
	return ret;
}

uint8_t i2c_recv_byte(uint8_t i2cBusChannel, uint8_t slaveAddress, uint8_t cmd)
{
	uint8_t buf;
	int ret = i2c_recv_multiple_data(i2cBusChannel, slaveAddress, cmd, &buf, 1);

	return ret ? 0 : buf;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

