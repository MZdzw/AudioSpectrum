#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "communication.h"
#include "mock_usbd_cdc_if.h"

Communication_t* usb;

void setUp (void) /* Is run before every test, put unit init calls here. */
{
    usb = Communication_InitObject();
}
void tearDown (void) /* Is run after every test, put unit clean-up calls here. */
{
    uint8_t* injectedRxBuffer = GetRxBufferUSB(usb);
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);    
}

static void InjectPreOrSufix(uint8_t* buffer, uint8_t startIdx)
{
    // inject prefix of msg
    for (uint8_t i = 0; i < 6; i++)
        buffer[i + startIdx] = 0xAA;
}

static void FillRxBuffer(uint8_t* buffer, USBAction_e action)
{
    switch (action)
    {
        case USB_REMOVE_SECTOR:
            // inject remove sector at 3 ID
            buffer[6] = 0;      // command ID
            buffer[7] = 3;      // sector ID
        break;
        case USB_ADD_SECTOR:
            // inject add sector with 2 ID
            buffer[6] = 1;      // command ID    
            buffer[7] = 2;      // sector ID
            buffer[8] = 0;      // start diode ID
            buffer[9] = 0;      // start diode ID
            buffer[10] = 0;     // start diode ID
            buffer[11] = 1;     // start diode ID (32 bits)
            buffer[12] = 0;     // end diode ID
            buffer[13] = 0;     // end diode ID
            buffer[14] = 0;     // end diode ID
            buffer[15] = 20;    // end diode ID (32 bits)
        break; 
        case USB_SET_DIODE_COLOR_HSV:
            // inject setdiode nr 4 with HSV color
            buffer[6] = 2;      // command ID    
            buffer[7] = 0;      // diode ID
            buffer[8] = 0;      // diode ID
            buffer[9] = 0;      // diode ID
            buffer[10] = 4;     // diode ID (32 bits)
            buffer[11] = 1;     // hue
            buffer[12] = 100;   // hue (16 bits)
            buffer[13] = 50;    // saturation
            buffer[14] = 60;    // value
        break;
        case USB_SET_DIODE_COLOR_RGB:
            // inject diode nr 4 with RGB color
            buffer[6] = 3;      // command ID    
            buffer[7] = 1;      // diode ID
            buffer[8] = 0;      // diode ID
            buffer[9] = 3;      // diode ID
            buffer[10] = 8;     // diode ID (32 bits)
            buffer[11] = 200;   // red
            buffer[12] = 100;   // green
            buffer[13] = 130;   // blue
        break;
        case USB_SET_SECTOR_COLOR_HSV:
            // inject sector nr 1 with HSV color
            buffer[6] = 4;      // command ID    
            buffer[7] = 1;      // sector ID
            buffer[8] = 1;      // hue
            buffer[9] = 10;     // hue (16 bits)
            buffer[10] = 100;   // saturation
            buffer[11] = 90;    // value
        break;
        case USB_SET_SECTOR_COLOR_RGB:
            // inject sector nr 0 with RGB color
            buffer[6] = 5;      // command ID    
            buffer[7] = 0;      // sector ID
            buffer[8] = 200;    // red
            buffer[9] = 10;     // green
            buffer[10] = 100;   // blue
        break;
        case USB_SET_SECTOR_COLOR_RAINBOW:
            // inject sector nr 5 with Rainbow
            buffer[6] = 6;      // command ID    
            buffer[7] = 5;      // sector ID
        break;
        case USB_SET_SECTOR_SPAWN_DIODE_COLOR:
            // inject spawn diode RGB color woth sector nr 2
            buffer[6] = 7;      // command ID    
            buffer[7] = 2;      // sector ID
            buffer[8] = 200;    // red
            buffer[9] = 10;     // green
            buffer[10] = 100;   // blue
        break;
        case USB_SET_SECTOR_ANIMATION_SPEED:
            // inject sector 4 with animation speed
            buffer[6] = 8;      // command ID    
            buffer[7] = 4;      // sector ID
            buffer[8] = 0;      // speed
            buffer[9] = 0;      // speed
            buffer[10] = 0;     // speed
            buffer[11] = 10;    // speed (32 bits)
        break;
        default:
        break; 
    }
}

