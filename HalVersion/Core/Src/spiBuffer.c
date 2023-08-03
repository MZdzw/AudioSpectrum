#include <stdlib.h>
#include <string.h>
#include "spiBuffer.h"

typedef struct SpiWs2812B_t
{
    uint8_t spiBuffer[WS2812B_DIODES * 
                      SPI_BYTES_PER_WS2812B_BIT * 8 * 3];
} SpiWs2812B_t;

static SpiWs2812B_t obj = {0, 0};

SpiWs2812B_t* initObject(void)
{
    return &obj;
}

uint8_t getSpiBufferElement(SpiWs2812B_t* this, uint32_t id)
{    
    return this->spiBuffer[id];
}

uint8_t* getSpiBufferPointer(SpiWs2812B_t* this)
{
    return this->spiBuffer;
}

void setSpiBufferElement(SpiWs2812B_t* this, uint8_t val, uint32_t id)
{
    this->spiBuffer[id] = val;
}
