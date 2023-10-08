#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__
#include "ws2812bDriver.h"

typedef enum Animation_e
{
    NO_ANIMATION,
    DIMMING,
    ROLLING,
    ROLLING_NO_WRAPPING
} Animation_e;

// typedef enum DimmingDirection_e
// {
//     DESCENDING,
//     ASCENDING
// } DimmingDirection_e;

typedef void(*Animation)(Ws2812b_Driver_t*, uint32_t);

#ifndef TESTING
typedef struct Animation_t Animation_t;
typedef struct LedStrip_t LedStrip_t;
#else
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

LedStrip_t* LedStrip_initObject(void);
Ws2812b_Driver_t* GetDriver(LedStrip_t* this);
Animation_t* GetAnimations(LedStrip_t* this);
Animation GetAnimationFunPtr(Animation_t* this, uint32_t id);
uint32_t GetAnimationSpeed(Animation_t* this, uint32_t id);
void SetAnimationSpeed(Animation_t* this, uint32_t id, uint32_t speed);
void SetAnimationFunPtr(Animation_t* this, uint32_t id);
void SetHSVColorForSector(Ws2812b_Driver_t* this, uint32_t id, uint16_t hue, uint8_t saturation, uint8_t value);
void SetRGBColorForSector(Ws2812b_Driver_t* this, uint32_t id, uint8_t red, uint8_t gren, uint8_t blue);
void SetRainbowForSector(Ws2812b_Driver_t* this, uint32_t sectorID);
void SetAnimation(LedStrip_t* this, Animation_e animationType, uint32_t id);

#ifdef TESTING
void SetDimDir(uint32_t sectorID, Animation_e dir);
#endif

#endif
