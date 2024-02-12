/*
 * usb_dfu.h
 *
 *  Created on: Feb 11, 2024
 *      Author: Kyle
 */

#ifndef INC_USB_DFU_H_
#define INC_USB_DFU_H_

#ifdef __cplusplus
extern "C" {
#endif

void USB_CheckDfuEnabled();
void USB_EnableDfu();

#ifdef __cplusplus
}
#endif

#endif /* INC_USB_DFU_H_ */
