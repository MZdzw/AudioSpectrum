#include <stddef.h>
#include "animations.h"
#include "fft.h"

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

static int indexColor[7] = {0, 310, 58, 230, 110, 160, 20};

static void SpawnDiodeSound(Ws2812b_Diode_t* diode)
{
    FFT_Sound_Properties_t tmp = CheckSound();
    if (tmp.sound == NORMAL_SOUND)
    {
        SetDiodeColorHSV(diode, 
        (Ws2812b_HSV_t){.hue = indexColor[tmp.index], .saturation = 100, .value = 100});
    }
    else if (tmp.sound == LOW_SOUND)
    {
        SetDiodeColorHSV(diode, 
        (Ws2812b_HSV_t){.hue = indexColor[tmp.index], .saturation = 100, .value = 100});
    }
    else
    {
        SetDiodeColorHSV(diode, 
        (Ws2812b_HSV_t){.hue = indexColor[0], .saturation = 0, .value = 0});
    }
}

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

static void RollingSound(Ws2812b_Driver_t* driver, uint32_t sectorId)
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

    SpawnDiodeSound(sectors[sectorId].firstDiode);
}

static void RollingSoundReversed(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    Ws2812b_RGB_t rgb;
    
    // for rolling purpose RGB functions will be faster
    // because they don't need to calculate anything
    
    Ws2812b_Diode_t* iter;
    for (iter = sectors[sectorId].firstDiode; iter != sectors[sectorId].lastDiode; iter = iter->next)
    {
        rgb = GetDiodeColorRGB(iter->next);
        SetDiodeColorRGB(iter, rgb);
    }

    SpawnDiodeSound(sectors[sectorId].lastDiode);
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
    // switch statement for that case should be faster
    switch (animationType)
    {
        case DIMMING:
            animationHolder[id].animation_p = Dimming;
        break;
        case ROLLING:
            animationHolder[id].animation_p = Rolling;
        break;
        case ROLLING_NO_WRAPPING:
            animationHolder[id].animation_p = RollingNoWrapping;
        break;
        case ROLLING_SOUND:
            animationHolder[id].animation_p = RollingSound;   
        break;
        case ROLLING_SOUND_REVERSED:
            animationHolder[id].animation_p = RollingSoundReversed;
        break;
        case NO_ANIMATION:
            animationHolder[id].animation_p = NoAnimation;
        break;
        default:

        break;
    }
}
