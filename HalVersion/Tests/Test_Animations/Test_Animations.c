#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "unity.h"
#include "animations.h"

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
        SetDiodeColorRGB(GetDriver(ledStrip), i, 0, 0, 0);
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
    Ws2812b_Color_t* colorsArray = GetDiodeColorsArray(GetDriver(ledStrip));
    Ws2812b_HSV_t expectedColorHSV[WS2812B_DIODES] = {{0, 0, 0}};
    for (unsigned int i = 0; i < WS2812B_DIODES; i++)
    {
        SetDiodeColorHSV(GetDriver(ledStrip), i, 270, 52, 20);
        expectedColorHSV[i].hue = 270;
        expectedColorHSV[i].saturation = 52;
        expectedColorHSV[i].value = 20 + 1;
    }
    // at default we have only one sector (id equal 0)
    Animation animation = GetAnimationFunPtr(GetAnimations(ledStrip), 0);
    SetDimDir(0, ASCENDING);
    animation(GetDriver(ledStrip), 0);
    Ws2812b_HSV_t actualHSVColorS[WS2812B_DIODES];
    for (unsigned int  i = 0; i < WS2812B_DIODES; i++)
    {
        actualHSVColorS[i] = colorsArray[i].hsv;
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
        SetDiodeColorHSV(GetDriver(ledStrip), i, 270, 52, 20);
        expectedColorHSV[i].hue = 270;
        expectedColorHSV[i].saturation = 52;
        expectedColorHSV[i].value = 20 - 1;
    }
    // at default we have only one sector (id equal 0)
    Animation animation = GetAnimationFunPtr(GetAnimations(ledStrip), 0);
    SetDimDir(0, DESCENDING);
    animation(GetDriver(ledStrip), 0);
    Ws2812b_Color_t* colorsArray = GetDiodeColorsArray(GetDriver(ledStrip));
    Ws2812b_HSV_t actualHSVColorS[WS2812B_DIODES];
    for (unsigned int  i = 0; i < WS2812B_DIODES; i++)
    {
        actualHSVColorS[i] = colorsArray[i].hsv;
    }
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedColorHSV, actualHSVColorS, sizeof(Ws2812b_HSV_t), WS2812B_DIODES);
}

// Check Rolling diodes feature
void Test_Rolling(void)
{
    srand(time(NULL));
    Ws2812b_HSV_t expectedColorHSV[WS2812B_DIODES] = {{0, 0, 0}};
    // set initial color value on led strip
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
    for (unsigned int i = 0; i < WS2812B_DIODES - 1; i++)
    {
        hue = rand() % 361;
        saturation = rand() % 101;
        value = rand() % 101;
        SetDiodeColorHSV(GetDriver(ledStrip), i, hue, saturation, value);
        expectedColorHSV[i + 1].hue = hue;
        expectedColorHSV[i + 1].saturation = saturation;
        expectedColorHSV[i + 1].value = value;
    }
    hue = rand() % 361;
    saturation = rand() % 101;
    value = rand() % 101;
    SetDiodeColorHSV(GetDriver(ledStrip), WS2812B_DIODES - 1, hue, saturation, value);
    expectedColorHSV[0].hue = hue;
    expectedColorHSV[0].saturation = saturation;
    expectedColorHSV[0].value = value;
    // at default we have only one sector (id equal 0)
    SetAnimation(ledStrip, ROLLING, 0);
    Animation animation = GetAnimationFunPtr(GetAnimations(ledStrip), 0);
    animation(GetDriver(ledStrip), 0);
    Ws2812b_Color_t* colorsArray = GetDiodeColorsArray(GetDriver(ledStrip));
    Ws2812b_HSV_t actualHSVColorS[WS2812B_DIODES];
    for (unsigned int  i = 0; i < WS2812B_DIODES; i++)
    {
        actualHSVColorS[i] = colorsArray[i].hsv;
    }
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expectedColorHSV, actualHSVColorS, sizeof(Ws2812b_HSV_t), WS2812B_DIODES);
}
