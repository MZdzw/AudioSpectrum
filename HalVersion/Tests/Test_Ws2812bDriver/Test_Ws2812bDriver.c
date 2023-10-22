#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "math.h"
#include "unity.h"
#include "ws2812bDriver.h"
#include "mock_spiInterface.h"

Ws2812b_Driver_t* deviceDriver;

Ws2812b_RGB_t expectedArray[WS2812B_DIODES];

void setUp (void) /* Is run before every test, put unit init calls here. */
{
    deviceDriver = Ws2812b_initObject();
}
void tearDown (void) /* Is run after every test, put unit clean-up calls here. */
{
    // clear DiodeColors array
    Ws2812b_Diode_t element = {{{0, 0, 0}, {0, 0, 0}, RGB}, NONE, NULL};
    Ws2812b_Diode_t* diodesArrayToClean = GetDiodesArray(deviceDriver);
    for (uint32_t i = 0; i < WS2812B_DIODES; i++)
    {
        diodesArrayToClean[i] = element;
    }
    // clear spiBuffer after each test
    for (uint32_t i = 0; i < WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144; i++)
    {
        setSpiBufferElement(GetDeviceBuffer(deviceDriver), 0, i);
    }
    // clear sectors
    Ws2812b_Sector_t sector = {NULL, NULL, false};
    Ws2812b_Sector_t* sectorsToClear = GetSectors(deviceDriver);
    for (uint8_t i = 0; i < MAX_SECTORS; i++)
    {
        sectorsToClear[i] = sector;
    }
    SetActiveSectors(deviceDriver, 0);    
}

// Check that after initialize the object is not NULL
void test_ObjectNotNullPtr(void)
{
    TEST_ASSERT_NOT_NULL(deviceDriver);
}

// Check that all values inside diodeColors are equal (0,0,0)
void test_DiodesArrayClearAfterInit(void)
{
    Ws2812b_RGB_t actualArray[WS2812B_DIODES];
    for (uint32_t i = 0; i < WS2812B_DIODES; i++)
    {
        expectedArray[i].red = 0;
        expectedArray[i].green = 0;
        expectedArray[i].blue = 0;

        actualArray[i].red = GetDiodesArray(deviceDriver)[i].diodeColor.rgb.red;
        actualArray[i].green = GetDiodesArray(deviceDriver)[i].diodeColor.rgb.green;
        actualArray[i].blue = GetDiodesArray(deviceDriver)[i].diodeColor.rgb.blue;
    }
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedArray, actualArray, sizeof(Ws2812b_RGB_t), WS2812B_DIODES);
}

// Check that after setting, will get the same value
void test_SettingAndGettingTheSameValue(void)
{
    uint32_t id = 4;
    Ws2812b_RGB_t rgbExpected = {.red = 90, .green = 122, .blue = 140};
    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + id, rgbExpected);

    Ws2812b_RGB_t rgbActual = GetDiodeColorRGB(GetDiodesArray(deviceDriver) + id);
    
    TEST_ASSERT_EQUAL_MEMORY(&rgbExpected, &rgbActual, sizeof(Ws2812b_RGB_t));

}

// Check that after init device Buffer (buffer connected directly with Spi hardware)
// is not NULL
void test_DeviceBufferNotNullAfterInit(void)
{
    TEST_ASSERT_NOT_NULL(GetDeviceBuffer(deviceDriver));
}

