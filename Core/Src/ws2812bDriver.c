#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ws2812bDriver.h"
#include "spiInterface.h"

// Private object members

#ifndef TESTING
typedef struct Ws2812b_Driver_t
{
    unsigned int activeSectors;
    SpiWs2812B_t* deviceBuffer;
    Ws2812b_Diode_t diodes[WS2812B_DIODES];
    Ws2812b_Sector_t sectors[MAX_SECTORS];
} Ws2812b_Driver_t;
#endif


//static functions

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

static void ResetSectorDiodesColor(Ws2812b_Driver_t* this, const Ws2812b_Sector_t* sector)
{
    Ws2812b_Diode_t* iter;
    for (iter = sector->firstDiode; iter != sector->lastDiode; iter = iter->next)
    {
        SetDiodeColorRGB(iter, ((Ws2812b_RGB_t){0, 0, 0}));
        SetDiodeToDeviceBuffer(this, iter);
    }
    SetDiodeColorRGB(iter, (Ws2812b_RGB_t){0, 0, 0});
    SetDiodeToDeviceBuffer(this, iter);
}

static void SetNextPointers(Ws2812b_Diode_t* head, Ws2812b_Diode_t* tail, Ws2812b_Diode_t* array)
{
    Ws2812b_Diode_t* tmp = head;
    if (head == tail)
    {
        head->next = tail;
        return;
    }

    while (head != tail)
    {
        if (head == array + WS2812B_DIODES - 1)
        {
            head->next = array;
            head = array;
        }
        else
        {
            head->next = head + 1;
            head++;
        }
    }
    head->next = tmp;
}

// Objects
static Ws2812b_Driver_t obj = 
{   0,
    NULL,                                              // deviceDriver     
    {{{{0, 0, 0}, {0, 0, 0}, RGB}, NONE, NULL}},       // 1st{ -> "array"; 2nd{ -> "Ws2812b_Color_t"; 3rd{ -> "rgb"; 4th{ -> "red, green, blue"
    {{NULL, NULL, false}}                        // sectors
};

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
    if (this->activeSectors != 0)
    {
        for (uint8_t i = 0; i < MAX_SECTORS; i++)
        {
            if (i != id)
            {
                if (this->sectors[i].isUsed)
                {
                    if (CheckOverlapping(startDiode, endDiode, (this->sectors[i].firstDiode - this->diodes),
                        (this->sectors[i].lastDiode - this->diodes)))
                        return false;
                }
            }
        }
    }

    this->sectors[id].firstDiode = &(GetDiodesArray(this)[startDiode]);
    this->sectors[id].lastDiode = &(GetDiodesArray(this)[endDiode]);
    SetNextPointers(this->sectors[id].firstDiode, this->sectors[id].lastDiode, GetDiodesArray(this));
    this->sectors[id].isUsed = true;
    this->activeSectors++;    
    return true;
}

bool RemoveSector(Ws2812b_Driver_t* this, const uint32_t id)
{
    if (id >= MAX_SECTORS)
        return false;
    if (!(this->sectors[id].isUsed))
        return false;
    
    ResetSectorDiodesColor(this, &GetSectors(this)[id]);
    this->sectors[id].firstDiode = NULL;
    this->sectors[id].lastDiode = NULL;
    this->sectors[id].isUsed = false;
    this->activeSectors--;

    return true;
}

static inline void SetDiodeColor(Ws2812b_Diode_t* diode, const Ws2812b_RGB_t colorRGB, const Ws2812b_HSV_t colorHSV, bool isHSVSet)
{
    diode->diodeColor.rgb = colorRGB;
    if (isHSVSet)
    {
        diode->diodeColor.hsv = colorHSV;
    }
}

