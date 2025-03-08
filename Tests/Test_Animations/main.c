#include "unity.h"

extern void Test_ObjectNotNullPtr(void);
extern void Test_DriverNotNull(void);
extern void Test_DimmingAscending(void);
extern void Test_DimmingDescending(void);
extern void Test_Rolling(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(Test_ObjectNotNullPtr);
    RUN_TEST(Test_DriverNotNull);
    RUN_TEST(Test_DimmingAscending);
    // RUN_TEST(Test_DimmingDescending);
    RUN_TEST(Test_Rolling);

    return UNITY_END();
}