// Check deviceBuffer if it's filled correctly
void test_DeviceBufferFilledCorectly(void)
{
    uint8_t expectedBuffer[WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144] = {0};
    // At begining buffer all buffer elements should be equal zero
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, getSpiBufferPointer(GetDeviceBuffer(deviceDriver)), sizeof(expectedBuffer));
    SendBufferOverSpi_CMockIgnore();
    uint32_t id = 4;
    Ws2812b_RGB_t rgb = {192, 0, 0};
    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + id, rgb);
    SendDeviceBuffer(deviceDriver);
    // first 144 bytes should be equal to zero - reset
    // one color posses 24 bytes (24 array elements)
    // One diode is composed of 3 colors (24 * 3 = 72 bytes)
    // So 4 diodes will take 72 * 4 = 288 bytes of space
    // So in total 288 + 144(reset) = 432 bytes (0 - 431)
    // at 432 position in device buffer should be green color(432-455)
    // at 456 position in device buffer should be red color(456-479)
    // at 480 position in device buffer should be blue color(480-503)
    for (uint32_t i = 0; i < WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144; i++)
    {
        // First 144 bytes should be equal zero (reset signal)
        if (i < 144)
        {
            expectedBuffer[i] = SPI_ZERO;
        }
        // Next bytes should be Diodes colors signal
        else
        {
            // every third byte should always be equal 255 (SPI_ONE)
            // no matter if we send 0 or 1 bit (look in ws2812b datasheet)
            // three bytes of spi are equal to 1 bit of ws2812b
            if (i % 3 == 0)
            {
                expectedBuffer[i] = SPI_ONE;
            }
            else
            {
                // we set 192 value of red color for diode with 4 ID
                // Check calculates above to have better understanding
                // According to that information in buffer at place 457
                // and 459 we should have get SPI_ONE instead of SPI_ZERO
                if (i == 457 || i == 460)
                {
                    expectedBuffer[i] = SPI_ONE;
                }
                else
                {
                    expectedBuffer[i] = SPI_ZERO;
                }
            }
        }
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, getSpiBufferPointer(GetDeviceBuffer(deviceDriver)), sizeof(expectedBuffer));
}

// Check that first element is filled corectly
void test_DeviceBufferFirstElementFilledCorectly(void)
{
    uint8_t expectedBuffer[WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144] = {0};
    // At begining buffer all buffer elements should be equal zero
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, getSpiBufferPointer(GetDeviceBuffer(deviceDriver)), sizeof(expectedBuffer));
    SendBufferOverSpi_CMockIgnore();
    uint32_t id = 0;
    Ws2812b_RGB_t rgb = {143, 189, 40};
    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + id, rgb);
    SendDeviceBuffer(deviceDriver);
    for (uint32_t i = 0; i < WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144; i++)
    {
        if (i < 144)
        {
            expectedBuffer[i] = SPI_ZERO;
        }
        else
        {
            if (i % 3 == 0)
            {
                expectedBuffer[i] = SPI_ONE;
            }
            else
            {
                if (i == 145 || i == 151 || i == 154 ||    // green color
                    i == 157 || i == 160 || i == 166 ||    // green color
                    i == 169 || i == 181 || i == 184 ||    // red color
                    i == 187 || i == 190 ||                // red color
                    i == 199 || i == 205)                  // blue color 
                {
                    expectedBuffer[i] = SPI_ONE;
                }
                else
                {
                    expectedBuffer[i] = SPI_ZERO;
                }
            }
        }
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, getSpiBufferPointer(GetDeviceBuffer(deviceDriver)), sizeof(expectedBuffer));
}

// Check that last element is filled corectly
void test_DeviceBufferLastElementFilledCorectly(void)
{
    uint8_t expectedBuffer[WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144] = {0};
    // At begining buffer all buffer elements should be equal zero
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, getSpiBufferPointer(GetDeviceBuffer(deviceDriver)), sizeof(expectedBuffer));
    // HAL_SPI_Transmit_DMA_IgnoreAndReturn(0);    // 0 means return OK
    uint32_t id = WS2812B_DIODES - 1;
    Ws2812b_RGB_t rgb = {200, 222, 153};
    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + id, rgb);
    SendDeviceBuffer(deviceDriver);
    for (uint32_t i = 0; i < WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144; i++)
    {
        if (i < 144)
        {
            expectedBuffer[i] = SPI_ZERO;
        }
        else
        {
            if (i % 3 == 0)
            {
                expectedBuffer[i] = SPI_ONE;
            }
            else
            {
                if (i == 2233 || i == 2236 || i == 2242 ||    // green color
                    i == 2245 || i == 2248 || i == 2251 ||    // green color
                    i == 2257 || i == 2260 || i == 2269 ||    // red color
                    i == 2281 || i == 2290 || i == 2293 ||    // blue color
                    i == 2302)                                // blue color 
                {
                    expectedBuffer[i] = SPI_ONE;
                }
                else
                {
                    expectedBuffer[i] = SPI_ZERO;
                }
            }
        }
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, getSpiBufferPointer(GetDeviceBuffer(deviceDriver)), sizeof(expectedBuffer));
}

