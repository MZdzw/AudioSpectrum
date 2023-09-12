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
            buffer[11] = 2;     // hue
            buffer[12] = 100;   // hue (16 bits)
            buffer[13] = 50;    // saturation
            buffer[14] = 60;    // value
        break;
        case USB_SET_DIODE_COLOR_RGB:
            // inject diode nr 4 with RGB color
            buffer[6] = 3;      // command ID    
            buffer[7] = 0;      // diode ID
            buffer[8] = 0;      // diode ID
            buffer[9] = 0;      // diode ID
            buffer[10] = 4;     // diode ID (32 bits)
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
    USBAction_e action;
    uint8_t* injectedRxBuffer = GetRxBufferUSB(usb);
    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_REMOVE_SECTOR);
    InjectPreOrSufix(injectedRxBuffer, 8);
    
    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_REMOVE_SECTOR, action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_ADD_SECTOR);
    InjectPreOrSufix(injectedRxBuffer, 16);
    
    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_ADD_SECTOR, action);
    
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 15);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_DIODE_COLOR_HSV, action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_DIODE_COLOR_RGB);
    InjectPreOrSufix(injectedRxBuffer, 14);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_DIODE_COLOR_RGB, action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_COLOR_HSV);
    InjectPreOrSufix(injectedRxBuffer, 12);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_COLOR_HSV, action);
    
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_COLOR_RGB);
    InjectPreOrSufix(injectedRxBuffer, 11);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_COLOR_RGB, action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_COLOR_RAINBOW);
    InjectPreOrSufix(injectedRxBuffer, 8);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_COLOR_RAINBOW, action);

    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_SPAWN_DIODE_COLOR);
    InjectPreOrSufix(injectedRxBuffer, 11);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_SPAWN_DIODE_COLOR, action);
    
    memset(injectedRxBuffer, 0, sizeof(uint8_t) * 64);

    InjectPreOrSufix(injectedRxBuffer, 0);
    FillRxBuffer(injectedRxBuffer, USB_SET_SECTOR_ANIMATION_SPEED);
    InjectPreOrSufix(injectedRxBuffer, 12);

    action = DecodeMsg(usb);
    TEST_ASSERT_EQUAL_UINT8(USB_SET_SECTOR_ANIMATION_SPEED, action);
}

