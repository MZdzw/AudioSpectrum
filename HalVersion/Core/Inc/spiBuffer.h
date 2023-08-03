#ifndef __SPIBUFFER_H__
#define __SPIBUFFER_H__
#include <stdint.h>
#include "ws2812bHW.h"

#define SPI_BYTES_PER_WS2812B_BIT 3
#define SPI_ONE     0b11111111
#define SPI_ZERO    0b00000000

// typedef struct SpiWs2812B_t SpiWs2812B_type;
typedef struct SpiWs2812B_t SpiWs2812B_t;

// public methods which grants access to object data
SpiWs2812B_t* initObject(void);

uint8_t getSpiBufferElement(SpiWs2812B_t* this, uint32_t id);
uint8_t* getSpiBufferPointer(SpiWs2812B_t* this);
void setSpiBufferElement(SpiWs2812B_t* this, uint8_t val ,uint32_t id);

#endif
