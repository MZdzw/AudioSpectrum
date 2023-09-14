#ifndef TESTING
#include "spi.h"
#else
#include "mock_stm32f4xx_hal_spi.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ws2812bDriver.h"

// Private object members
#ifdef TESTING
SPI_HandleTypeDef hspi1;
#endif

#ifndef TESTING
typedef struct Ws2812b_Driver_t
{
    SpiWs2812B_t* deviceBuffer;
    Ws2812b_Color_t diodeColors[WS2812B_DIODES];
    Ws2812b_Sector_t sectors[MAX_SECTORS];
} Ws2812b_Driver_t;
#endif

static unsigned int activeSectors;  

//////

//static functions
static void GetSectorsRange(Ws2812b_Driver_t* this, uint32_t* start, uint32_t* end,
                            uint32_t* startEdge, uint32_t* endEdge)
{
    *start = this->sectors[0].startDiode;
    *end = this->sectors[0].endDiode;
    *startEdge = UINT32_MAX;
    *endEdge = 0;
    for (uint8_t i = 0; i < MAX_SECTORS; i++)
    {
        if(this->sectors[i].isUsed)
        {
            if(this->sectors[i].startDiode <= this->sectors[i].endDiode)
            {
                if (this->sectors[i].startDiode < *start)
                {
                    *start = this->sectors[i].startDiode;
                }
                if (this->sectors[i].endDiode > *end)
                {
                    *end = this->sectors[i].endDiode;
                }
            }
            else
            {
                if (this->sectors[i].startDiode < *startEdge)
                {
                    *startEdge = this->sectors[i].startDiode;
                }
                if (this->sectors[i].endDiode > *endEdge)
                {
                    *endEdge = this->sectors[i].endDiode;
                }
            }
        }
    }
}

static bool CheckOverlapping(const uint32_t newStartDiode, const uint32_t newEndDiode, 
                             const uint32_t startDiode, const uint32_t endDiode)
{
    if (startDiode <= endDiode)
    {
        if (newStartDiode <= newEndDiode)
        {
            if ((newStartDiode < startDiode && newEndDiode < startDiode) ||
                (newStartDiode > endDiode && newEndDiode > endDiode))
                return false;
        }
        else
        {
            if (newStartDiode > endDiode && newEndDiode < startDiode)
                return false;
        }
    }
    else
    {
        if (newStartDiode <= newEndDiode)
        {
            if (newStartDiode > endDiode && newEndDiode < startDiode)
                return false;
        }
    }
    return true;
}

// Objects
static Ws2812b_Driver_t obj = {0, {{0, 0, 0}}, {{0, 0, false}}};

Ws2812b_Driver_t* Ws2812b_initObject(void)
{
    obj.deviceBuffer = initObject();
    SetSector(&obj, 0, 0, WS2812B_DIODES - 1);
    return &obj;
}

bool SetSector(Ws2812b_Driver_t* this, const uint32_t id, const uint32_t startDiode, const uint32_t endDiode)
{
    if (id >= MAX_SECTORS)
        return false;
    if (this->sectors[id].isUsed)
        return false;
    if (startDiode >= WS2812B_DIODES || endDiode >= WS2812B_DIODES)
        return false;
    
    // check if sector is not overlapping with different sectors
    if (activeSectors != 0)
    {
        uint32_t start, end, startEdge, endEdge;
        GetSectorsRange(this, &start, &end, &startEdge, &endEdge);
        if(CheckOverlapping(startDiode, endDiode, start, end) ||
           CheckOverlapping(startDiode, endDiode, startEdge, endEdge))
           return false;
    }

    this->sectors[id].startDiode = startDiode;
    this->sectors[id].endDiode = endDiode;
    this->sectors[id].isUsed = true;
    activeSectors++;    
    return true;
}

