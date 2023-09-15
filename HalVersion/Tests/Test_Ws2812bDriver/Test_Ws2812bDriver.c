#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "math.h"
#include "unity.h"
#include "ws2812bDriver.h"
#include "mock_stm32f4xx_hal_spi.h"

Ws2812b_Driver_t* deviceDriver;

Ws2812b_RGB_t expectedArray[WS2812B_DIODES];

void setUp (void) /* Is run before every test, put unit init calls here. */
{
    deviceDriver = Ws2812b_initObject();
}
void tearDown (void) /* Is run after every test, put unit clean-up calls here. */
{
    // clear DiodeColors array
    for (uint32_t i = 0; i < WS2812B_DIODES; i++)
    {
        SetDiodeColorRGB(deviceDriver, i, 0, 0, 0);
    }
    // clear spiBuffer after each test
    for (uint32_t i = 0; i < WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144; i++)
    {
        setSpiBufferElement(GetDeviceBuffer(deviceDriver), 0, i);
    }
    // clear sectors
    Ws2812b_Sector_t sector = {0, 0, false};
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
void test_DiodeColorsArrayClearAfterInit(void)
{
    for (uint32_t i = 0; i < WS2812B_DIODES; i++)
    {
        expectedArray[i].red = 0;
        expectedArray[i].green = 0;
        expectedArray[i].blue = 0;
    }
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedArray, GetDiodeColorsArray(deviceDriver), sizeof(Ws2812b_RGB_t), WS2812B_DIODES);
}

