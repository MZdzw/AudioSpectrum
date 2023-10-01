#include <stdbool.h>
#include "unity.h"
#include <stdlib.h>     // for dynamic memory allocation
// this techique can be used in other tests
// we include c files to get the access to static variables/functions
// we do it only for tests purposes
// remember not to compile "activity.c" file!!!
#include "activity.c"
// include mock headers
#include "mock_communication.h"
#include "mock_ws2812bDriver.h"
#include "mock_animations.h"

LedStrip_t* objLeds;
Animation_t animationsArray[MAX_SECTORS];
Ws2812b_Driver_t* objWs2812b_Driver;
Communication_t* objCommunication;

Ws2812b_Sector_t sectorsMocked[MAX_SECTORS];
Ws2812b_Diode_t diodesMocked[WS2812B_DIODES];

void setUp (void) /* Is run before every test, put unit init calls here. */
{
    // Arange
    objLeds = malloc(sizeof(LedStrip_t));
    objWs2812b_Driver = malloc(sizeof(Ws2812b_Driver_t));
    objCommunication = malloc(sizeof(Communication_t));

    LedStrip_initObject_ExpectAndReturn(objLeds);
    Ws2812b_initObject_ExpectAndReturn(objWs2812b_Driver);
    Communication_InitObject_ExpectAndReturn(objCommunication);

    for (unsigned int i = 0; i < MAX_SECTORS; i++)
    {
        sectorsMocked[i].startDiode = 0;
        sectorsMocked[i].endDiode = 0;
        sectorsMocked[i].isUsed = false;

        animationsArray[i].sectorId = 0;
        animationsArray[i].speed = 0;
        animationsArray[i].animation_p = NULL;
    }
}
void tearDown (void) /* Is run after every test, put unit clean-up calls here. */
{
    for (unsigned int i = 0; i < MAX_SECTORS; i++)
    {
        sectorsMocked[i].startDiode = 0;
        sectorsMocked[i].endDiode = 0;
        sectorsMocked[i].isUsed = false;
    }
    Ws2812b_Color_t colorClear = {{0, 0, 0}, {0, 0, 0}, RGB};
    for (unsigned int i = 0; i < WS2812B_DIODES; i++)
    {
        diodesMocked[i].diodeColor = colorClear;
        diodesMocked[i].dimDirection = NONE;
        diodesMocked[i].next = NULL;
    }
    
    free(objLeds);
    free(objWs2812b_Driver);
    free(objCommunication);
}

// check that if we dont decode message before the functions works as expected
void Test_NoDecodeMessageBefore(void)
{
    USBMsg_t msg;
    Activity_e action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_UINT8(ERRORS, action);
}

// Check that if decode return ssome kind of msg, the program set proper activity
void Test_CheckThatProperActivityIsSet(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].isUsed = true;

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    RemoveSector_IgnoreAndReturn(true);

    DecodeMsg_ExpectAndReturn(objCommunication, (USBMsg_t){.action = USB_REMOVE_SECTOR});
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivityRemoveSector, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_ADD_SECTOR, .sectorID = 1, .diodesRange.startDiode = 21, .diodesRange.endDiode = 25}));
    // SetSector_ExpectAndReturn(objWs2812b_Driver, 1, 21, 25, true);
    msg = DecodeMsg(objCommunication);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetSector_ExpectAndReturn(objWs2812b_Driver, 1, 21, 25, true);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivityAddSector, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_IgnoreAndReturn(sectorsMocked);
    GetDiodesArray_IgnoreAndReturn(diodesMocked);
    // SetDiodeColorHSV_Expect(objWs2812b_Driver, 9, 300, 100, 100);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_HSV, .diodeID = 9, .hsvColor = {300, 100, 100}}));
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeHSV, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_NOT_IN_USED_SECTOR, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    // SetDiodeColorRGB_Expect(objWs2812b_Driver, 9, 250, 100, 100);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_RGB, .diodeID = 9, .rgbColor = {250, 100, 100}}));
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeRGB, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_NOT_IN_USED_SECTOR, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetHSVColorForSector_Ignore();
    DecodeMsg_ExpectAndReturn(objCommunication, (USBMsg_t){.action = USB_SET_SECTOR_COLOR_HSV});
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorHSV, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetRGBColorForSector_Ignore();
    DecodeMsg_ExpectAndReturn(objCommunication, (USBMsg_t){.action = USB_SET_SECTOR_COLOR_RGB});
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRGB, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetRainbowForSector_Ignore();
    DecodeMsg_ExpectAndReturn(objCommunication, (USBMsg_t){.action = USB_SET_SECTOR_COLOR_RAINBOW});
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRainbow, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    // GetDiodesArray_IgnoreAndReturn(diodesMocked);
    SetDiodeColorRGB_Ignore();
    DecodeMsg_ExpectAndReturn(objCommunication, (USBMsg_t){.action = USB_SET_SECTOR_SPAWN_DIODE_COLOR});
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorSpawnDiode, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    GetAnimations_IgnoreAndReturn(animationsArray);
    SetAnimationSpeed_Ignore();
    DecodeMsg_ExpectAndReturn(objCommunication, (USBMsg_t){.action = USB_SET_SECTOR_ANIMATION_SPEED});
    msg = DecodeMsg(objCommunication);

    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorAnimationSpeed, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

