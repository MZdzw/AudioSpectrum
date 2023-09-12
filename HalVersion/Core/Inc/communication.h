#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include <stdbool.h>
#ifndef TESTING
#include "usbd_cdc_if.h"
#else
#include "mock_usbd_cdc_if.h"
#endif

#define MAX_LENGTH 22

typedef enum USBAction_e
{
    USB_REMOVE_SECTOR,                    // 14 length
    USB_ADD_SECTOR,                       // 22 length
    USB_SET_DIODE_COLOR_HSV,              // 21 length    
    USB_SET_DIODE_COLOR_RGB,              // 20 length
    USB_SET_SECTOR_COLOR_HSV,             // 18 length
    USB_SET_SECTOR_COLOR_RGB,             // 17 length
    USB_SET_SECTOR_COLOR_RAINBOW,         // 14 length
    USB_SET_SECTOR_SPAWN_DIODE_COLOR,     // 17 length
    USB_SET_SECTOR_ANIMATION_SPEED,       // 18 length
    // Put new mesages here

    USB_PROPER_ACTIONS,
    //errors at the end
    USB_BAD_PREFIX,
    USB_BAD_APPENDIX,
} USBAction_e;

typedef struct Communication_t Communication_t;

Communication_t* Communication_InitObject(void);

uint8_t* GetTxBufferUSB(Communication_t* this);
uint8_t* GetRxBufferUSB(Communication_t* this);
uint8_t* GetFlagUSBPtr(Communication_t* this);
uint32_t GetMsgLen(Communication_t* this);
void SendMsgUSB(Communication_t* this, const char* msg);
bool CheckReceiveUSB(Communication_t* this);
USBAction_e DecodeMsg(Communication_t* this);

#endif
