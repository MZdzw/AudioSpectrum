#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include <stdbool.h>
#include <stdint.h>

#define UINT8_t_MAX 255

typedef enum USBAction_e
{
    USB_REMOVE_SECTOR,                    // 14 length
    USB_ADD_SECTOR,                       // 18 length
    USB_SET_DIODE_COLOR_HSV,              // 19 length
    USB_SET_DIODE_COLOR_RGB,              // 18 length
    USB_SET_SECTOR_COLOR_HSV,             // 18 length
    USB_SET_SECTOR_COLOR_RGB,             // 17 length
    USB_SET_SECTOR_COLOR_RAINBOW,         // 14 length
    USB_SET_SECTOR_SPAWN_DIODE_COLOR,     // 17 length
    USB_SET_SECTOR_ANIMATION_SPEED,       // 18 length
    USB_SET_ROLLING_EFFECT,               // 14 length
    USB_SET_DIMMING_ENTIRE_EFFECT,        // 14 length
    USB_SET_NO_ANIMATION_EFFECT,          // 14 length
    USB_LED_STRIP_STATE_REQ,              // 13 length
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
    uint16_t startDiode;
    uint16_t endDiode;
} USBDiodes_t;

typedef struct USBDecodedData_t
{
    USBAction_e action;
    union 
    {
        uint8_t sectorID;
        uint16_t diodeID;
    };
    union 
    {
        USBRGB_t rgbColor;
        USBHSV_t hsvColor;
        uint32_t animationSpeed;
        USBAnimation_e animation;
    };
    USBDiodes_t diodesRange;
} USBDecodedData_t;

#ifndef TESTING
typedef struct USB_t USB_t;
#else
typedef struct USB_t
{
    uint8_t bufferTX[256];
    uint8_t bufferRX[256];
    uint8_t flagUSB;
    uint32_t msgLen;
    USBDecodedData_t decodedData;
} USB_t;
#endif

USB_t* USB_InitObject(void);

uint8_t* GetTxBufferUSB(USB_t* this);
uint8_t* GetRxBufferUSB(USB_t* this);
uint8_t* GetFlagUSBPtr(USB_t* this);
uint32_t GetMsgLen(USB_t* this);
USBDecodedData_t* GetUSBDecodedData(USB_t* this);
void SendMsgUSB(USB_t* this, const uint16_t length);
bool CheckReceiveUSB(USB_t* this);
USBDecodedData_t DecodeMsg(USB_t* this);

#endif
