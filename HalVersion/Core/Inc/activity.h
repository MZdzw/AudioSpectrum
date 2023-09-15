#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__
#include "communication.h"
#include "ws2812bDriver.h"


typedef enum Activity_e
{
    OK,
    ERRORS,
    SECTOR_ID_EMPTY,
    SECTOR_ID_FULL,
    SECTOR_ID_ALREADY_EXIST,
    DIODE_ID_OUT_OF_RANGE,
} Activity_e;

typedef Activity_e (*Activity)(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);

Activity act;

void ActivateAction(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);


#endif
