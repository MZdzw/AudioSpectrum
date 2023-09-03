#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__
#include "ws2812bDriver.h"

typedef enum Animation_e
{
    DIMMING,
    ROLLING
} Animation_e;

typedef enum DimmingDirection_e
{
    DESCENDING,
    ASCENDING
} DimmingDirection_e;

typedef void(*Animation)(Ws2812b_Driver_t*, uint32_t);
typedef struct Animation_t Animation_t;
typedef struct LedStrip_t LedStrip_t;

LedStrip_t* LedStrip_initObject(void);
Ws2812b_Driver_t* GetDriver(LedStrip_t* this);
Animation_t* GetAnimations(LedStrip_t* this);
Animation GetAnimationFunPtr(Animation_t* this, uint32_t id);
uint32_t GetAnimationSpeed(Animation_t* this, uint32_t id);
void SetAnimationSpeed(Animation_t* this, uint32_t id, uint32_t speed);
void SetAnimationFunPtr(Animation_t* this, uint32_t id);
void SetHSVColorForSector(Ws2812b_Driver_t* this, uint32_t id, uint16_t hue, uint8_t saturation, uint8_t value);
void SetRainbowForSector(Ws2812b_Driver_t* this, uint32_t sectorID);
void SetAnimation(LedStrip_t* this, Animation_e animationType, uint32_t id);

#ifdef TESTING
void SetDimDir(uint32_t sectorID, Animation_e dir);
#endif

#endif
