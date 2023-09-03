#include "communication.h"

// volatile uint8_t bufferUSB[64];
// volatile uint8_t flagUSB;

typedef struct Communication_t
{
    uint8_t bufferTX[64];
    uint8_t bufferRX[64];
    uint8_t flagUSB;
    uint32_t msgLen;

} Communication_t;



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

uint8_t* GetRxBufferUSB(Communication_t* this)
{
    return this->bufferRX;
}

uint8_t* GetFlagUSBPtr(Communication_t* this)
{
    return &(this->flagUSB);
}

void SendMsgUSB(Communication_t* this, const char* msg)
{
    memcpy(this->bufferTX, msg, strlen(msg));
    this->msgLen = strlen(msg);
    SendUSB(this);
}

void CheckReceiveUSB(Communication_t* this)
{
    if (this->flagUSB)
    {
        //analyze received data

        this->flagUSB = 0;
    }
}