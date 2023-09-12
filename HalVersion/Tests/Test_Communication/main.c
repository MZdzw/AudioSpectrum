#include "unity.h"

extern void Test_ObjectNotNullPtr(void);
extern void Test_CheckAfterInitAllMembersAreZero(void);
extern void Test_DecodingMessages(void);
extern void Test_USBSetDiodeColorHSVAndRGB(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(Test_ObjectNotNullPtr);
    RUN_TEST(Test_CheckAfterInitAllMembersAreZero);
    RUN_TEST(Test_DecodingMessages);
    RUN_TEST(Test_USBSetDiodeColorHSVAndRGB);

    return UNITY_END();
}