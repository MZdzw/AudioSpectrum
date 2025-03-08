#include "unity.h"

extern void test_ArrayInitializedWithZeros(void);
extern void test_ReadValueStatusAfterInitialize(void);
extern void test_GettingSameValueAsSet(void);
extern void test_SetValueToBufferOutOfRangeDoesNothing(void);
extern void test_SetValueToBufferWithinRangeSuccess(void);
extern void test_TryingToGetValueOutOfArrayRange(void);
extern void test_TryingToGetValueWithinArrayRange(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_ArrayInitializedWithZeros);
    RUN_TEST(test_GettingSameValueAsSet);

    return UNITY_END();
}