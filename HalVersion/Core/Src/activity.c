#include "activity.h"

static Activity_e ActivityRemoveSector(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivityAddSector(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetDiodeHSV(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetDiodeRGB(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorColorHSV(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorColorRGB(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorColorRainbow(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorSpawnDiode(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorAnimationSpeed(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg);

static Activity lookUpTableActivities[9] = {&ActivityRemoveSector, &ActivityAddSector, &ActivitySetDiodeHSV, &ActivitySetDiodeRGB,
                                        &ActivitySetSectorColorHSV, &ActivitySetSectorColorRGB, &ActivitySetSectorColorRainbow, 
                                        &ActivitySetSectorSpawnDiode, &ActivitySetSectorAnimationSpeed};


static Activity_e ActivityRemoveSector(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivityAddSector(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetDiodeHSV(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetDiodeRGB(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetSectorColorHSV(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetSectorColorRGB(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetSectorColorRainbow(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetSectorSpawnDiode(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

static Activity_e ActivitySetSectorAnimationSpeed(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    return 0;
}

void ActivateAction(Ws2812b_Driver_t* driver, USBMsg_t* usbMsg)
{
    if (usbMsg->action >= USB_PROPER_ACTIONS)
    {
        return;
    }   

    act = lookUpTableActivities[usbMsg->action];
    act(driver, usbMsg);
}