// Check remove sector activity
void Test_RemoveSector(void)
{
    // Arange, Act, Assert
    // Arange
    USBMsg_t msg;
    Activity_e action;
    // firstly we want to remove proper sector
    // Act
    // prepare msg (relate to Test_communication.c file)
    sectorsMocked[0].isUsed = true;
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    RemoveSector_IgnoreAndReturn(true);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_REMOVE_SECTOR, .sectorID = 0}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);
    // Assert
    TEST_ASSERT_EQUAL_PTR(&ActivityRemoveSector, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);

    // then we want to remove not existing sector
    sectorsMocked[3].isUsed = false;
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    RemoveSector_IgnoreAndReturn(false);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_REMOVE_SECTOR, .sectorID = 3}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivityRemoveSector, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_NON_EXISITING, action);

    // and lastly remove sector out of range
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    RemoveSector_IgnoreAndReturn(false);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_REMOVE_SECTOR, .sectorID = MAX_SECTORS}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivityRemoveSector, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_OUT_OF_RANGE, action);
}

void Test_AddSector(void)
{
    // Arange, Act, Assert
    // Arange
    USBMsg_t msg;
    Activity_e action;

    // firstly we want to add sector when there is place for that
    // Act
    // prepare msg (relate to Test_communication.c file)
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 20;
    sectorsMocked[0].isUsed = true;
    
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetSector_ExpectAndReturn(objWs2812b_Driver, 1, 21, 25, true);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_ADD_SECTOR, .sectorID = 1, .diodesRange = {21, 25}}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    // Assert
    TEST_ASSERT_EQUAL_PTR(&ActivityAddSector, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

// Check if adding existing sector fails
void Test_AddExisitngSector(void)
{
    USBMsg_t msg;
    Activity_e action;
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 20;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 21;
    sectorsMocked[1].endDiode = 25;
    sectorsMocked[1].isUsed = true;

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_ADD_SECTOR, .sectorID = 1, .diodesRange = {21, 25}}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivityAddSector, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_ALREADY_EXIST, action);
}

// Check adding diode out of range
void Test_AddDiodeOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;
    // we want to add diode out of range
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_ADD_SECTOR, .sectorID = 2, .diodesRange = {WS2812B_DIODES + 5, WS2812B_DIODES + 10}}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);
    TEST_ASSERT_EQUAL_PTR(&ActivityAddSector, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_OUT_OF_RANGE, action);
}

void Test_AddOverlappingSector(void)
{
    // Arange, Act, Assert
    // Arange
    USBMsg_t msg;
    Activity_e action;

    // firstly we want to add sector when there is place for that
    // Act
    // prepare msg (relate to Test_communication.c file)
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 5;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 8;
    sectorsMocked[1].endDiode = 20;
    sectorsMocked[1].isUsed = true;

    // we want add sector which overlapping with other 
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_ADD_SECTOR, .sectorID = 2, .diodesRange = {15, 25}}));
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetSector_ExpectAndReturn(objWs2812b_Driver, 2, 15, 25, false);
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivityAddSector, act);
    TEST_ASSERT_EQUAL_UINT8(SECTORS_OVERLAPPING, action);
}