bool RemoveSector(Ws2812b_Driver_t* this, const uint32_t id)
{
    if (id >= MAX_SECTORS)
        return false;
    if (!(this->sectors[id].isUsed))
        return false;
    
    this->sectors[id].startDiode = 0;
    this->sectors[id].endDiode = 0;
    this->sectors[id].isUsed = false;
    activeSectors--;

    return true;
}

static inline void SetDiodeColor(Ws2812b_Driver_t* this, uint32_t id, Ws2812b_RGB_t* colorRGB, Ws2812b_HSV_t* colorHSV, bool isHSVSet)
{
    this->diodeColors[id].rgb = *colorRGB;
    if (isHSVSet)
    {
        this->diodeColors[id].hsv = *colorHSV;
    }
}

static inline void SetColorToDeviceBuffer(Ws2812b_Driver_t* this, uint32_t id, Ws2812b_Color_e color, uint8_t colorVal)
{
    for(int8_t i = 7; i >= 0; i--)
    {
        if((colorVal & (1 << i)) == 0)
        {
            setSpiBufferElement(this->deviceBuffer, SPI_ONE, id * 72 + 144 + (7 - i) * 3 + 24 * color);
            setSpiBufferElement(this->deviceBuffer, SPI_ZERO, id * 72 + 144 + (7 - i) * 3 + 1 + 24 * color);
            setSpiBufferElement(this->deviceBuffer, SPI_ZERO, id * 72 + 144 + (7 - i) * 3 + 2 + 24 * color);
        }
        else
        {
            setSpiBufferElement(this->deviceBuffer, SPI_ONE, id * 72 + 144 + (7 - i) * 3 + 24 * color);
            setSpiBufferElement(this->deviceBuffer, SPI_ONE, id * 72 + 144 + (7 - i) * 3 + 1 + 24 * color);
            setSpiBufferElement(this->deviceBuffer, SPI_ZERO, id * 72 + 144 + (7 - i) * 3 + 2 + 24 * color);
        }
    }
}

void SetDiodeColorRGB(Ws2812b_Driver_t* this, uint32_t id, uint8_t red, uint8_t green, uint8_t blue)
{
    SetDiodeColor(this, id, &(Ws2812b_RGB_t){.red = red, .green = green, .blue = blue},
                  &(Ws2812b_HSV_t){0}, false);
    this->diodeColors[id].lastColor = RGB;
}

void SetDiodeColorHSV(Ws2812b_Driver_t* this, uint32_t id, uint16_t hue, uint8_t saturation, uint8_t value)
{
    Ws2812b_RGB_t colorRGB = {0};
    Ws2812b_HSV_t colorHSV = {0};
    if(hue > 360 || hue < 0 || saturation > 100 || saturation < 0 || value > 100 || value < 0)
    {
        SetDiodeColor(this, id, &colorRGB, &colorHSV, false);
        return;
    }
    colorHSV.hue = hue;
    colorHSV.saturation = saturation;
    colorHSV.value = value;

    // HSV to RGB calculation
    float s = saturation / 100.0f;
    float v = value / 100.0f;
    float C = s * v;
    float X = C * (1 - fabs((fmod(hue / 60.0f, 2) - 1)));
    float m = v - C;
    float r, g, b;
    if(hue >= 0 && hue < 60)
    {
        r = C;
        g = X;
        b = 0;
    }
    else if(hue >= 60 && hue < 120)
    {
        r = X;
        g = C;
        b = 0;
    }
    else if(hue >= 120 && hue < 180)
    {
        r = 0;
        g = C;
        b = X;
    }
    else if(hue >= 180 && hue < 240)
    {
        r = 0;
        g = X;
        b = C;
    }
    else if(hue >= 240 && hue < 300)
    {
        r = X;
        g = 0;
        b = C;
    }
    else
    {
        r = C;
        g = 0;
        b = X;
    }

    colorRGB.red = round((r + m) * 255);
    colorRGB.green = round((g + m) * 255);
    colorRGB.blue = round((b + m) * 255);

    SetDiodeColor(this, id, &colorRGB, &colorHSV, true);
    this->diodeColors[id].lastColor = HSV;
}

