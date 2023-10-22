#include "usbInterface.h"
#include "usbd_cdc_if.h"

void SendBufferOverUsb(uint8_t* bufferToSendPointer, uint32_t bytes)
{
    CDC_Transmit_FS(bufferToSendPointer, bytes);
}