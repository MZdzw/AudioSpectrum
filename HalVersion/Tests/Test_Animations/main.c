#include "unity.h"

extern void Test_ObjectNotNullPtr(void);
extern void Test_DriverNotNull(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(Test_ObjectNotNullPtr);
    RUN_TEST(Test_DriverNotNull);

    return UNITY_END();
}