void GetDiodeColorRGB(Ws2812b_Driver_t* this, uint32_t id, uint8_t* red, uint8_t* green, uint8_t* blue)
{
    *red = this->diodeColors[id].rgb.red;
    *green = this->diodeColors[id].rgb.green;
    *blue = this->diodeColors[id].rgb.blue;
}

void GetDiodeColorHSV(Ws2812b_Driver_t* this, uint32_t id, uint16_t* hue, uint8_t* saturation, uint8_t* value)
{
    if (this->diodeColors[id].lastColor == HSV)
    {
        *hue = this->diodeColors[id].hsv.hue;
        *saturation = this->diodeColors[id].hsv.saturation;
        *value = this->diodeColors[id].hsv.value;
        return;
    }

    uint8_t r, g, b;
    GetDiodeColorRGB(this, id, &r, &g, &b);
    uint8_t rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);
    
    *value = round(100 * ((float)rgbMax / 255));
    if (*value == 0)
    {
        *hue = 0;
        *saturation = 0;
        return;
    }

    *saturation = round(100 * (float)(rgbMax - rgbMin) / rgbMax);
    if (*saturation == 0)
    {
        *hue = 0;
        return;
    }

    float hueF;
    if (rgbMax == r)
    {
        hueF = ((float)g - b) / (rgbMax - rgbMin);
    }
    else if (rgbMax == g)
    {
        hueF = 2.0f + ((float)b - r) / (rgbMax - rgbMin);
    }
    else
    {
        hueF = 4.0f + ((float)r - g) / (rgbMax - rgbMin);
    }

    if (hueF < 0)
    {
        *hue = (uint16_t)(round((hueF * 60)) + 360);    
    }
    else
    {
        *hue = (uint16_t)round((hueF * 60)); 
    }
}

void SetActiveSectors(Ws2812b_Driver_t* this, unsigned int val)
{
    activeSectors = val;
}

unsigned int GetActiveSectors(Ws2812b_Driver_t* this)
{
    return activeSectors;
}


SpiWs2812B_t* GetDeviceBuffer(Ws2812b_Driver_t* this)
{
    return this->deviceBuffer;
}

void SetDiodeToDeviceBuffer(Ws2812b_Driver_t* this, uint32_t id)
{
    uint8_t red, green, blue;
    GetDiodeColorRGB(this, id, &red, &green, &blue);
    SetColorToDeviceBuffer(this, id, GREEN, green);
    SetColorToDeviceBuffer(this, id, RED, red);
    SetColorToDeviceBuffer(this, id, BLUE, blue);
}

void SendDeviceBuffer(Ws2812b_Driver_t* this)
{
    for (uint32_t i = 0; i < WS2812B_DIODES; i++)
    {
        SetDiodeToDeviceBuffer(this, i);
    }

    HAL_SPI_Transmit_DMA(&hspi1, getSpiBufferPointer(this->deviceBuffer), WS2812B_DIODES * 24 * 3 + 144);
}

void SendPartOfDeviceBuffer(Ws2812b_Driver_t* this, uint32_t diodes)
{
    if (diodes > WS2812B_DIODES)
    {
        diodes = WS2812B_DIODES;
    }
    for (uint32_t i = 0; i < diodes; i++)
    {
        SetDiodeToDeviceBuffer(this, i);
    }

    HAL_SPI_Transmit_DMA(&hspi1, getSpiBufferPointer(this->deviceBuffer), diodes * 24 * 3 + 144);
}

Ws2812b_Sector_t* GetSectors(Ws2812b_Driver_t* this)
{
    return this->sectors;
}


Ws2812b_Color_t* GetDiodeColorsArray(Ws2812b_Driver_t* this)
{
    return this->diodeColors;
}