// Check that after setting, will get the same value
void test_SettingAndGettingTheSameValue(void)
{
    uint32_t id = 4;
    uint8_t redExpected = 90;
    uint8_t greenExpected = 122;
    uint8_t blueExpected = 140;
    SetDiodeColorRGB(deviceDriver, id, redExpected, greenExpected, blueExpected);
    uint8_t red, green, blue;
    GetDiodeColorRGB(deviceDriver, id, &red, &green, &blue);
    
    TEST_ASSERT_EQUAL_UINT8(redExpected, red);
    TEST_ASSERT_EQUAL_UINT8(greenExpected, green);
    TEST_ASSERT_EQUAL_UINT8(blueExpected, blue);
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
    HAL_SPI_Transmit_DMA_IgnoreAndReturn(0);    // 0 means return OK
    uint32_t id = 4;
    uint8_t red = 192;
    uint8_t green = 0;
    uint8_t blue = 0;
    SetDiodeColorRGB(deviceDriver, id, red, green, blue);
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
    HAL_SPI_Transmit_DMA_IgnoreAndReturn(0);    // 0 means return OK
    uint32_t id = 0;
    uint8_t red = 143;
    uint8_t green = 189;
    uint8_t blue = 40;
    SetDiodeColorRGB(deviceDriver, id, red, green, blue);
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
    HAL_SPI_Transmit_DMA_IgnoreAndReturn(0);    // 0 means return OK
    uint32_t id = WS2812B_DIODES - 1;
    uint8_t red = 200;
    uint8_t green = 222;
    uint8_t blue = 153;
    SetDiodeColorRGB(deviceDriver, id, red, green, blue);
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
    TEST_ASSERT_EQUAL_UINT32(0, sector[0].startDiode);
    TEST_ASSERT_EQUAL_UINT32(WS2812B_DIODES - 1, sector[0].endDiode);
    TEST_ASSERT_TRUE(sector[0].isUsed);
    for (uint8_t i = 1; i < MAX_SECTORS; i++)
    {
        TEST_ASSERT_EQUAL_UINT32(0, sector[i].startDiode);
        TEST_ASSERT_EQUAL_UINT32(0, sector[i].endDiode);
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
    uint8_t redExpected = 61;
    uint8_t greenExpected = 102;
    uint8_t blueExpected = 41;

    SetDiodeColorHSV(deviceDriver, id, 100, 60, 40);
    uint8_t red, green, blue;
    GetDiodeColorRGB(deviceDriver, id, &red, &green, &blue);

    
    TEST_ASSERT_EQUAL_UINT8(redExpected, red);
    TEST_ASSERT_EQUAL_UINT8(greenExpected, green);
    TEST_ASSERT_EQUAL_UINT8(blueExpected, blue);

    SetDiodeColorHSV(deviceDriver, id, 285, 53, 6);
    redExpected = 13;
    greenExpected = 7;
    blueExpected = 15;
    GetDiodeColorRGB(deviceDriver, id, &red, &green, &blue);
    TEST_ASSERT_EQUAL_UINT8(redExpected, red);
    TEST_ASSERT_EQUAL_UINT8(greenExpected, green);
    TEST_ASSERT_EQUAL_UINT8(blueExpected, blue);
}

// Check getter of HSV (check if converse from RGB to HSV is correct)
void Test_GettingHSVfromRGB(void)
{
    uint32_t id = 8;
    uint8_t redSet = 61;
    uint8_t greenSet = 102;
    uint8_t blueSet = 40;

    SetDiodeColorRGB(deviceDriver, id, redSet, greenSet, blueSet);
    uint8_t red, green, blue;
    GetDiodeColorRGB(deviceDriver, id, &red, &green, &blue);
    TEST_ASSERT_EQUAL_UINT8(redSet, red);
    TEST_ASSERT_EQUAL_UINT8(greenSet, green);
    TEST_ASSERT_EQUAL_UINT8(blueSet, blue);
    // getting HSV
    uint16_t hue;
    uint8_t saturation, value;
    uint16_t hueExpected = 100;
    uint8_t saturationExpected = 61;
    uint8_t valueExpected = 40;
    GetDiodeColorHSV(deviceDriver, id, &hue, &saturation, &value);
    TEST_ASSERT_EQUAL_UINT16(hueExpected, hue);
    TEST_ASSERT_EQUAL_UINT8(saturationExpected, saturation);
    TEST_ASSERT_EQUAL_UINT8(valueExpected, value);
}

// Check setting random HSV and getting HSV (it checks conversion between HSV-RGB and RGB-HSV)
void Test_SettingRandomHSVAndGettingHSVBack(void)
{
    srand(time(NULL));
    uint32_t id = 8;
    uint16_t randomHue;
    uint8_t randomSaturation;
    uint8_t randomValue;

    uint16_t hue;
    uint8_t saturation, value;
    
    for (unsigned int i = 0; i < 100; i++)
    {
        randomHue = rand() % 361;
        randomSaturation = rand() % 101;
        randomValue = rand() % 101;
        SetDiodeColorHSV(deviceDriver, id, randomHue, randomSaturation, randomValue);
        GetDiodeColorHSV(deviceDriver, id, &hue, &saturation, &value);
        TEST_ASSERT_EQUAL_UINT16(randomHue, hue);
        TEST_ASSERT_EQUAL_UINT8(randomSaturation, saturation);
        TEST_ASSERT_EQUAL_UINT8(randomValue, value);
    }
}

// check that setting color type sets last used (set) color correctly
void Test_LastUsedColor(void)
{
    uint32_t idRGB = 8;
    uint32_t idHSV = 10;
    SetDiodeColorRGB(deviceDriver, idRGB, 10, 10, 10);
    SetDiodeColorHSV(deviceDriver, idHSV, 15, 15, 15);
    TEST_ASSERT_EQUAL_INT(RGB, GetDiodeColorsArray(deviceDriver)[idRGB].lastColor);
    TEST_ASSERT_EQUAL_INT(HSV, GetDiodeColorsArray(deviceDriver)[idHSV].lastColor);
    SetDiodeColorRGB(deviceDriver, idHSV, 10, 10, 10);
    TEST_ASSERT_EQUAL_INT(RGB, GetDiodeColorsArray(deviceDriver)[idHSV].lastColor);
}