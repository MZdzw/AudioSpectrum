#include "unity.h"

extern void test_ObjectNotNullPtr(void);
extern void test_DiodesArrayClearAfterInit(void);
extern void test_SettingAndGettingTheSameValue(void);
extern void test_DeviceBufferNotNullAfterInit(void);
extern void test_DeviceBufferFilledCorectly(void);
extern void test_DeviceBufferFirstElementFilledCorectly(void);
extern void test_DeviceBufferLastElementFilledCorectly(void);
extern void Test_CheckIfSectorsAreWellInitialized(void);
extern void Test_CheckIfSectorRemoveWorks(void);
extern void Test_CheckIfSectorRemoveOfNotUsedSectorFails(void);
extern void Test_CheckRemoveDefaultSectorAndThenAddTwoNoOverlapping(void);
extern void Test_CheckOverlappingSectors(void);
extern void Test_CreateSectorOnEdge(void);
extern void Test_CreateSectorOnEdgeAndAddNonOverlappingSectors(void);
extern void Test_CreateSectorOnEdgeAndAddOverlappingSector(void);
extern void Test_CreateSectorOnEdgeAndAddAnotherSectorOnEdge(void);
extern void Test_CreateSectorAndAddSectorInsideAndAddSectorOutside(void);
extern void Test_CreateSectorOutOfRange(void);
extern void Test_CreateSectorWithDiodesOfMinusIndexes(void);
extern void Test_SettingHSVSetsRGBCorrectly(void);
extern void Test_GettingHSVfromRGB(void);
extern void Test_SettingRandomHSVAndGettingHSVBack(void);
extern void Test_LastUsedColor(void);
extern void Test_CheckThatAfterInitDeviceDriverIsSetProperly(void);
extern void Test_CheckSectorsPointers(void);
extern void Test_CheckThatNextPointersAreOk(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_ObjectNotNullPtr);
    RUN_TEST(test_DiodesArrayClearAfterInit);
    RUN_TEST(test_SettingAndGettingTheSameValue);
    RUN_TEST(test_DeviceBufferNotNullAfterInit);
    RUN_TEST(test_DeviceBufferFilledCorectly);
    RUN_TEST(test_DeviceBufferFirstElementFilledCorectly);
    RUN_TEST(test_DeviceBufferLastElementFilledCorectly);
    RUN_TEST(Test_CheckIfSectorsAreWellInitialized);
    RUN_TEST(Test_CheckIfSectorRemoveWorks);
    RUN_TEST(Test_CheckIfSectorRemoveOfNotUsedSectorFails);
    RUN_TEST(Test_CheckRemoveDefaultSectorAndThenAddTwoNoOverlapping);
    RUN_TEST(Test_CheckOverlappingSectors);
    RUN_TEST(Test_CreateSectorOnEdge);
    RUN_TEST(Test_CreateSectorOnEdgeAndAddNonOverlappingSectors);   
    RUN_TEST(Test_CreateSectorOnEdgeAndAddOverlappingSector);
    RUN_TEST(Test_CreateSectorOnEdgeAndAddAnotherSectorOnEdge);
    RUN_TEST(Test_CreateSectorAndAddSectorInsideAndAddSectorOutside);
    RUN_TEST(Test_CreateSectorOutOfRange);
    RUN_TEST(Test_CreateSectorWithDiodesOfMinusIndexes);
    RUN_TEST(Test_SettingHSVSetsRGBCorrectly);
    RUN_TEST(Test_GettingHSVfromRGB);
    RUN_TEST(Test_SettingRandomHSVAndGettingHSVBack);
    RUN_TEST(Test_LastUsedColor);
    RUN_TEST(Test_CheckThatAfterInitDeviceDriverIsSetProperly);
    RUN_TEST(Test_CheckSectorsPointers);
    RUN_TEST(Test_CheckThatNextPointersAreOk);

    return UNITY_END();
}