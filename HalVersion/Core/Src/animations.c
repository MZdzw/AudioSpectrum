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

static void Dimming(Ws2812b_Driver_t* driver, uint32_t sectorId)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    // get current color val
    uint8_t red, green, blue;
    GetDiodeColorRGB(driver, sectors[sectorId].startDiode, &red, &green, &blue);
    if (red == 0)
        dimDir[sectorId] = ASCENDING;
    if (red == 255)
        dimDir[sectorId] = DESCENDING;
    if (dimDir[sectorId] == ASCENDING)
    {
        for (uint32_t i = sectors[sectorId].startDiode; i <= sectors[sectorId].endDiode; i++)
        {
            SetDiodeColorRGB(driver, i, red + 1, green + 1, blue + 1);
        }
    }
    else
    {
        for (uint32_t i = sectors[sectorId].startDiode; i <= sectors[sectorId].endDiode; i++)
        {
            SetDiodeColorRGB(driver, i, red - 1, green - 1, blue - 1);
        }
    }
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