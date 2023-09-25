#include "activity.h"

static bool CheckOverflow(uint32_t number)
{
    if (number >= (UINT32_T_MAX - 2))
        return true;
    return false;
}

static bool CheckInsideUsedSector(Ws2812b_Driver_t* driver, uint32_t diodeNr)
{
    Ws2812b_Sector_t* tmp = GetSectors(driver);
    for (uint8_t i = 0; i < MAX_SECTORS; i++)
    {
        if (tmp[i].isUsed)
        {
            if (tmp[i].startDiode > tmp[i].endDiode)
            {
                if (diodeNr >= tmp[i].startDiode || diodeNr <= tmp[i].endDiode)
                    return true;
            }
            else
            {
                if (diodeNr >= tmp[i].startDiode && diodeNr <= tmp[i].endDiode)
                    return true;
            }
        }
    }
    return false;
}

static Activity act;

static Activity_e ActivityRemoveSector(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivityAddSector(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetDiodeHSV(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetDiodeRGB(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorColorHSV(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorColorRGB(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorColorRainbow(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorSpawnDiode(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetSectorAnimationSpeed(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetAnimationRolling(LedStrip_t* leds, USBMsg_t* usbMsg);
static Activity_e ActivitySetAnimationDimming(LedStrip_t* leds, USBMsg_t* usbMsg);

static Activity lookUpTableActivities[11] = {&ActivityRemoveSector, &ActivityAddSector, &ActivitySetDiodeHSV, &ActivitySetDiodeRGB,
                                        &ActivitySetSectorColorHSV, &ActivitySetSectorColorRGB, &ActivitySetSectorColorRainbow, 
                                        &ActivitySetSectorSpawnDiode, &ActivitySetSectorAnimationSpeed,
                                        &ActivitySetAnimationRolling, &ActivitySetAnimationDimming};


static Activity_e ActivityRemoveSector(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[usbMsg->sectorID].isUsed))
        return SECTOR_ID_NON_EXISITING;

    RemoveSector(driver, usbMsg->sectorID);
    
    return OK;
}

static Activity_e ActivityAddSector(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (GetSectors(driver)[usbMsg->sectorID].isUsed)
        return SECTOR_ID_ALREADY_EXIST;
    if (usbMsg->diodesRange.startDiode >= WS2812B_DIODES || usbMsg->diodesRange.endDiode >= WS2812B_DIODES)
        return DIODE_ID_OUT_OF_RANGE;
    
    if (!(SetSector(driver, usbMsg->sectorID, usbMsg->diodesRange.startDiode, usbMsg->diodesRange.endDiode)))
        return SECTORS_OVERLAPPING;
        
    return OK;
}

static Activity_e ActivitySetDiodeHSV(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->diodeID >= WS2812B_DIODES)
        return DIODE_ID_OUT_OF_RANGE;
    if (usbMsg->hsvColor.hue > 360 || usbMsg->hsvColor.saturation > 100 || usbMsg->hsvColor.value > 100)
        return COLOR_OUT_OF_RANGE;
    if (!CheckInsideUsedSector(driver, usbMsg->diodeID))
        return DIODE_ID_NOT_IN_USED_SECTOR;

    SetDiodeColorHSV(driver, usbMsg->diodeID, usbMsg->hsvColor.hue, usbMsg->hsvColor.saturation, usbMsg->hsvColor.value);

    return OK;
}

static Activity_e ActivitySetDiodeRGB(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->diodeID >= WS2812B_DIODES)
        return DIODE_ID_OUT_OF_RANGE;
    if (!CheckInsideUsedSector(driver, usbMsg->diodeID))
        return DIODE_ID_NOT_IN_USED_SECTOR;

    SetDiodeColorRGB(driver, usbMsg->diodeID, usbMsg->rgbColor.red, usbMsg->rgbColor.green, usbMsg->rgbColor.blue);

    return OK;
}

static Activity_e ActivitySetSectorColorHSV(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[usbMsg->sectorID].isUsed))
        return SECTOR_ID_NOT_USED;
    if (usbMsg->hsvColor.hue > 360 || usbMsg->hsvColor.saturation > 100 || usbMsg->hsvColor.value > 100)
        return COLOR_OUT_OF_RANGE;

    SetHSVColorForSector(driver, usbMsg->sectorID, usbMsg->hsvColor.hue, usbMsg->hsvColor.saturation, usbMsg->hsvColor.value);

    return OK;
}

static Activity_e ActivitySetSectorColorRGB(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[usbMsg->sectorID].isUsed))
        return SECTOR_ID_NOT_USED;

    SetRGBColorForSector(driver, usbMsg->sectorID, usbMsg->rgbColor.red, usbMsg->rgbColor.green, usbMsg->rgbColor.blue);

    return OK;
}

static Activity_e ActivitySetSectorColorRainbow(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[usbMsg->sectorID].isUsed))
        return SECTOR_ID_NOT_USED;

    SetRainbowForSector(driver, usbMsg->sectorID);

    return OK;
}

static Activity_e ActivitySetSectorSpawnDiode(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    Ws2812b_Sector_t* sectors = GetSectors(driver); 
    if (!sectors[usbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;

    SetDiodeColorRGB(driver, sectors[usbMsg->sectorID].startDiode,
    usbMsg->rgbColor.red, usbMsg->rgbColor.green, usbMsg->rgbColor.blue);

    return OK;
}

static Activity_e ActivitySetSectorAnimationSpeed(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[usbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;

    SetAnimationSpeed(GetAnimations(leds), usbMsg->sectorID, usbMsg->animationSpeed);

    return OK;
}

static Activity_e ActivitySetAnimationRolling(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[usbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;
    
    SetAnimation(leds, ROLLING, usbMsg->sectorID);

    return OK;
}

static Activity_e ActivitySetAnimationDimming(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    if (usbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[usbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;
    
    SetAnimation(leds, DIMMING, usbMsg->sectorID);

    return OK;
}

Activity_e ActivateAction(LedStrip_t* leds, USBMsg_t* usbMsg)
{
    if (usbMsg->action >= USB_PROPER_ACTIONS)
    {
        return ERRORS;
    }   
    act = lookUpTableActivities[usbMsg->action];

    return act(leds, usbMsg);
}