void Test_ActivitySetDiodeColorHSV(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].firstDiode = diodesMocked;
    sectorsMocked[0].lastDiode = diodesMocked + WS2812B_DIODES - 1;
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = WS2812B_DIODES - 1;
    sectorsMocked[0].isUsed = true;

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_StopIgnore();
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);

    GetDiodesArray_StopIgnore();
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorHSV_Expect(diodesMocked + 3, ((Ws2812b_HSV_t){200, 100, 100}));

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_HSV, .diodeID = 3, .hsvColor = {200, 100, 100}}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeHSV, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

void Test_ActivitySetDiodeColorHSVDiodeIDOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorHSV_Expect(diodesMocked + WS2812B_DIODES + 10, ((Ws2812b_HSV_t){200, 100, 100}));
    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_HSV, .diodeID = WS2812B_DIODES + 10, .hsvColor = {200, 100, 100}}));

    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeHSV, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_OUT_OF_RANGE, action);
}

void Test_ActivitySetDiodeColorHSVValuesOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].firstDiode = diodesMocked;
    sectorsMocked[0].lastDiode = diodesMocked + WS2812B_DIODES - 1;
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = WS2812B_DIODES - 1;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_HSV, .diodeID = 9, .hsvColor = {400, 100, 100}}));

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorHSV_Expect(diodesMocked + 9, ((Ws2812b_HSV_t){400, 100, 100}));

    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeHSV, act);
    TEST_ASSERT_EQUAL_UINT8(COLOR_OUT_OF_RANGE, action);
}

void Test_ActivitySetDiodeColorHSVInNotUsedSector(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].firstDiode = diodesMocked;
    sectorsMocked[0].lastDiode = diodesMocked + 14;
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 14;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].firstDiode = diodesMocked + 17;
    sectorsMocked[1].lastDiode = diodesMocked + 22;
    sectorsMocked[1].startDiode = 17;
    sectorsMocked[1].endDiode = 22;
    sectorsMocked[1].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_HSV, .diodeID = 25, .hsvColor = {20, 100, 100}}));

    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);

    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeHSV, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_NOT_IN_USED_SECTOR, action);
}

void Test_ActivitySetDiodeColorRGB(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_RGB, .diodeID = 9, .rgbColor = {200, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorRGB_Expect(diodesMocked + 9, ((Ws2812b_RGB_t){200, 100, 100}));
    
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeRGB, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

void Test_ActivitySetDiodeColorRGBDiodeIDOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_RGB, .diodeID = WS2812B_DIODES + 10, .hsvColor = {200, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);

    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeRGB, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_OUT_OF_RANGE, action);
}

void Test_ActivitySetDiodeColorRGBInNotUsedSector(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 14;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 17;
    sectorsMocked[1].endDiode = 22;
    sectorsMocked[1].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_DIODE_COLOR_RGB, .diodeID = 28, .rgbColor = {100, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);

    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetDiodeRGB, act);
    TEST_ASSERT_EQUAL_UINT8(DIODE_ID_NOT_IN_USED_SECTOR, action);
}