// Check that after initialization object is not NULL
void Test_ObjectNotNullPtr(void)
{
    TEST_ASSERT_NOT_NULL(usb);
}

// Check that after object initialization all members are 0
void Test_CheckAfterInitAllMembersAreZero(void)
{
    uint8_t expectedBuffer[64] = {0};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, GetTxBufferUSB(usb), sizeof(expectedBuffer));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, GetRxBufferUSB(usb), sizeof(expectedBuffer));
    TEST_ASSERT_EQUAL_UINT8(0, *GetFlagUSBPtr(usb));
    TEST_ASSERT_EQUAL_UINT32(0, GetMsgLen(usb));
}

// Check decoding messages feature
void Test_DecodingMessages(void)
{
    USBMsg_t msg;
    uint8_t* injectedRxBuffer = GetRxBufferUSB(usb);
    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_REMOVE_SECTOR);
    InjectPreOrSufix(injectedRxBuffer, 8);
    
    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_REMOVE_SECTOR, msg.action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_ADD_SECTOR);
    InjectPreOrSufix(injectedRxBuffer, 16);
    
    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_ADD_SECTOR, msg.action);
    
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 15);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_DIODE_COLOR_HSV, msg.action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_RGB);
    InjectPreOrSufix(injectedRxBuffer, 14);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_DIODE_COLOR_RGB, msg.action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 12);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_COLOR_HSV, msg.action);
    
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_COLOR_RGB);
    InjectPreOrSufix(injectedRxBuffer, 11);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_COLOR_RGB, msg.action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_COLOR_RAINBOW);
    InjectPreOrSufix(injectedRxBuffer, 8);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_COLOR_RAINBOW, msg.action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_SPAWN_DIODE_COLOR);
    InjectPreOrSufix(injectedRxBuffer, 11);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_SPAWN_DIODE_COLOR, msg.action);
    
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_ANIMATION_SPEED);
    InjectPreOrSufix(injectedRxBuffer, 12);

    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_ANIMATION_SPEED, msg.action);
}

// Check that decode USB_SET_DIODE_COLOR_HSV and USB_SET_DIODE_COLOR_RGB works as expected
void Test_USBSetDiodeColorHSVAndRGB(void)
{
    USBMsg_t msg;
    uint8_t* injectedRxBuffer = GetRxBufferUSB(usb);
    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 15);
    
    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_DIODE_COLOR_HSV, msg.action);
    TEST_ASSERT_EQUAL_UINT32(4, msg.diodeID);
    TEST_ASSERT_EQUAL_UINT16(356, msg.hsvColor.hue);
    TEST_ASSERT_EQUAL_UINT8(50, msg.hsvColor.saturation);
    TEST_ASSERT_EQUAL_UINT8(60, msg.hsvColor.value);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_RGB);
    InjectPreOrSufix(injectedRxBuffer, 14);
    
    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_DIODE_COLOR_RGB, msg.action);
    TEST_ASSERT_EQUAL_UINT32(16777992, msg.diodeID);
    TEST_ASSERT_EQUAL_UINT8(200, msg.rgbColor.red);
    TEST_ASSERT_EQUAL_UINT8(100, msg.rgbColor.green);
    TEST_ASSERT_EQUAL_UINT8(130, msg.rgbColor.blue);
}

// Check that wrong message appendix raise error
void Test_WrongMessageAppendix(void)
{
    USBMsg_t msg;
    uint8_t* injectedRxBuffer = GetRxBufferUSB(usb);
    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 18);                     // wrong appendix (wrong place)
    
    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_BAD_APPENDIX, msg.action);      // it should be bad appendix
}

// Check that wrong message prefix raise error
void Test_WrongMessagePrefix(void)
{
    USBMsg_t msg;
    uint8_t* injectedRxBuffer = GetRxBufferUSB(usb);
    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 15);
    // wrong prefix
    injectedRxBuffer[6] = 0xAA;
    injectedRxBuffer[7] = 0xAA;
    
    msg = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_BAD_PREFIX, msg.action);      // it should be bad appendix
}
