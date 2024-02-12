/*
 * usb_dfu.c
 *
 *  Created on: Feb 11, 2024
 *      Author: Kyle
 */

#include "main.h"
#include "usb_dfu.h"
#include "stm32g4xx_hal_rcc.h"

#define DFU_DISABLED 0x00
#define DFU_ENABLED 0x11
#define MCU_BOOTLOADER_STARTADDR 0x1FFF0000	//The address of the MCU's system memory (bootloader)

void (*SysMemBootJump) (void);
uint8_t switchToBootloader __attribute__ ((section (".noinit")));

void USB_BootloaderInit()
{
	switchToBootloader = DFU_DISABLED;	//Reset the variable to prevent being stuck in the bootloader (since a device reset wont change it)
	volatile uint32_t addr = MCU_BOOTLOADER_STARTADDR;	//The system memory start address
	SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));	//Point the PC to the System Memory reset vector

	HAL_RCC_DeInit();		//Reset the system clock
	SysTick->CTRL = 0;		//Reset the  SysTick Timer
	SysTick->LOAD = 0;
	SysTick->VAL  = 0;

	__set_MSP(*(uint32_t *)addr);	//Set the Main Stack Pointer

	SysMemBootJump();				//Run our virtual function defined above that sets the PC

	while(1);
}

void USB_CheckDfuEnabled()
{
	if (switchToBootloader == DFU_ENABLED)
		USB_BootloaderInit();
}

void USB_EnableDfu()
{
	SYSCFG->MEMRMP &= 0xFFFFFFF9;		//Remap the memory (may not be necessary)
	SYSCFG->MEMRMP |= 1;
	switchToBootloader = DFU_ENABLED;	//Set the noinit variable to be read by startup code
	NVIC_SystemReset();					//Reset the system
}
