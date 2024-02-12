/*
 * usb_receive.c
 *
 *  Created on: Feb 11, 2024
 *      Author: Kyle
 */

#include "main.h"
#include "usb_receive.h"
#include "usb_dfu.h"
#include "stdbool.h"
#include "string.h"
#include "usbd_cdc_if.h"

uint8_t buffer[64];
uint32_t len = 0;

void receiveData(uint8_t* Buf, uint32_t *Len)
{
	char cdcBuffer[32];
	bool success = false;

	for(int i = 0; i < *Len; i++)
	{
		CDC_Transmit_FS(Buf + i, 1);
		if(*(Buf + i) == 0x21)	// !
			USB_EnableDfu();
		else if(*(Buf + i) == 0x0A || *(Buf + i) == 0x0D)	// CR or LF
		{
			if (len != 0)
			{
				switch(buffer[0])
				{
				case 0x74:	// t
					if(len != 7) break;
					uint8_t hours = ((buffer[1] - 0x30) * 10) + (buffer[2] - 0x30);
					if(hours > 23) break;
					uint8_t minutes = ((buffer[3] - 0x30) * 10) + (buffer[4] - 0x30);
					if(minutes > 59) break;
					uint8_t seconds = ((buffer[5] - 0x30) * 10) + (buffer[6] - 0x30);
					if(seconds > 59) break;
					setTime(seconds, minutes, hours);
					success = true;
					break;
				case 0x68:	// h
					if(len != 2) break;
					if(buffer[1] != 0x30 && buffer[1] != 0x31) break;
					bool is24HourTime = buffer[1] == 0x31 ? true : false;
					setTimeType(is24HourTime);
					success = true;
					break;
				case 0x66:	// f
					if(len != 3) break;
					uint8_t frequency = ((buffer[1] - 0x30) * 10) + (buffer[2] - 0x30);
					if(frequency == 0 || frequency > 59) break;
					setClockFrequency(frequency);
					success = true;
				}
				len = 0;
				if(success)
					sprintf(cdcBuffer, "Okay\r\n");
				else
					sprintf(cdcBuffer, "Error. Invalid command\r\n");

				CDC_Transmit_FS((uint8_t *) cdcBuffer, strlen(cdcBuffer));
			}
		}
		else
		{
			buffer[len] = *(Buf + i);
			len++;
		}
	}

}


