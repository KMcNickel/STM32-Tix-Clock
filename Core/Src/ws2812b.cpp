/*
 * ws2812b.c
 *
 *  Created on: Feb 3, 2024
 *      Author: Kyle
 */

#include "ws2812b.h"

ws2812b::ws2812b(TIM_HandleTypeDef * timer, uint32_t channel)
{
	pwmTimer = timer;
	pwmChannel = channel;

	for (int i = 0; i < PWM_ARRAY_LENGTH; i++)
		pwmData[i] = 0;
}

void ws2812b::setBrightness(int ledNum, int green, int red, int blue)
{
	if(ledNum > LED_COUNT || ledNum < 0) return;

	uint32_t data = (green << 16) | (red << 8) | blue;

	for (int i = 0; i < LED_DATA_BITS; i++)
	{
		if (data & (1 << i))
			pwmData[(23 - i) + (LED_DATA_BITS * ledNum)] = 20;
		else
			pwmData[(23 - i) + (LED_DATA_BITS * ledNum)] = 10;
	}
}

void ws2812b::setBrightness(int ledNum, uint32_t color)
{
	if(ledNum > LED_COUNT || ledNum < 0) return;

	for (int i = 0; i < LED_DATA_BITS; i++)
	{
		if (color & (1 << i))
			pwmData[(23 - i) + (LED_DATA_BITS * ledNum)] = 20;
		else
			pwmData[(23 - i) + (LED_DATA_BITS * ledNum)] = 10;
	}
}

void ws2812b::send()
{
	sendingInProgress = true;
	HAL_TIM_PWM_Start_DMA(pwmTimer, pwmChannel, (uint32_t *) pwmData, PWM_ARRAY_LENGTH);
	while (sendingInProgress) {}
}

void ws2812b::clearAll()
{
	for (int i = 0; i < PWM_ARRAY_DATA_LENGTH; i++)
		pwmData[i] = 10;
	for (int i = PWM_ARRAY_DATA_LENGTH; i < PWM_ARRAY_LENGTH; i++)
		pwmData[i] = 0;

	send();
}

void ws2812b::timerEnded()
{
	//HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	pwmTimer->Instance->CCR1 = 0;
	sendingInProgress = false;
}
