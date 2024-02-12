/*
 * syscalls_override.c
 *
 *  Created on: Feb 11, 2024
 *      Author: Kyle
 */

#include "stm32g4xx_hal.h"

int __io_putchar(int ch)
{
	return (int) ITM_SendChar(ch);
}