// Check if sectors are filled correctly
// after driver initialization, there should be one sector
// this sector should start from 0 diode to WS2812B_DIODES
// this sector should be having set isUsed variable
// next sectors should be zeros
void Test_CheckIfSectorsAreWellInitialized(void)
{
    Ws2812b_Sector_t* sector = GetSectors(deviceDriver);
    TEST_ASSERT_EQUAL_PTR(&(GetDiodesArray(deviceDriver)[0]), sector[0].firstDiode);
    TEST_ASSERT_EQUAL_PTR(&(GetDiodesArray(deviceDriver)[WS2812B_DIODES - 1]), sector[0].lastDiode);
    TEST_ASSERT_TRUE(sector[0].isUsed);
    for (uint8_t i = 1; i < MAX_SECTORS; i++)
    {
        TEST_ASSERT_EQUAL_PTR(NULL, sector[i].firstDiode);
        TEST_ASSERT_EQUAL_PTR(NULL, sector[i].lastDiode);
        TEST_ASSERT_FALSE(sector[i].isUsed);
    }
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Check that removing sector works
void Test_CheckIfSectorRemoveWorks(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
}

// Check that removing sector which is not used return false
void Test_CheckIfSectorRemoveOfNotUsedSectorFails(void)
{
    TEST_ASSERT_FALSE(RemoveSector(deviceDriver, 1));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Check remove default sector and then add two not overlapping
void Test_CheckRemoveDefaultSectorAndThenAddTwoNoOverlapping(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, 0, 10));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 1, 11, WS2812B_DIODES - 1));
    TEST_ASSERT_EQUAL_UINT(2, GetActiveSectors(deviceDriver));
}

// Check overlapping sectors
void Test_CheckOverlappingSectors(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, 0, 15));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 1, 11, WS2812B_DIODES));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Create sector on edge
void Test_CreateSectorOnEdge(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, WS2812B_DIODES - 5, 5));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Create sector on edge and add next non overlapping sectors
void Test_CreateSectorOnEdgeAndAddNonOverlappingSectors(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, WS2812B_DIODES - 5, 5));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 1, 6, 10));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 2, 11, 16));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 3, 17, 22));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 4, 23, 24));
    TEST_ASSERT_EQUAL_UINT(5, GetActiveSectors(deviceDriver));
}

// Create sector on edge and add next overlapping sector
void Test_CreateSectorOnEdgeAndAddOverlappingSector(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, WS2812B_DIODES - 5, 5));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 1, 2, 10));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Create sector on edge and add next sector on edge
void Test_CreateSectorOnEdgeAndAddAnotherSectorOnEdge(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, WS2812B_DIODES - 5, 5));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 1, WS2812B_DIODES - 2, WS2812B_DIODES - 4));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Create sector and then create another sector which will be inside the first one,
// then add another which will have the first one inside 
void Test_CreateSectorAndAddSectorInsideAndAddSectorOutside(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_TRUE(SetSector(deviceDriver, 0, 8, 20));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 1, 12, 15));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 2, 1, 25));
    TEST_ASSERT_EQUAL_UINT(1, GetActiveSectors(deviceDriver));
}

