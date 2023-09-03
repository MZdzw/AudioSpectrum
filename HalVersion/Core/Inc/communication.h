#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include "usbd_cdc_if.h"

typedef struct Communication_t Communication_t;

Communication_t* Communication_InitObject(void);

uint8_t* GetRxBufferUSB(Communication_t* this);
uint8_t* GetFlagUSBPtr(Communication_t* this);
void SendMsgUSB(Communication_t* this, const char* msg);
void CheckReceiveUSB(Communication_t* this);

#endif
