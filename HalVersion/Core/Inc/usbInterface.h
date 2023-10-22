#ifndef __USBINTERFACE_H__
#define __USBINTERFACE_H__
#include "stdint.h"

void SendBufferOverUsb(uint8_t* bufferToSendPointer, uint32_t bytes);

#endif
