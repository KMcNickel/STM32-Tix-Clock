/*
 * ws2812b.h
 *
 *  Created on: Feb 3, 2024
 *      Author: Kyle
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LED_COUNT 27
#define LED_DATA_BITS 24
#define LED_RESET_BITS 45
#define PWM_ARRAY_DATA_LENGTH (LED_COUNT * LED_DATA_BITS)
#define PWM_ARRAY_LENGTH ((LED_COUNT * LED_DATA_BITS) + LED_RESET_BITS)


#include "stm32g4xx_hal.h"
#include "stdbool.h"
#include <stdio.h>

class ws2812b
{
public:
	ws2812b(TIM_HandleTypeDef * timer, uint32_t channel);
	void setBrightness(int ledNum, int green, int red, int blue);
	void setBrightness(int ledNum, uint32_t color);
	void send();
	void clearAll();
	void timerEnded();

private:
	TIM_HandleTypeDef * pwmTimer;
	uint32_t pwmChannel;
	uint32_t pwmData[PWM_ARRAY_LENGTH];
	bool sendingInProgress = false;
};



#ifdef __cplusplus
}
#endif

#endif /* INC_WS2812B_H_ */