// Check that creating sector out of diodes range return false
void Test_CreateSectorOutOfRange(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 0, WS2812B_DIODES, 2));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 1, WS2812B_DIODES, WS2812B_DIODES + 20));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
}

// Check creating with diodes of minus indexes
void Test_CreateSectorWithDiodesOfMinusIndexes(void)
{
    TEST_ASSERT_TRUE(RemoveSector(deviceDriver, 0));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
    TEST_ASSERT_FALSE(SetSector(deviceDriver, 0, -5, -10));
    TEST_ASSERT_EQUAL_UINT(0, GetActiveSectors(deviceDriver));
}

// Check if HSV sets RGB correctly
void Test_SettingHSVSetsRGBCorrectly(void)
{
    uint32_t id = 5;
    Ws2812b_RGB_t rgbExpected = {61, 102, 41};

    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + id, rgbExpected);
    Ws2812b_RGB_t rgbActual;
    rgbActual = GetDiodeColorRGB(GetDiodesArray(deviceDriver) + id);

    TEST_ASSERT_EQUAL_MEMORY(&rgbExpected, &rgbActual, sizeof(Ws2812b_RGB_t));


    SetDiodeColorHSV(GetDiodesArray(deviceDriver) + id, (Ws2812b_HSV_t){285, 53, 6});
    rgbExpected.red = 13;
    rgbExpected.green = 7;
    rgbExpected.blue = 15;
    rgbActual = GetDiodeColorRGB(GetDiodesArray(deviceDriver) + id);

    TEST_ASSERT_EQUAL_MEMORY(&rgbExpected, &rgbActual, sizeof(Ws2812b_RGB_t));
}

// Check getter of HSV (check if converse from RGB to HSV is correct)
void Test_GettingHSVfromRGB(void)
{
    uint32_t id = 8;
    Ws2812b_RGB_t rgbExpected = {61, 102, 40};

    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + id, rgbExpected);
    Ws2812b_RGB_t rgbActual;   
    rgbActual = GetDiodeColorRGB(GetDiodesArray(deviceDriver) + id);

    TEST_ASSERT_EQUAL_MEMORY(&rgbExpected, &rgbActual, sizeof(Ws2812b_RGB_t));
    // getting HSV
    Ws2812b_HSV_t hsvActual;
    Ws2812b_HSV_t hsvExpected = {100, 61, 40};
    hsvActual = GetDiodeColorHSV(GetDiodesArray(deviceDriver) + id);

    TEST_ASSERT_EQUAL_MEMORY(&hsvExpected, &hsvActual, sizeof(Ws2812b_HSV_t));
}

// Check setting random HSV and getting HSV (it checks conversion between HSV-RGB and RGB-HSV)
void Test_SettingRandomHSVAndGettingHSVBack(void)
{
    srand(time(NULL));
    uint32_t id = 8;
    Ws2812b_HSV_t hsvRandom, hsv;
    
    for (unsigned int i = 0; i < 100; i++)
    {
        hsvRandom.hue = rand() % 361;
        hsvRandom.saturation = rand() % 101;
        hsvRandom.value = rand() % 101;
        SetDiodeColorHSV(GetDiodesArray(deviceDriver) + id, hsvRandom);
        hsv = GetDiodeColorHSV(GetDiodesArray(deviceDriver) + id);

        TEST_ASSERT_EQUAL_MEMORY(&hsv, &hsvRandom, sizeof(Ws2812b_HSV_t));
    }
}

// check that setting color type sets last used (set) color correctly
void Test_LastUsedColor(void)
{
    uint32_t idRGB = 8;
    uint32_t idHSV = 10;
    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + idRGB, (Ws2812b_RGB_t){10, 10, 10});
    SetDiodeColorHSV(GetDiodesArray(deviceDriver) + idHSV, (Ws2812b_HSV_t){15, 15, 15});
   
    TEST_ASSERT_EQUAL_INT(RGB, GetDiodesArray(deviceDriver)[idRGB].diodeColor.lastColor);
    TEST_ASSERT_EQUAL_INT(HSV, GetDiodesArray(deviceDriver)[idHSV].diodeColor.lastColor);

    SetDiodeColorRGB(GetDiodesArray(deviceDriver) + idHSV, (Ws2812b_RGB_t){10, 10, 10});
    TEST_ASSERT_EQUAL_INT(RGB, GetDiodesArray(deviceDriver)[idHSV].diodeColor.lastColor);
}

