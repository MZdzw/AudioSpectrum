#include <stdbool.h>
#include "unity.h"
#include "spiBuffer.h"
#include <stdio.h>
#include <string.h>

SpiWs2812B_t* deviceDriver;

uint8_t expectedArray[WS2812B_DIODES * 
                      SPI_BYTES_PER_WS2812B_BIT * 8 * 3];
/* Is run before every test, put unit init calls here. */
void setUp (void) 
{
    deviceDriver = initObject();
    memset(expectedArray, 0, sizeof(expectedArray));
}

void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

// Check if buffer initialized with zeros
void test_ArrayInitializedWithZeros(void)
{
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, getSpiBufferPointer(deviceDriver), sizeof(expectedArray));
}

// Check if getting value at specific index is the same as set
void test_GettingSameValueAsSet(void)
{
    uint8_t setValue = 4;
    uint32_t setId = 5;
    setSpiBufferElement(deviceDriver, setValue, setId);

    TEST_ASSERT_EQUAL_UINT8(setValue, getSpiBufferElement(deviceDriver, setId));
}

/* Below testcases aren't required because these scenarios will be handled in "higher" modules */

/********************************************************
// Check if setting value which is out of range of buffer size
// is not permitted (e.g. should do nothing)
void test_SetValueToBufferOutOfRangeDoesNothing(void)
{
    setSpiBufferElement(deviceDriver, 9, -1);
    TEST_ASSERT_EQUAL_UINT32(1, outOfRangeAccess);

    setSpiBufferElement(deviceDriver, 9, WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3);
    TEST_ASSERT_EQUAL_UINT32(1, outOfRangeAccess);
}

// Check if setting value within array bound is successfull
void test_SetValueToBufferWithinRangeSuccess(void)
{
    setSpiBufferElement(deviceDriver, 9, 0);
    TEST_ASSERT_EQUAL_UINT32(0, outOfRangeAccess);

    setSpiBufferElement(deviceDriver, 9, WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3 - 1);
    TEST_ASSERT_EQUAL_UINT32(0, outOfRangeAccess);
}

// Check if trying to get value out of array range leads to not successfull reading
void test_TryingToGetValueOutOfArrayRange(void)
{
    uint8_t outputData = getSpiBufferElement(deviceDriver, WS2812B_DIODES * SPI_BYTES_PER_WS2812B_BIT * 8 * 3);
    TEST_ASSERT_EQUAL_UINT8(0, getReadValueStatus(deviceDriver));

    outputData = getSpiBufferElement(deviceDriver, -1);
    TEST_ASSERT_EQUAL_UINT8(0, getReadValueStatus(deviceDriver));
}

// Check if trying to get value within array range leads to successfull reading
void test_TryingToGetValueWithinArrayRange(void)
{
    uint8_t outputData = getSpiBufferElement(deviceDriver, 0);
    TEST_ASSERT_EQUAL_UINT8(1, getReadValueStatus(deviceDriver));
}
********************************************************/
