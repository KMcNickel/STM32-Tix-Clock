/*
 * clockManager.cpp
 *
 *  Created on: Feb 3, 2024
 *      Author: Kyle
 */


#include "clockManager.h"

uint8_t bcdToDecimal(uint8_t bcd)
{
	return ((bcd / 10) << 4) & (bcd % 10);
}

uint8_t decimalToBcd(uint8_t dec)
{
	return ((dec >> 4) * 10) & (dec & 0xf);
}

clockManager::clockManager(TIM_HandleTypeDef * timer, uint32_t channel,
		RNG_HandleTypeDef * rng, I2C_HandleTypeDef * i2c) : ledGrid(timer, channel)
{
	pwmTimer = timer;
	pwmChannel = channel;
	trng = rng;
	clockI2c = i2c;
}

void clockManager::setupRTC()
{
	HAL_StatusTypeDef halRetval;
	char serialBuffer[32];
	uint8_t rtcDataBuffer;

	//Set the Control Register on the DS3231 to enable the square wave output and set it to 1Hz
	rtcDataBuffer = 0x00;
	halRetval = HAL_I2C_Mem_Write(clockI2c, 0xD0, DS3231_CONTROL_REGISTER, 1, &rtcDataBuffer, 1, 1000);
	if (halRetval == HAL_OK)
	{
		sprintf(serialBuffer, "Unable to setup RTC\r\n");

		CDC_Transmit_FS((uint8_t *) serialBuffer, strlen(serialBuffer));
	}
}

bool clockManager::getTime()
{
	HAL_StatusTypeDef halRetval;
	uint8_t buffer[2];
	uint8_t hours;

	halRetval = HAL_I2C_Mem_Read(clockI2c, 0xD0, DS3231_TIME_MINUTE_REGISTER, 1, buffer, 2, 1000);
	if (halRetval == HAL_OK)
	{
		if (use24HourTime)
		{
			currentTime[0] = buffer[1] >> 4;
			currentTime[1] = buffer[1] & 0xf;
		}
		else
		{
			hours = bcdToDecimal(buffer[1]);
			hours %= 12;
			hours = decimalToBcd(hours);
			currentTime[0] = hours >> 4;
			currentTime[1] = hours & 0xf;
		}

		currentTime[2] = buffer[0] >> 4;
		currentTime[3] = buffer[0] & 0xf;

		return true;
	}
	return false;
}

void clockManager::updateAll()
{
	HAL_StatusTypeDef halRetval;
	cmox_drbg_retval_t drbgRetval;
	char serialBuffer[32];


	if (random_ctx == 0)
	{
		char personalizationString[] = "ClockManager";
		uint8_t entropy[32];

		for (int i = 0; i < 8; i++)
		{
			halRetval = HAL_RNG_GenerateRandomNumber(trng, (uint32_t *) (entropy + (4 * i)));
			HAL_Delay(1);
		}

		random_ctx = cmox_ctr_drbg_construct(&random, CMOX_CTR_DRBG_AES256_FAST);

		drbgRetval = cmox_drbg_init(random_ctx, entropy, sizeof(entropy),
						(uint8_t *) personalizationString, sizeof(personalizationString),
						NULL, 0);
	}

	if (getTime())
	{
		sprintf(serialBuffer, "%d%d:%d%d\r\n", currentTime[0], currentTime[1], currentTime[2], currentTime[3]);
		for (int i = 0; i < 4; i++)
			updateSection(i, currentTime[3 - i]);
	}
	else
		sprintf(serialBuffer, "RTC Error\r\n");

	CDC_Transmit_FS((uint8_t *) serialBuffer, strlen(serialBuffer));

	ledGrid.send();
}


void clockManager::updateSection(uint8_t sectionNum, uint8_t count)
{
	uint8_t rand;
	uint8_t colorIndex;
	uint8_t sectionCount;
	uint8_t sectionOffset;
	uint8_t temp;
	cmox_drbg_retval_t drbgRetval;
	uint8_t ledList[9];

	drbgRetval = cmox_drbg_generate(random_ctx, NULL, 0, &rand, 1);
	colorIndex = rand % COLOR_COUNT;

	while (true)
	{
		for (int i = 0; i <= sectionNum; i++)
			if (colorIndex == lastColor[i])
			{
				colorIndex++;
				if(colorIndex == COLOR_COUNT)
					colorIndex = 0;
				continue;
			}
		break;
	}

	switch (sectionNum)
	{
	case 0:
		sectionCount = 9;
		sectionOffset = 0;
		break;
	case 1:
		sectionCount = 6;
		sectionOffset = 9;
		break;
	case 2:
		sectionCount = 9;
		sectionOffset = 15;
		break;
	case 3:
		sectionCount = 3;
		sectionOffset = 24;
		break;
	}

	if (count > sectionCount)
		return;

	for (int i = 0; i < sectionCount; i++)
		ledList[i] = i + sectionOffset;

	for (int i = (sectionCount - 1); i > 0; i--)
	{
		drbgRetval = cmox_drbg_generate(random_ctx, NULL, 0, &rand, 1);
		int j = rand % (i + 1);
		temp = *(ledList + i);
		*(ledList + i) = *(ledList + j);
		*(ledList + j) = temp;
	}

	for (int i = 0; i < sectionCount; i++)
		if(i < count)
			ledGrid.setBrightness(ledList[i], colors[colorIndex]);
		else
			ledGrid.setBrightness(ledList[i], 0);

	lastColor[sectionNum] = colorIndex;
}

void clockManager::pulseCompleted()
{
	ledGrid.timerEnded();
}