// Check that after init diodesArray members are set properly
void Test_CheckThatAfterInitDeviceDriverIsSetProperly(void)
{
    Ws2812b_Color_t colorAfterInit = {{0, 0, 0}, {0, 0, 0}, RGB};
    Ws2812b_Diode_t* diodesArr = GetDiodesArray(deviceDriver);

    TEST_ASSERT_NOT_NULL(GetDeviceBuffer(deviceDriver));

    
    TEST_ASSERT_EQUAL_MEMORY(&colorAfterInit, &diodesArr[0].diodeColor, sizeof(Ws2812b_Color_t));
}

// Check that sectors pointers (and interface) are set correctly
void Test_CheckSectorsPointers(void)
{
    Ws2812b_Sector_t* sectors = GetSectors(deviceDriver);
    Ws2812b_Diode_t* diodes = GetDiodesArray(deviceDriver);

    // after init there is only one active sector (with id 0 and from 0 to WS2812B_DIODES - 1))
    TEST_ASSERT_EQUAL_PTR(diodes, sectors[0].firstDiode);
    TEST_ASSERT_EQUAL_PTR(diodes + WS2812B_DIODES - 1, sectors[0].lastDiode);
}

void Test_CheckThatNextPointersAreOk(void)
{
    Ws2812b_Sector_t* sectors = GetSectors(deviceDriver);
    Ws2812b_Diode_t* diodes = GetDiodesArray(deviceDriver);

    RemoveSector(deviceDriver, 0);
    SetSector(deviceDriver, 0, 5, 15);
    SetSector(deviceDriver, 1, 17, 25);
    SetSector(deviceDriver, 2, 27, 4);

    // check for sector 0 if the pointers are good
    TEST_ASSERT_EQUAL_PTR(diodes + 5, sectors[0].firstDiode);
    TEST_ASSERT_EQUAL_PTR(diodes + 15, sectors[0].lastDiode);
    for (uint8_t i = 5; i < 15; i++)
    {
        TEST_ASSERT_EQUAL_PTR(diodes + i + 1, diodes[i].next);
    }
    TEST_ASSERT_EQUAL_PTR(diodes + 5, diodes[15].next);

    // check for sector 1 if the pointers are good
    TEST_ASSERT_EQUAL_PTR(diodes + 17, sectors[1].firstDiode);
    TEST_ASSERT_EQUAL_PTR(diodes + 25, sectors[1].lastDiode);
    for (uint8_t i = 17; i < 25; i++)
    {
        TEST_ASSERT_EQUAL_PTR(diodes + i + 1, diodes[i].next);
    }
    TEST_ASSERT_EQUAL_PTR(diodes + 17, diodes[25].next);

    // check for sector 2 if the pointers are good
    TEST_ASSERT_EQUAL_PTR(diodes + 27, sectors[2].firstDiode);
    TEST_ASSERT_EQUAL_PTR(diodes + 4, sectors[2].lastDiode);
    for (uint8_t i = 27; i < WS2812B_DIODES - 1; i++)
    {
        TEST_ASSERT_EQUAL_PTR(diodes + i + 1, diodes[i].next);
    }
    TEST_ASSERT_EQUAL_PTR(diodes, diodes[WS2812B_DIODES - 1].next);
    for (uint8_t i = 0; i < 4; i++)
    {
        TEST_ASSERT_EQUAL_PTR(diodes + i + 1, diodes[i].next);
    }
    TEST_ASSERT_EQUAL_PTR(diodes + 27, diodes[4].next);
}
