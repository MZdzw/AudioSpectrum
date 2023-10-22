#ifndef __WS2812BDRIVER_H__
#define __WS2812BDRIVER_H__
#include <stdint.h>
#include <stdbool.h>
#include "spiBuffer.h"

#define MAX_SECTORS 5


typedef enum ColorTypeLastUsed_e
{
    RGB, HSV
} ColorTypeLastUsed_e;

typedef enum DimmingDirection_e
{
    DESCENDING = -1,
    NONE,
    ASCENDING,
} DimmingDirection_e;

typedef struct Ws2812b_RGB_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Ws2812b_RGB_t;

typedef struct Ws2812b_HSV_t
{
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
} Ws2812b_HSV_t;

typedef struct Ws2812b_Color_t
{
    Ws2812b_RGB_t rgb;
    Ws2812b_HSV_t hsv;
    ColorTypeLastUsed_e lastColor; 
} Ws2812b_Color_t;

typedef struct Ws2812b_Diode_t
{
    Ws2812b_Color_t diodeColor;
    DimmingDirection_e dimDirection;
    struct Ws2812b_Diode_t* next;
} Ws2812b_Diode_t;

typedef struct Ws2812b_Sector_t
{
    Ws2812b_Diode_t* firstDiode;
    Ws2812b_Diode_t* lastDiode;

    // unsigned int startDiode;
    // unsigned int endDiode;
    bool isUsed;
} Ws2812b_Sector_t;

#ifndef TESTING
typedef struct Ws2812b_Driver_t Ws2812b_Driver_t;
#else
typedef struct Ws2812b_Driver_t
{
    unsigned int activeSectors;
    SpiWs2812B_t* deviceBuffer;
    Ws2812b_Diode_t diodes[WS2812B_DIODES];
    Ws2812b_Sector_t sectors[MAX_SECTORS];
} Ws2812b_Driver_t;
#endif

Ws2812b_Driver_t* Ws2812b_initObject(void);
bool SetSector(Ws2812b_Driver_t* this, const uint32_t id, const uint32_t startDiode, const uint32_t endDiode);
bool RemoveSector(Ws2812b_Driver_t* this, const uint32_t id);

void SetDiodeColorRGB(Ws2812b_Diode_t* diode, const Ws2812b_RGB_t rgb);
void SetDiodeColorHSV(Ws2812b_Diode_t* diode, const Ws2812b_HSV_t hsv);
Ws2812b_RGB_t GetDiodeColorRGB(const Ws2812b_Diode_t* diode);
Ws2812b_HSV_t GetDiodeColorHSV(const Ws2812b_Diode_t* diode);

SpiWs2812B_t* GetDeviceBuffer(Ws2812b_Driver_t* this);

void SetActiveSectors(Ws2812b_Driver_t* this, unsigned int val);
unsigned int GetActiveSectors(Ws2812b_Driver_t* this);

void SetDiodeToDeviceBuffer(Ws2812b_Driver_t* this, const Ws2812b_Diode_t* diode);

void SendDeviceBuffer(Ws2812b_Driver_t* this);
void SendPartOfDeviceBuffer(Ws2812b_Driver_t* this, uint32_t diodes);

Ws2812b_Sector_t* GetSectors(Ws2812b_Driver_t* this);

Ws2812b_Diode_t* GetDiodesArray(Ws2812b_Driver_t* this);


#endif