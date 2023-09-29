#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include <stdbool.h>
#ifndef TESTING
#include "usbd_cdc_if.h"
#else
#include "mock_usbd_cdc_if.h"
#endif

#define MAX_LENGTH 22
#define UINT8_t_MAX 255

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
    USB_SET_ROLLING_EFFECT,               // 14 length
    USB_SET_DIMMING_ENTIRE_EFFECT,        // 14 length
    USB_SET_NO_ANIMATION_EFFECT,          // 14 length
    // Put new mesages here

    USB_PROPER_ACTIONS,
    //errors at the end
    USB_BAD_PREFIX,
    USB_BAD_APPENDIX,
    USB_ERROR
} USBAction_e;

typedef enum USBAnimation_e
{
    USB_ROLLING,
    USB_DIMMING_ENTIRE,
    USB_NO_ANIMATION
} USBAnimation_e;

typedef struct USBRGB_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} USBRGB_t;

typedef struct USBHSV_t
{
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
} USBHSV_t;

typedef struct USBDiodes_t
{
    uint32_t startDiode;
    uint32_t endDiode;
} USBDiodes_t;

typedef struct USBMsg_t
{
    USBAction_e action;
    union 
    {
        uint8_t sectorID;
        uint32_t diodeID;
    };
    union 
    {
        USBRGB_t rgbColor;
        USBHSV_t hsvColor;
        uint32_t animationSpeed;
        USBAnimation_e animation;
    };
    USBDiodes_t diodesRange;
} USBMsg_t;

#ifndef TESTING
typedef struct Communication_t Communication_t;
#else
typedef struct Communication_t
{
    uint8_t bufferTX[64];
    uint8_t bufferRX[64];
    uint8_t flagUSB;
    uint32_t msgLen;
} Communication_t;
#endif

Communication_t* Communication_InitObject(void);

uint8_t* GetTxBufferUSB(Communication_t* this);
uint8_t* GetRxBufferUSB(Communication_t* this);
uint8_t* GetFlagUSBPtr(Communication_t* this);
uint32_t GetMsgLen(Communication_t* this);
void SendMsgUSB(Communication_t* this, const char* msg);
bool CheckReceiveUSB(Communication_t* this);
USBMsg_t DecodeMsg(Communication_t* this);

#endif
