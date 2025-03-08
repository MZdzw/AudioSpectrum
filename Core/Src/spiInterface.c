#include "spiInterface.h"
#include "spi.h"

void SendBufferOverSpi(uint8_t* bufferToSendPointer, uint32_t bytes)
{
    HAL_SPI_Transmit_DMA(&hspi1, bufferToSendPointer, bytes);
}