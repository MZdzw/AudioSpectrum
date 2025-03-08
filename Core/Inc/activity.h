#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__
#include "communication.h"
#include "ws2812bDriver.h"
#include "animations.h"

#define UINT32_T_MAX 4294967295

typedef enum Activity_e
{
    OK,
    ERRORS,
    SECTOR_ID_NOT_USED,
    SECTOR_ID_ALREADY_EXIST,
    SECTOR_ID_NON_EXISITING,
    SECTOR_ID_OUT_OF_RANGE,
    SECTORS_OVERLAPPING,
    DIODE_ID_NOT_IN_USED_SECTOR,
    DIODE_ID_OUT_OF_RANGE,
    COLOR_OUT_OF_RANGE,
} Activity_e;

typedef Activity_e (*Activity)(LedStrip_t* leds, USBMsg_t* usbMsg);

Activity_e ActivateAction(LedStrip_t* leds, USBMsg_t* usbMsg);


#endif
