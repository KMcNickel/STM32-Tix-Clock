/*
 * clockManager.h
 *
 *  Created on: Feb 3, 2024
 *      Author: Kyle
 */

#ifndef INC_CLOCKMANAGER_H_
#define INC_CLOCKMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define COLOR_COUNT 13

#include "stm32g4xx_hal.h"
#include "ws2812b.h"
#include "stdbool.h"
#include "drbg/cmox_ctr_drbg.h"
#include "usbd_cdc_if.h"

#define DS3231_TIME_SECOND_REGISTER 0x00
#define DS3231_TIME_MINUTE_REGISTER 0x01
#define DS3231_TIME_HOUR_REGISTER 0x02
#define DS3231_TIME_DAY_REGISTER 0x03
#define DS3231_TIME_DATE_REGISTER 0x04
#define DS3231_TIME_MONTH_REGISTER 0x05
#define DS3231_TIME_YEAR_REGISTER 0x06
#define DS3231_ALARM_1_SECOND_REGISTER 0x07
#define DS3231_ALARM_1_MINUTE_REGISTER 0x08
#define DS3231_ALARM_1_HOUR_REGISTER 0x09
#define DS3231_ALARM_1_DAY_DATE_REGISTER 0x0A
#define DS3231_ALARM_2_MINUTE_REGISTER 0x0B
#define DS3231_ALARM_2_HOUR_REGISTER 0x0C
#define DS3231_ALARM_2_DAY_DATE_REGISTER 0x0D
#define DS3231_CONTROL_REGISTER 0x0E
#define DS3231_CONTROL_STATUS_REGISTER 0x0F
#define DS3231_AGING_OFFSET_REGISTER 0x10
#define DS3231_TEMP_MSB_OFFSET_REGISTER 0x11
#define DS3231_TEMP_LSB_OFFSET_REGISTER 0x12
class clockManager
{
private:
	TIM_HandleTypeDef * pwmTimer;
	cmox_ctr_drbg_handle_t random;
	cmox_drbg_handle_t * random_ctx;
	RNG_HandleTypeDef * trng;
	I2C_HandleTypeDef * clockI2c;
	uint32_t pwmChannel;
	ws2812b ledGrid;

	uint8_t lastColor[4] = {0, 0, 0, 0};
	uint8_t currentTime[4] = {0, 0, 0, 0};

	uint32_t colors[COLOR_COUNT] = {
			0x00008000,
			0x00800000,
			0x00000080,
			0x00404000,
			0x00400040,
			0x00004040,
			0x00202020,
			0x00602000,
			0x00600020,
			0x00006020,
			0x00206000,
			0x00200060,
			0x00002060
	};
	void updateSection(uint8_t sectionNum, uint8_t count);
	bool getTime();

public:
	bool use24HourTime = false;
	clockManager(TIM_HandleTypeDef * timer, uint32_t channel, RNG_HandleTypeDef * rng, I2C_HandleTypeDef * i2c);
	void setupRTC();
	void updateAll();
	void pulseCompleted();
};



#ifdef __cplusplus
}
#endif

#endif /* INC_CLOCKMANAGER_H_ */
