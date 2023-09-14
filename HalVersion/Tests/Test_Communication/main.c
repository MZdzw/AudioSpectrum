#include "unity.h"

extern void Test_ObjectNotNullPtr(void);
extern void Test_CheckAfterInitAllMembersAreZero(void);
extern void Test_DecodingMessages(void);
extern void Test_USBSetDiodeColorHSVAndRGB(void);
extern void Test_WrongMessageAppendix(void);
extern void Test_WrongMessagePrefix(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(Test_ObjectNotNullPtr);
    RUN_TEST(Test_CheckAfterInitAllMembersAreZero);
    RUN_TEST(Test_DecodingMessages);
    RUN_TEST(Test_USBSetDiodeColorHSVAndRGB);
    RUN_TEST(Test_WrongMessageAppendix);
    RUN_TEST(Test_WrongMessagePrefix);

    return UNITY_END();
}