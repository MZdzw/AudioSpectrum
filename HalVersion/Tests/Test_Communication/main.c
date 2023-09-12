#include "unity.h"

extern void Test_ObjectNotNullPtr(void);
extern void Test_CheckAfterInitAllMembersAreZero(void);
extern void Test_DecodingMessages(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(Test_ObjectNotNullPtr);
    RUN_TEST(Test_CheckAfterInitAllMembersAreZero);
    RUN_TEST(Test_DecodingMessages);

    return UNITY_END();
}