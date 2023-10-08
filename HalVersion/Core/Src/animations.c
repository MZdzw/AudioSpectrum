#include <stddef.h>
#include "animations.h"

#ifndef TESTING
typedef struct Animation_t
{
    uint32_t sectorId;
    uint32_t speed;
    Animation animation_p;
} Animation_t;

typedef struct LedStrip_t
{
    Animation_t animation[MAX_SECTORS];
    Ws2812b_Driver_t* ledStripDriver;   
} LedStrip_t;
#endif


static LedStrip_t obj = {{{0, 0, NULL}}, NULL};

static DimmingDirection_e dimDir[MAX_SECTORS] = {ASCENDING};


static void NoAnimation(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    (void)driver;
    (void)sectorId;
}

static void Dimming(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    // get current color val
    Ws2812b_HSV_t hsv;
    hsv = GetDiodeColorHSV(sectors[sectorId].firstDiode);
    if (hsv.value == 0)
        dimDir[sectorId] = ASCENDING;
    if (hsv.value == 100)
        dimDir[sectorId] = DESCENDING;
    hsv.value += dimDir[sectorId];
    Ws2812b_Diode_t* iter;
    for (iter = sectors[sectorId].firstDiode; iter != sectors[sectorId].lastDiode; iter = iter->next)
    {
        SetDiodeColorHSV(iter, hsv);
    }
    SetDiodeColorHSV(iter, hsv);
}

static void Rolling(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    Ws2812b_RGB_t rgb, tmpRgb;
    
    // for rolling purpose RGB functions will be faster
    // because they don't need to calculate anything
    
    rgb = GetDiodeColorRGB(sectors[sectorId].firstDiode);

    Ws2812b_Diode_t* iter;
    for (iter = sectors[sectorId].firstDiode; iter != sectors[sectorId].lastDiode; iter = iter->next)
    {
        tmpRgb = GetDiodeColorRGB(iter->next);
        SetDiodeColorRGB(iter->next, rgb);
        rgb = tmpRgb;
    }
    SetDiodeColorRGB(iter->next, rgb);
}

static void RollingNoWrapping(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    Ws2812b_RGB_t rgb, tmpRgb;
    
    // for rolling purpose RGB functions will be faster
    // because they don't need to calculate anything
    
    rgb = GetDiodeColorRGB(sectors[sectorId].firstDiode);

    Ws2812b_Diode_t* iter;
    for (iter = sectors[sectorId].firstDiode; iter != sectors[sectorId].lastDiode; iter = iter->next)
    {
        tmpRgb = GetDiodeColorRGB(iter->next);
        SetDiodeColorRGB(iter->next, rgb);
        rgb = tmpRgb;
    }
    SetDiodeColorRGB(sectors[sectorId].firstDiode, (Ws2812b_RGB_t){0, 0, 0});
}

LedStrip_t* LedStrip_initObject(void)
{
    obj.ledStripDriver = Ws2812b_initObject();
    obj.animation[0].sectorId = 0;
    obj.animation[0].speed = 20;
    obj.animation[0].animation_p = Dimming;

    return &obj;
}

Ws2812b_Driver_t* GetDriver(LedStrip_t* this)
{
    return this->ledStripDriver;
}

Animation_t* GetAnimations(LedStrip_t* this)
{
    return this->animation;
}

Animation GetAnimationFunPtr(Animation_t* this, uint32_t id)
{
    return this[id].animation_p;
}

uint32_t GetAnimationSpeed(Animation_t* this, uint32_t id)
{
    return this[id].speed;
}

void SetAnimationSpeed(Animation_t* this, uint32_t id, uint32_t speed)
{
    this[id].speed = speed;
}

void SetAnimationFunPtr(Animation_t* this, uint32_t id)
{
    this[id].animation_p = Dimming;
}

void SetHSVColorForSector(Ws2812b_Driver_t* this, uint32_t id, uint16_t hue, uint8_t saturation, uint8_t value)
{
    Ws2812b_Sector_t sector = GetSectors(this)[id];

    Ws2812b_Diode_t* iter;
    for (iter = sector.firstDiode; iter != sector.lastDiode; iter = iter->next)
    {
        SetDiodeColorHSV(iter, (Ws2812b_HSV_t){hue, saturation, value});
    }
}

void SetRGBColorForSector(Ws2812b_Driver_t* this, uint32_t id, uint8_t red, uint8_t green, uint8_t blue)
{
    Ws2812b_Sector_t sector = GetSectors(this)[id];

    Ws2812b_Diode_t* iter;
    for (iter = sector.firstDiode; iter != sector.lastDiode; iter = iter->next)
    {
        SetDiodeColorRGB(iter, (Ws2812b_RGB_t){red, green, blue});
    }
    SetDiodeColorRGB(iter, (Ws2812b_RGB_t){red, green, blue});
}

void SetRainbowForSector(Ws2812b_Driver_t* this, uint32_t sectorID)
{
    Ws2812b_Sector_t sector = GetSectors(this)[sectorID];
    uint32_t diodesNum = 1;
    for (Ws2812b_Diode_t* iter = sector.firstDiode; iter != sector.lastDiode; iter = iter->next)
    {
        diodesNum++;
    }
    uint16_t step = 360 / diodesNum;
    uint32_t cnt = 0;

    Ws2812b_Diode_t* iter;
    for (iter = sector.firstDiode; iter != sector.lastDiode; iter = iter->next)
    {
        SetDiodeColorHSV(iter, (Ws2812b_HSV_t){step * cnt, 100, 100});
        cnt++;
    }
    SetDiodeColorHSV(iter, (Ws2812b_HSV_t){step * cnt, 100, 100});
}

void SetAnimation(LedStrip_t* this, Animation_e animationType, uint32_t id)
{
    Animation_t* animationHolder = GetAnimations(this);
    if (animationType == DIMMING)
    {
        animationHolder[id].animation_p = Dimming;
    }
    else if (animationType == ROLLING)
    {
        animationHolder[id].animation_p = Rolling;
    }
    else if (animationType == ROLLING_NO_WRAPPING)
    {
        animationHolder[id].animation_p = RollingNoWrapping;
    }
    else if (animationType == NO_ANIMATION)
    {
        animationHolder[id].animation_p = NoAnimation;
    }
}

#ifdef TESTING
void SetDimDir(uint32_t sectorID, Animation_e dir)
{
    dimDir[sectorID] = dir ? ASCENDING : DESCENDING;
}
#endif