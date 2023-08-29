#ifndef __WS2812BDRIVER_H__
#define __WS2812BDRIVER_H__
#include <stdint.h>
#include <stdbool.h>
#include "spiBuffer.h"

#define MAX_SECTORS 5

#ifndef TESTING
typedef struct Ws2812b_RGB_t Ws2812b_RGB_t;
typedef struct Ws2812b_Sector_t
{
    unsigned int startDiode;
    unsigned int endDiode;
    bool isUsed;
} Ws2812b_Sector_t;
// typedef struct Ws2812b_Sector_t Ws2812b_Sector_t;
#else
typedef struct Ws2812b_RGB_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Ws2812b_RGB_t;

typedef struct Ws2812b_Sector_t
{
    unsigned int startDiode;
    unsigned int endDiode;
    bool isUsed;
} Ws2812b_Sector_t;
#endif

typedef struct Ws2812b_HSV_t Ws2812b_HSV_t;
typedef struct Ws2812b_Driver_t Ws2812b_Driver_t;

Ws2812b_Driver_t* Ws2812b_initObject(void);
bool SetSector(Ws2812b_Driver_t* this, const uint32_t id, const uint32_t startDiode, const uint32_t endDiode);
bool RemoveSector(Ws2812b_Driver_t* this, const uint32_t id);

void SetDiodeColorRGB(Ws2812b_Driver_t* this, uint32_t id, uint8_t red, uint8_t green, uint8_t blue);
void SetDiodeColorHSV(Ws2812b_Driver_t* this, uint32_t id, uint16_t hue, uint8_t saturation, uint8_t value);
void GetDiodeColorRGB(Ws2812b_Driver_t* this, uint32_t id, uint8_t* red, uint8_t* green, uint8_t* blue);
void GetDiodeColorHSV(Ws2812b_Driver_t* this, uint32_t id, uint16_t* hue, uint8_t* saturation, uint8_t* value);


SpiWs2812B_t* GetDeviceBuffer(Ws2812b_Driver_t* this);

void SetActiveSectors(Ws2812b_Driver_t* this, unsigned int val);
unsigned int GetActiveSectors(Ws2812b_Driver_t* this);

void SetDiodeToDeviceBuffer(Ws2812b_Driver_t* this, uint32_t id);

void SendDeviceBuffer(Ws2812b_Driver_t* this);
void SendPartOfDeviceBuffer(Ws2812b_Driver_t* this, uint32_t diodes);

Ws2812b_Sector_t* GetSectors(Ws2812b_Driver_t* this);

#ifdef TESTING
Ws2812b_RGB_t* GetDiodeColorsArray(Ws2812b_Driver_t* this);
#endif

#endif