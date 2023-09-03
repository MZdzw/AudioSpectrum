#include <stddef.h>
#include "animations.h"

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


static LedStrip_t obj = {{{0, 0, NULL}}, NULL};

static DimmingDirection_e dimDir[MAX_SECTORS] = {ASCENDING};
static Ws2812b_Color_t* colors_p;

static void Dimming(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    // get current color val
    uint16_t hue;
    uint8_t saturation, value;
    GetDiodeColorHSV(driver, sectors[sectorId].startDiode, &hue, &saturation, &value);
    if (value == 0)
        dimDir[sectorId] = ASCENDING;
    if (value == 100)
        dimDir[sectorId] = DESCENDING;
    if (dimDir[sectorId] == ASCENDING)
    {
        for (uint32_t i = sectors[sectorId].startDiode; i <= sectors[sectorId].endDiode; i++)
        {
            SetDiodeColorHSV(driver, i, hue, saturation, value + 1);
        }
    }
    else
    {
        for (uint32_t i = sectors[sectorId].startDiode; i <= sectors[sectorId].endDiode; i++)
        {
            SetDiodeColorHSV(driver, i, hue, saturation, value - 1);
        }
    }
}

static void Rolling(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    uint8_t red, green, blue;
    uint8_t tmpRed, tmpGreen, tmpBlue;
    
    // for rolling purpose RGB functions will be faster
    // because they don't need to calculate anything
    GetDiodeColorRGB(driver, sectors[sectorId].endDiode, &tmpRed, &tmpGreen, &tmpBlue);
    for (uint32_t i = sectors[sectorId].endDiode; i > sectors[sectorId].startDiode; i--)
    {
        GetDiodeColorRGB(driver, i - 1, &red, &green, &blue);
        SetDiodeColorRGB(driver, i, red, green, blue);
    }
    SetDiodeColorRGB(driver, sectors[sectorId].startDiode, tmpRed, tmpGreen, tmpBlue);
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
    for (unsigned int i = sector.startDiode; i < sector.endDiode; i++)
    {
        SetDiodeColorHSV(this, i, hue, saturation, value);
    } 
}

void SetRainbowForSector(Ws2812b_Driver_t* this, uint32_t sectorID)
{
    Ws2812b_Sector_t sector = GetSectors(this)[sectorID];
    uint32_t diodesNum = sector.endDiode - sector.startDiode;
    uint16_t step = 360 / diodesNum;
    uint32_t cnt = 0;
    for (unsigned int i = sector.startDiode; i < sector.endDiode; i++)
    {
        SetDiodeColorHSV(this, i, step * cnt, 100, 100);
        cnt++;
    }
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
}

#ifdef TESTING
void SetDimDir(uint32_t sectorID, Animation_e dir)
{
    dimDir[sectorID] = dir ? ASCENDING : DESCENDING;
}
#endif