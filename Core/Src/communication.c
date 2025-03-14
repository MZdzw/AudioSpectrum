#include "communication.h"
#include "string.h"
#include "usbInterface.h"

#ifndef TESTING
typedef struct USB_t
{
    uint8_t bufferTX[256];
    uint8_t bufferRX[256];
    uint8_t flagUSB;
    uint32_t msgLen;
    USBDecodedData_t decodedData;
} USB_t;
#endif

static uint8_t lookUpTableUSBLength[USB_PROPER_ACTIONS] =
  {14, 18, 19, 18, 18, 17, 14, 17, 18, 14, 14, 14, 13};

USB_t obj =
{
    .bufferRX = {0},
    .bufferTX = {0},
    .flagUSB = 0,
    .msgLen = 0,
    .decodedData.action = USB_ERROR,
    .decodedData.diodeID = 0,
    .decodedData.hsvColor = {0, 0, 0},
    .decodedData.diodesRange = {0, 0}
};

USB_t* USB_InitObject(void)
{
    return &obj;
}

uint8_t* GetTxBufferUSB(USB_t* this)
{
    return this->bufferTX;
}

uint8_t* GetRxBufferUSB(USB_t* this)
{
    return this->bufferRX;
}

uint8_t* GetFlagUSBPtr(USB_t* this)
{
    return &(this->flagUSB);
}

uint32_t GetMsgLen(USB_t* this)
{
    return this->msgLen;
}

USBDecodedData_t* GetUSBDecodedData(USB_t* this)
{
    return &this->decodedData;
}

void SendMsgUSB(USB_t* this, const uint16_t length)
{
    this->msgLen = length;
    SendBufferOverUsb(this->bufferTX, this->msgLen);
}

bool CheckReceiveUSB(USB_t* this)
{
    if (this->flagUSB)
    {
        this->flagUSB = 0;
        return true;
    }
    return false;
}

USBDecodedData_t DecodeMsg(USB_t* this)
{
    USBDecodedData_t msg = {.action = USB_ERROR};
    for (uint8_t i = 0; i < 6; i++)
        if (this->bufferRX[i] != 0xAA)
        {
            msg.action = USB_BAD_PREFIX;
            return msg;
        }

    if (this->bufferRX[6] >= USB_BAD_PREFIX)
    {
        if (this->bufferRX[6] == 0xAA)
        {
            msg.action = USB_BAD_PREFIX;
            return msg;
        }
        msg.action = this->bufferRX[6];
        return msg;
    }
    
    for (uint8_t i = lookUpTableUSBLength[this->bufferRX[6]] - 1; i > lookUpTableUSBLength[this->bufferRX[6]] - 7; i--)
        if (this->bufferRX[i] != 0xAA)
        {
            msg.action = USB_BAD_APPENDIX;
            return msg;
        }

    msg.action = this->bufferRX[6];

    switch (msg.action)
    {
        case USB_REMOVE_SECTOR:
            msg.sectorID = this->bufferRX[7];
        break;
        case USB_ADD_SECTOR:
            msg.sectorID = this->bufferRX[7];
            msg.diodesRange.startDiode = (this->bufferRX[8] << 8 & 0x0000FF00) | (this->bufferRX[9] & 0x000000FF);
            msg.diodesRange.endDiode = (this->bufferRX[10] << 8 & 0x0000FF00)  | (this->bufferRX[11] & 0x000000FF);
        break;
        case USB_SET_DIODE_COLOR_HSV:
            msg.diodeID = (this->bufferRX[7] << 8 & 0x0000FF00) | (this->bufferRX[8] & 0x000000FF);
            msg.hsvColor.hue = (this->bufferRX[9] << 8 & 0xFF00) | (this->bufferRX[10] & 0x00FF);
            msg.hsvColor.saturation = this->bufferRX[11];
            msg.hsvColor.value = this->bufferRX[12];
        break;
        case USB_SET_DIODE_COLOR_RGB:
            msg.diodeID = (this->bufferRX[7] << 8 & 0x0000FF00) | (this->bufferRX[8] & 0x000000FF);
            msg.rgbColor.red = this->bufferRX[9];
            msg.rgbColor.green = this->bufferRX[10];
            msg.rgbColor.blue = this->bufferRX[11];
        break;
        case USB_SET_SECTOR_COLOR_RAINBOW:
            msg.sectorID = this->bufferRX[7];
        break;
        case USB_SET_SECTOR_SPAWN_DIODE_COLOR:
            msg.sectorID = this->bufferRX[7];
            msg.rgbColor.red = this->bufferRX[8];
            msg.rgbColor.green = this->bufferRX[9];
            msg.rgbColor.blue = this->bufferRX[10];
        break;
        case USB_SET_SECTOR_ANIMATION_SPEED:
            msg.sectorID = this->bufferRX[7];
            msg.animationSpeed = (this->bufferRX[8] << 24 & 0xFF000000) | (this->bufferRX[9] << 16 & 0x00FF0000) | 
                                 (this->bufferRX[10] << 8 & 0x0000FF00) | (this->bufferRX[11] & 0x000000FF);
        break;
        case USB_SET_ROLLING_EFFECT:
            msg.sectorID = this->bufferRX[7];
            msg.animation = USB_ROLLING;
        break;
        case USB_SET_DIMMING_ENTIRE_EFFECT:
            msg.sectorID = this->bufferRX[7];
            msg.animation = USB_DIMMING_ENTIRE;
        break;
        case USB_SET_NO_ANIMATION_EFFECT:
            msg.sectorID = this->bufferRX[7];
            msg.animation = USB_NO_ANIMATION;
        break;
        default:
            // not known coomand, set to default
            msg.diodeID = 0;
            msg.hsvColor.hue = 0;
            msg.hsvColor.saturation = 0;
            msg.hsvColor.value = 0;
            msg.diodesRange.startDiode = 0;
            msg.diodesRange.endDiode = 0; 
        break;
    }

    return msg;
}