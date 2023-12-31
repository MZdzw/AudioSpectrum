#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "unity.h"
#include "animations.h"
// #include ""

LedStrip_t* ledStrip;

void setUp (void) /* Is run before every test, put unit init calls here. */
{
    ledStrip = LedStrip_initObject();
}
void tearDown (void) /* Is run after every test, put unit clean-up calls here. */
{
    // clear DiodeColors array
    for (uint32_t i = 0; i < WS2812B_DIODES; i++)
    {
        SetDiodeColorRGB(GetDiodesArray(GetDriver(ledStrip)) + i, (Ws2812b_RGB_t){0, 0, 0});
    }
    // clear spiBuffer after each test
    for (uint32_t i = 0; i < WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 + 144; i++)
    {
        setSpiBufferElement(GetDeviceBuffer(GetDriver(ledStrip)), 0, i);
    }
    // clear sectors
    Ws2812b_Sector_t sector = {0, 0, false};
    Ws2812b_Sector_t* sectorsToClear = GetSectors(GetDriver(ledStrip));
    for (uint8_t i = 0; i < MAX_SECTORS; i++)
    {
        sectorsToClear[i] = sector;
    }
    SetActiveSectors(GetDriver(ledStrip), 0);  
}

// check that after init object is not NULL
void Test_ObjectNotNullPtr(void)
{
    TEST_ASSERT_NOT_NULL(ledStrip);
}

// check that driver after init is not NULL
void Test_DriverNotNull(void)
{
    TEST_ASSERT_NOT_NULL(GetDriver(ledStrip));
}

// Check if dimming works good (ascending value)
void Test_DimmingAscending(void)
{
    // set initial color value on led strip
    Ws2812b_Diode_t* diodesArray = GetDiodesArray(GetDriver(ledStrip));
    Ws2812b_HSV_t expectedColorHSV[WS2812B_DIODES] = {{0, 0, 0}};
    for (unsigned int i = 0; i < WS2812B_DIODES; i++)
    {
        SetDiodeColorHSV(GetDiodesArray(GetDriver(ledStrip)) + i, (Ws2812b_HSV_t){270, 50, 20});
        expectedColorHSV[i].hue = 270;
        expectedColorHSV[i].saturation = 50;
        expectedColorHSV[i].value = 20 + 1;
    }
    // at default we have only one sector (id equal 0)
    Animation animation = GetAnimationFunPtr(GetAnimations(ledStrip), 0);
    // SetDimDir(0, ASCENDING);
    animation(GetDriver(ledStrip), 0);
    Ws2812b_HSV_t actualHSVColorS[WS2812B_DIODES];
    for (unsigned int  i = 0; i < WS2812B_DIODES; i++)
    {
        actualHSVColorS[i] = diodesArray[i].diodeColor.hsv;
    }
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedColorHSV, actualHSVColorS, sizeof(Ws2812b_HSV_t), WS2812B_DIODES);
}

// Check if dimming works good (ascending value)
void Test_DimmingDescending(void)
{
    Ws2812b_HSV_t expectedColorHSV[WS2812B_DIODES] = {{0, 0, 0}};
    // set initial color value on led strip
    for (unsigned int i = 0; i < WS2812B_DIODES; i++)
    {
        SetDiodeColorHSV(GetDiodesArray(GetDriver(ledStrip)) + i, (Ws2812b_HSV_t){270, 52, 100});
        expectedColorHSV[i].hue = 270;
        expectedColorHSV[i].saturation = 52;
        expectedColorHSV[i].value = 100 - 1;
    }
    // at default we have only one sector (id equal 0)
    Animation animation = GetAnimationFunPtr(GetAnimations(ledStrip), 0);
    animation(GetDriver(ledStrip), 0);
    Ws2812b_Diode_t* diodesArray = GetDiodesArray(GetDriver(ledStrip));
    Ws2812b_HSV_t actualHSVColorS[WS2812B_DIODES];
    for (unsigned int  i = 0; i < WS2812B_DIODES; i++)
    {
        actualHSVColorS[i] = diodesArray[i].diodeColor.hsv;
    }
    
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedColorHSV, actualHSVColorS, sizeof(Ws2812b_HSV_t), WS2812B_DIODES);
}

// Check Rolling diodes feature
void Test_Rolling(void)
{
    srand(time(NULL));
    Ws2812b_RGB_t expectedColorRGB[WS2812B_DIODES] = {{0, 0, 0}};
    // set initial color value on led strip
    Ws2812b_HSV_t hsv;
    Ws2812b_HSV_t expectedHsv;

    for (unsigned int i = 0; i < WS2812B_DIODES - 2; i++)
    {
        hsv.hue = rand() % 361;
        hsv.saturation = rand() % 101;
        hsv.value = rand() % 101;
        SetDiodeColorHSV(GetDiodesArray(GetDriver(ledStrip)) + i, hsv);
        expectedColorRGB[i + 1] = GetDiodeColorRGB(GetDiodesArray(GetDriver(ledStrip)) + i);
    }
    hsv.hue = rand() % 361;
    hsv.saturation = rand() % 101;
    hsv.value = rand() % 101;
    SetDiodeColorHSV(GetDiodesArray(GetDriver(ledStrip)) + WS2812B_DIODES - 1, hsv);
    expectedColorRGB[0] = GetDiodeColorRGB(GetDiodesArray(GetDriver(ledStrip)) + WS2812B_DIODES - 1);
    // at default we have only one sector (id equal 0)
    SetAnimation(ledStrip, ROLLING, 0);
    Animation animation = GetAnimationFunPtr(GetAnimations(ledStrip), 0);
    animation(GetDriver(ledStrip), 0);
    Ws2812b_Diode_t* diodesArray = GetDiodesArray(GetDriver(ledStrip));
    Ws2812b_RGB_t actualRGBColors[WS2812B_DIODES];
    for (unsigned int  i = 0; i < WS2812B_DIODES; i++)
    {
        actualRGBColors[i] = diodesArray[i].diodeColor.rgb;
    }

    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedColorRGB, actualRGBColors, sizeof(Ws2812b_RGB_t), WS2812B_DIODES);
}