void Test_ActivitySetSectorColorHSV(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_HSV, .sectorID = 0, .hsvColor = {200, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorHSV_Expect(diodesMocked, ((Ws2812b_HSV_t){200, 100, 100}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorHSV, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

void Test_ActivitySetSectorColorHSVSectorIDOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_HSV, .sectorID = MAX_SECTORS + 3, .hsvColor = {200, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);
    
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorHSV, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_OUT_OF_RANGE, action);
}

void Test_ActivitySetSectorColorHSVValuesOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 11;
    sectorsMocked[1].endDiode = 18;
    sectorsMocked[1].isUsed = true;
    sectorsMocked[2].startDiode = 20;
    sectorsMocked[2].endDiode = 28;
    sectorsMocked[2].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_HSV, .sectorID = 2, .hsvColor = {400, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorHSV, act);
    TEST_ASSERT_EQUAL_UINT8(COLOR_OUT_OF_RANGE, action);
}

void Test_ActivitySetSectorColorHSVInNotUsedSector(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 11;
    sectorsMocked[1].endDiode = 18;
    sectorsMocked[1].isUsed = true;
    sectorsMocked[2].startDiode = 20;
    sectorsMocked[2].endDiode = 28;
    sectorsMocked[2].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_HSV, .sectorID = 3, .hsvColor = {250, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorHSV, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_NOT_USED, action);
}

void Test_ActivitySetSectorColorRGB(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_RGB, .sectorID = 0, .rgbColor = {200, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorHSV_Expect(diodesMocked, ((Ws2812b_HSV_t){200, 100, 100}));
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRGB, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

void Test_ActivitySetSectorColorRGBSectorIDOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;

    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 30;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_RGB, .sectorID = MAX_SECTORS + 3, .rgbColor = {200, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);
    
    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRGB, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_OUT_OF_RANGE, action);
}

void Test_ActivitySetSectorColorRGBInNotUsedSector(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 11;
    sectorsMocked[1].endDiode = 18;
    sectorsMocked[1].isUsed = true;
    sectorsMocked[2].startDiode = 20;
    sectorsMocked[2].endDiode = 28;
    sectorsMocked[2].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_RGB, .sectorID = 3, .rgbColor = {250, 100, 100}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRGB, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_NOT_USED, action);
}

void Test_ActivitySetSectorColorRainbow(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_RAINBOW, .sectorID = 0}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    SetRainbowForSector_Expect(objWs2812b_Driver, 0);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRainbow, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

void Test_ActivitySetSectorColorRainbowSectorIdOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_RAINBOW, .sectorID = 7}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRainbow, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_OUT_OF_RANGE, action);
}

void Test_ActivitySetSectorColorRainbowSectorIdNotInUse(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 11;
    sectorsMocked[1].endDiode = 20;
    sectorsMocked[1].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_COLOR_RAINBOW, .sectorID = 2}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorColorRainbow, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_NOT_USED, action);
}

void Test_ActivitySetSectorSpawnDiode(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].firstDiode = diodesMocked;
    sectorsMocked[0].lastDiode = diodesMocked + 10;
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_SPAWN_DIODE_COLOR, .sectorID = 0, .rgbColor = {200, 120, 60}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
    
    GetDiodesArray_ExpectAndReturn(objWs2812b_Driver, diodesMocked);
    SetDiodeColorRGB_Expect(diodesMocked, ((Ws2812b_RGB_t){200, 120, 60}));
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorSpawnDiode, act);
    TEST_ASSERT_EQUAL_UINT8(OK, action);
}

void Test_ActivitySetSectorSpawnDiodeSectorIdOutOfRange(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_SPAWN_DIODE_COLOR, .sectorID = MAX_SECTORS, .rgbColor = {200, 120, 60}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorSpawnDiode, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_OUT_OF_RANGE, action);
}

void Test_ActivitySetSectorSpawnDiodeSectorIdNotInUse(void)
{
    USBMsg_t msg;
    Activity_e action;
    
    sectorsMocked[0].startDiode = 0;
    sectorsMocked[0].endDiode = 10;
    sectorsMocked[0].isUsed = true;
    sectorsMocked[1].startDiode = 12;
    sectorsMocked[1].endDiode = 25;
    sectorsMocked[1].isUsed = true;

    DecodeMsg_ExpectAndReturn(objCommunication, ((USBMsg_t){.action = USB_SET_SECTOR_SPAWN_DIODE_COLOR, .sectorID = 2, .rgbColor = {200, 120, 60}}));
    GetDriver_ExpectAndReturn(objLeds, objWs2812b_Driver);
    GetSectors_ExpectAndReturn(objWs2812b_Driver, sectorsMocked);
   
    msg = DecodeMsg(objCommunication);
    action = ActivateAction(objLeds, &msg);

    TEST_ASSERT_EQUAL_PTR(&ActivitySetSectorSpawnDiode, act);
    TEST_ASSERT_EQUAL_UINT8(SECTOR_ID_NOT_USED, action);
}
