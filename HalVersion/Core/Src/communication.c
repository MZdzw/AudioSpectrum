#include "communication.h"
#include "string.h"

typedef struct Communication_t
{
    uint8_t bufferTX[64];
    uint8_t bufferRX[64];
    uint8_t flagUSB;
    uint32_t msgLen;

} Communication_t;

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

USBAction_e DecodeMsg(Communication_t* this)
{
    for (uint8_t i = 0; i < 6; i++)
        if (this->bufferRX[i] != 0xAA)
            return USB_BAD_PREFIX;

    if (this->bufferRX[6] >= USB_BAD_PREFIX)
        return this->bufferRX[6];

    for (uint8_t i = lookUpTableUSBLength[this->bufferRX[6]] - 1; i > lookUpTableUSBLength[this->bufferRX[6]] - 7; i--)
        if (this->bufferRX[i] != 0xAA)
            return USB_BAD_APPENDIX;
    
    return this->bufferRX[6];
}