static inline void SetColorToDeviceBuffer(Ws2812b_Driver_t* this, const Ws2812b_Diode_t* diode, const Ws2812b_Color_e color, uint8_t colorVal)
{
    uint32_t id = (diode - this->diodes);
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

void SetDiodeColorRGB(Ws2812b_Diode_t* diode, const Ws2812b_RGB_t rgb)
{
    SetDiodeColor(diode, rgb, (Ws2812b_HSV_t){0}, false);
    diode->diodeColor.lastColor = RGB;
}

void SetDiodeColorHSV(Ws2812b_Diode_t* diode, const Ws2812b_HSV_t hsv)
{
    Ws2812b_RGB_t colorRGB = {0};
    Ws2812b_HSV_t colorHSV = {0};
    if(hsv.hue > 360 || hsv.saturation > 100 || hsv.value > 100)
    {
        SetDiodeColor(diode, colorRGB, colorHSV, false);
        return;
    }
    colorHSV.hue = hsv.hue;
    colorHSV.saturation = hsv.saturation;
    colorHSV.value = hsv.value;

    // HSV to RGB calculation
    float s = hsv.saturation / 100.0f;
    float v = hsv.value / 100.0f;
    float C = s * v;
    float X = C * (1 - fabs((fmod(hsv.hue / 60.0f, 2) - 1)));
    float m = v - C;
    float r, g, b;
    if(hsv.hue < 60)
    {
        r = C;
        g = X;
        b = 0;
    }
    else if(hsv.hue >= 60 && hsv.hue < 120)
    {
        r = X;
        g = C;
        b = 0;
    }
    else if(hsv.hue >= 120 && hsv.hue < 180)
    {
        r = 0;
        g = C;
        b = X;
    }
    else if(hsv.hue >= 180 && hsv.hue < 240)
    {
        r = 0;
        g = X;
        b = C;
    }
    else if(hsv.hue >= 240 && hsv.hue < 300)
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

    SetDiodeColor(diode, colorRGB, colorHSV, true);
    diode->diodeColor.lastColor = HSV;
}

Ws2812b_RGB_t GetDiodeColorRGB(const Ws2812b_Diode_t* diode)
{
    return diode->diodeColor.rgb;
}

Ws2812b_HSV_t GetDiodeColorHSV(const Ws2812b_Diode_t* diode)
{
    Ws2812b_HSV_t retColor;
    if (diode->diodeColor.lastColor == HSV)
    {
        return diode->diodeColor.hsv;
    }

    Ws2812b_RGB_t rgb = GetDiodeColorRGB(diode);;
    uint8_t rgbMin, rgbMax;

    rgbMin = rgb.red < rgb.green ? (rgb.red < rgb.blue ? rgb.red : rgb.blue) : (rgb.green < rgb.blue ? rgb.green : rgb.blue);
    rgbMax = rgb.red > rgb.green ? (rgb.red > rgb.blue ? rgb.red : rgb.blue) : (rgb.green > rgb.blue ? rgb.green : rgb.blue);
    
    retColor.value = round(100 * ((float)rgbMax / 255));
    if (retColor.value == 0)
    {
        retColor.hue = 0;
        retColor.saturation = 0;
        return retColor;
    }

    retColor.saturation = round(100 * (float)(rgbMax - rgbMin) / rgbMax);
    if (retColor.saturation == 0)
    {
        retColor.hue = 0;
        return retColor;
    }

    float hueF;
    if (rgbMax == rgb.red)
    {
        hueF = ((float)rgb.green - rgb.blue) / (rgbMax - rgbMin);
    }
    else if (rgbMax == rgb.green)
    {
        hueF = 2.0f + ((float)rgb.blue - rgb.red) / (rgbMax - rgbMin);
    }
    else
    {
        hueF = 4.0f + ((float)rgb.red - rgb.green) / (rgbMax - rgbMin);
    }

    if (hueF < 0)
    {
        retColor.hue = (uint16_t)(round((hueF * 60)) + 360);    
    }
    else
    {
        retColor.hue = (uint16_t)round((hueF * 60)); 
    }
    return retColor;
}

void SetActiveSectors(Ws2812b_Driver_t* this, unsigned int val)
{
    this->activeSectors = val;
}

unsigned int GetActiveSectors(Ws2812b_Driver_t* this)
{
    return this->activeSectors;
}


SpiWs2812B_t* GetDeviceBuffer(Ws2812b_Driver_t* this)
{
    return this->deviceBuffer;
}

void SetDiodeToDeviceBuffer(Ws2812b_Driver_t* this, const Ws2812b_Diode_t* diode)
{
    Ws2812b_RGB_t rgb = GetDiodeColorRGB(diode);

    SetColorToDeviceBuffer(this, diode, GREEN, rgb.green);
    SetColorToDeviceBuffer(this, diode, RED, rgb.red);
    SetColorToDeviceBuffer(this, diode, BLUE, rgb.blue);
}

void SendDeviceBuffer(Ws2812b_Driver_t* this)
{
    // now only send diodes of active sector
    for (uint8_t i = 0; i < MAX_SECTORS; i++)
    {
        if (this->sectors[i].isUsed)
        {
            Ws2812b_Diode_t* iter;
            for (iter = this->sectors[i].firstDiode; iter != this->sectors[i].lastDiode; iter = iter->next)
            {
                SetDiodeToDeviceBuffer(this, iter);
            }
            SetDiodeToDeviceBuffer(this, iter);
        }
    }

    SendBufferOverSpi(getSpiBufferPointer(this->deviceBuffer), WS2812B_DIODES * 24 * 3 + 144);
}

void SendPartOfDeviceBuffer(Ws2812b_Driver_t* this, uint32_t diodes)
{
    if (diodes > WS2812B_DIODES)
    {
        diodes = WS2812B_DIODES;
    }
    // for (uint32_t i = 0; i < diodes; i++)
    // {
    //     SetDiodeToDeviceBuffer(this, i);
    // }

    SendBufferOverSpi(getSpiBufferPointer(this->deviceBuffer), diodes * 24 * 3 + 144);
}

Ws2812b_Sector_t* GetSectors(Ws2812b_Driver_t* this)
{
    return this->sectors;
}


Ws2812b_Diode_t* GetDiodesArray(Ws2812b_Driver_t* this)
{
    return this->diodes;
}
