#include "communication.h"
#include "string.h"

#ifndef TESTING
typedef struct Communication_t
{
    uint8_t bufferTX[64];
    uint8_t bufferRX[64];
    uint8_t flagUSB;
    uint32_t msgLen;
} Communication_t;
#endif

static uint8_t lookUpTableUSBLength[USB_PROPER_ACTIONS] = {14, 22, 21, 20, 18, 17, 14, 17, 18};

static inline void SendUSB(Communication_t* this)
{
    CDC_Transmit_FS((uint8_t*)this->bufferTX, this->msgLen);
}

Communication_t obj = {{0}, {0}, 0, 0};

Communication_t* Communication_InitObject(void)
{
    // obj.bufferRX = bufferUSB;
    // obj.bufferRXSize = 64;
    return &obj;
}

uint8_t* GetTxBufferUSB(Communication_t* this)
{
    return this->bufferTX;
}

uint8_t* GetRxBufferUSB(Communication_t* this)
{
    return this->bufferRX;
}

uint8_t* GetFlagUSBPtr(Communication_t* this)
{
    return &(this->flagUSB);
}

uint32_t GetMsgLen(Communication_t* this)
{
    return this->msgLen;
}

void SendMsgUSB(Communication_t* this, const char* msg)
{
    memcpy(this->bufferTX, msg, strlen(msg));
    this->msgLen = strlen(msg);
    SendUSB(this);
}

bool CheckReceiveUSB(Communication_t* this)
{
    if (this->flagUSB)
    {
        this->flagUSB = 0;
        return true;
    }
    return false;
}

USBMsg_t DecodeMsg(Communication_t* this)
{
    USBMsg_t msg = {.action = USB_ERROR};
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
            msg.diodesRange.startDiode = (this->bufferRX[8] << 24 & 0xFF000000) | (this->bufferRX[9] << 16 & 0x00FF0000) | 
                                         (this->bufferRX[10] << 8 & 0x0000FF00) | (this->bufferRX[11] & 0x000000FF);
            msg.diodesRange.endDiode = (this->bufferRX[12] << 24 & 0xFF000000)  | (this->bufferRX[13] << 16 & 0x00FF0000) | 
                                        (this->bufferRX[14] << 8 & 0x0000FF00)  | (this->bufferRX[15] & 0x000000FF);
        break;
        case USB_SET_DIODE_COLOR_HSV:
            msg.diodeID = (this->bufferRX[7] << 24 & 0xFF000000) | (this->bufferRX[8] << 16 & 0x00FF0000) | 
                           (this->bufferRX[9] << 8 & 0x0000FF00) | (this->bufferRX[10] & 0x000000FF);
            msg.hsvColor.hue = (this->bufferRX[11] << 8 & 0xFF00) | (this->bufferRX[12] & 0x00FF);
            msg.hsvColor.saturation = this->bufferRX[13];
            msg.hsvColor.value = this->bufferRX[14];
        break;
        case USB_SET_DIODE_COLOR_RGB:
            msg.diodeID = (this->bufferRX[7] << 24 & 0xFF000000) | (this->bufferRX[8] << 16 & 0x00FF0000) | 
                           (this->bufferRX[9] << 8 & 0x0000FF00) | (this->bufferRX[10] & 0x000000FF);
            msg.rgbColor.red = this->bufferRX[11];
            msg.rgbColor.green = this->bufferRX[12];
            msg.rgbColor.blue = this->bufferRX[13];
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