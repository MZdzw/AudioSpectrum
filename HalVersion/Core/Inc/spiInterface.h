#ifndef __SPIINTERFACE_H__
#define __SPIINTERFACE_H__
#include "stdint.h"

void SendBufferOverSpi(uint8_t* bufferToSendPointer, uint32_t bytes);

#endif
