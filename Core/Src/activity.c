#include "activity.h"
#include "string.h"

static bool CheckInsideUsedSector(Ws2812b_Driver_t* driver, uint32_t diodeNr)
{
    Ws2812b_Sector_t* sectors = GetSectors(driver);
    Ws2812b_Diode_t* diodes = GetDiodesArray(driver);

    for (uint8_t i = 0; i < MAX_SECTORS; i++)
    {
        if (sectors[i].isUsed)
        {
            if (sectors[i].firstDiode > sectors[i].lastDiode)
            {
                if ((diodes + diodeNr) >= sectors[i].firstDiode ||
                    (diodes + diodeNr) <= sectors[i].lastDiode)
                    return true;
            }
            else
            {
                if ((diodes + diodeNr) >= sectors[i].firstDiode &&
                    (diodes + diodeNr) <= sectors[i].lastDiode)
                    return true;
            }
        }
    }
    return false;
}

static Activity act;

static Activity_e ActivityRemoveSector(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivityAddSector(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetDiodeHSV(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetDiodeRGB(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetSectorColorHSV(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetSectorColorRGB(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetSectorColorRainbow(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetSectorSpawnDiode(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetSectorAnimationSpeed(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetAnimationRolling(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetAnimationDimmingEntire(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivitySetAnimationNoAnimation(LedStrip_t* leds, USB_t* usb);
static Activity_e ActivityRespondOnLedStripRequest(LedStrip_t* leds, USB_t* usb);

static Activity lookUpTableActivities[13] =
{
    &ActivityRemoveSector, &ActivityAddSector, &ActivitySetDiodeHSV, &ActivitySetDiodeRGB,
    &ActivitySetSectorColorHSV, &ActivitySetSectorColorRGB, &ActivitySetSectorColorRainbow,
    &ActivitySetSectorSpawnDiode, &ActivitySetSectorAnimationSpeed,
    &ActivitySetAnimationRolling, &ActivitySetAnimationDimmingEntire,
    &ActivitySetAnimationNoAnimation, &ActivityRespondOnLedStripRequest
};


static Activity_e ActivityRemoveSector(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[decodedUsbMsg->sectorID].isUsed))
        return SECTOR_ID_NON_EXISITING;

    RemoveSector(driver, decodedUsbMsg->sectorID);
    
    return OK;
}

static Activity_e ActivityAddSector(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (GetSectors(driver)[decodedUsbMsg->sectorID].isUsed)
        return SECTOR_ID_ALREADY_EXIST;
    if (decodedUsbMsg->diodesRange.startDiode >= WS2812B_DIODES ||
          decodedUsbMsg->diodesRange.endDiode >= WS2812B_DIODES)
        return DIODE_ID_OUT_OF_RANGE;
    
    if (!(SetSector(driver, decodedUsbMsg->sectorID,
          decodedUsbMsg->diodesRange.startDiode,
          decodedUsbMsg->diodesRange.endDiode)))
        return SECTORS_OVERLAPPING;
        
    return OK;
}

static Activity_e ActivitySetDiodeHSV(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->diodeID >= WS2812B_DIODES)
        return DIODE_ID_OUT_OF_RANGE;
    if (decodedUsbMsg->hsvColor.hue > 360 ||
        decodedUsbMsg->hsvColor.saturation > 100 ||
        decodedUsbMsg->hsvColor.value > 100)
        return COLOR_OUT_OF_RANGE;
    if (!CheckInsideUsedSector(driver, decodedUsbMsg->diodeID))
        return DIODE_ID_NOT_IN_USED_SECTOR;

    SetDiodeColorHSV(GetDiodesArray(driver) + decodedUsbMsg->diodeID, 
    (Ws2812b_HSV_t){decodedUsbMsg->hsvColor.hue,
                    decodedUsbMsg->hsvColor.saturation,
                    decodedUsbMsg->hsvColor.value});

    return OK;
}

static Activity_e ActivitySetDiodeRGB(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->diodeID >= WS2812B_DIODES)
        return DIODE_ID_OUT_OF_RANGE;
    if (!CheckInsideUsedSector(driver, decodedUsbMsg->diodeID))
        return DIODE_ID_NOT_IN_USED_SECTOR;

    SetDiodeColorRGB(GetDiodesArray(driver) + decodedUsbMsg->diodeID,
    (Ws2812b_RGB_t){decodedUsbMsg->rgbColor.red,
                    decodedUsbMsg->rgbColor.blue,
                    decodedUsbMsg->rgbColor.blue});

    return OK;
}

static Activity_e ActivitySetSectorColorHSV(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[decodedUsbMsg->sectorID].isUsed))
        return SECTOR_ID_NOT_USED;
    if (decodedUsbMsg->hsvColor.hue > 360 ||
        decodedUsbMsg->hsvColor.saturation > 100 ||
        decodedUsbMsg->hsvColor.value > 100)
        return COLOR_OUT_OF_RANGE;

    SetHSVColorForSector(driver,
                         decodedUsbMsg->sectorID,
                         decodedUsbMsg->hsvColor.hue,
                         decodedUsbMsg->hsvColor.saturation,
                         decodedUsbMsg->hsvColor.value);

    return OK;
}

static Activity_e ActivitySetSectorColorRGB(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[decodedUsbMsg->sectorID].isUsed))
        return SECTOR_ID_NOT_USED;

    SetRGBColorForSector(driver,
                         decodedUsbMsg->sectorID,
                         decodedUsbMsg->rgbColor.red,
                         decodedUsbMsg->rgbColor.green,
                         decodedUsbMsg->rgbColor.blue);

    return OK;
}

static Activity_e ActivitySetSectorColorRainbow(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!(GetSectors(driver)[decodedUsbMsg->sectorID].isUsed))
        return SECTOR_ID_NOT_USED;

    SetRainbowForSector(driver, decodedUsbMsg->sectorID);

    return OK;
}

static Activity_e ActivitySetSectorSpawnDiode(LedStrip_t* leds, USB_t* usb)
{
    Ws2812b_Driver_t* driver = GetDriver(leds);
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    Ws2812b_Sector_t* sectors = GetSectors(driver); 
    if (!sectors[decodedUsbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;

    
    SetDiodeColorRGB(sectors[decodedUsbMsg->sectorID].firstDiode,
    (Ws2812b_RGB_t){decodedUsbMsg->rgbColor.red, decodedUsbMsg->rgbColor.green,
                    decodedUsbMsg->rgbColor.blue});

    return OK;
}

static Activity_e ActivitySetSectorAnimationSpeed(LedStrip_t* leds, USB_t* usb)
{
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[decodedUsbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;

    SetAnimationSpeed(GetAnimations(leds), decodedUsbMsg->sectorID,
                      decodedUsbMsg->animationSpeed);

    return OK;
}

static Activity_e ActivitySetAnimationRolling(LedStrip_t* leds, USB_t* usb)
{
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[decodedUsbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;
    
    SetAnimation(leds, ROLLING, decodedUsbMsg->sectorID);

    return OK;
}

static Activity_e ActivitySetAnimationDimmingEntire(LedStrip_t* leds, USB_t* usb)
{
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[decodedUsbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;
    
    SetAnimation(leds, DIMMING, decodedUsbMsg->sectorID);

    return OK;
}

static Activity_e ActivitySetAnimationNoAnimation(LedStrip_t* leds, USB_t* usb)
{
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->sectorID >= MAX_SECTORS)
        return SECTOR_ID_OUT_OF_RANGE;
    if (!GetSectors(GetDriver(leds))[decodedUsbMsg->sectorID].isUsed)
        return SECTOR_ID_NOT_USED;
    
    SetAnimation(leds, NO_ANIMATION, decodedUsbMsg->sectorID);

    return OK;
}

static Activity_e ActivityRespondOnLedStripRequest(LedStrip_t* leds, USB_t* usb)
{
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    uint8_t respond[WS2812B_DIODES * 4 + 12] = {0};
    for (uint8_t i = 0; i < 6; i++)
        respond[i] = 0xAA;
    Ws2812b_Diode_t* diodesArray = GetDiodesArray(GetDriver(leds));
    for (uint16_t i = 0; i < WS2812B_DIODES; i++)
    {
        respond[i * 4 + 6] = (diodesArray[i].diodeColor.hsv.hue >> 8) & 0xFF;
        respond[i * 4 + 7] = (diodesArray[i].diodeColor.hsv.hue) & 0xFF;
        respond[i * 4 + 8] = diodesArray[i].diodeColor.hsv.saturation;
        respond[i * 4 + 9] = diodesArray[i].diodeColor.hsv.value;
    }
    for (uint8_t i = 0; i < 6; i++)
        respond[WS2812B_DIODES * 4 + i + 6] = 0xAA;

    memcpy(GetTxBufferUSB(usb), respond, WS2812B_DIODES * 4 + 12);
    SendMsgUSB(usb, WS2812B_DIODES * 4 + 12);

    return OK;
}

Activity_e ActivateAction(LedStrip_t* leds, USB_t* usb)
{
    const USBDecodedData_t* const decodedUsbMsg = GetUSBDecodedData(usb);
    if (decodedUsbMsg->action >= USB_PROPER_ACTIONS)
    {
        return ERRORS;
    }   
    act = lookUpTableActivities[decodedUsbMsg->action];

    return act(leds, usb);
}
