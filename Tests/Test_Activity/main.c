#include "unity.h"

extern void Test_NoDecodeMessageBefore(void);
extern void Test_CheckThatProperActivityIsSet(void);
extern void Test_RemoveSector(void);
extern void Test_AddSector(void);
extern void Test_AddExisitngSector(void);
extern void Test_AddDiodeOutOfRange(void);
extern void Test_AddOverlappingSector(void);
extern void Test_ActivitySetDiodeColorHSV(void);
extern void Test_ActivitySetDiodeColorHSVDiodeIDOutOfRange(void);
extern void Test_ActivitySetDiodeColorHSVValuesOutOfRange(void);
extern void Test_ActivitySetDiodeColorHSVInNotUsedSector(void);
extern void Test_ActivitySetDiodeColorRGB(void);
extern void Test_ActivitySetDiodeColorRGBDiodeIDOutOfRange(void);
extern void Test_ActivitySetDiodeColorRGBInNotUsedSector(void);
extern void Test_ActivitySetSectorColorHSV(void);
extern void Test_ActivitySetSectorColorHSVSectorIDOutOfRange(void);
extern void Test_ActivitySetSectorColorHSVValuesOutOfRange(void);
extern void Test_ActivitySetSectorColorHSVInNotUsedSector(void);
extern void Test_ActivitySetSectorColorRGB(void);
extern void Test_ActivitySetSectorColorRGBSectorIDOutOfRange(void);
extern void Test_ActivitySetSectorColorRGBInNotUsedSector(void);
extern void Test_ActivitySetSectorColorRainbow(void);
extern void Test_ActivitySetSectorColorRainbowSectorIdOutOfRange(void);
extern void Test_ActivitySetSectorColorRainbowSectorIdNotInUse(void);
extern void Test_ActivitySetSectorSpawnDiode(void);
extern void Test_ActivitySetSectorSpawnDiodeSectorIdOutOfRange(void);
extern void Test_ActivitySetSectorSpawnDiodeSectorIdNotInUse(void);


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(Test_NoDecodeMessageBefore);
    RUN_TEST(Test_CheckThatProperActivityIsSet);
    RUN_TEST(Test_RemoveSector);
    RUN_TEST(Test_AddSector);
    RUN_TEST(Test_AddExisitngSector);
    RUN_TEST(Test_AddDiodeOutOfRange);
    RUN_TEST(Test_AddOverlappingSector);
    RUN_TEST(Test_ActivitySetDiodeColorHSV);
    RUN_TEST(Test_ActivitySetDiodeColorHSVDiodeIDOutOfRange);
    RUN_TEST(Test_ActivitySetDiodeColorHSVValuesOutOfRange);
    RUN_TEST(Test_ActivitySetDiodeColorHSVInNotUsedSector);
    RUN_TEST(Test_ActivitySetDiodeColorRGB);
    RUN_TEST(Test_ActivitySetDiodeColorRGBDiodeIDOutOfRange);
    RUN_TEST(Test_ActivitySetSectorColorHSV);
    RUN_TEST(Test_ActivitySetSectorColorHSVSectorIDOutOfRange);
    RUN_TEST(Test_ActivitySetSectorColorHSVValuesOutOfRange);
    RUN_TEST(Test_ActivitySetDiodeColorRGBInNotUsedSector);
    RUN_TEST(Test_ActivitySetSectorColorHSVInNotUsedSector);
    RUN_TEST(Test_ActivitySetSectorColorRGB);
    RUN_TEST(Test_ActivitySetSectorColorRGBSectorIDOutOfRange);
    RUN_TEST(Test_ActivitySetSectorColorRGBInNotUsedSector);
    RUN_TEST(Test_ActivitySetSectorColorRainbow);
    RUN_TEST(Test_ActivitySetSectorColorRainbowSectorIdOutOfRange);
    RUN_TEST(Test_ActivitySetSectorColorRainbowSectorIdNotInUse);
    RUN_TEST(Test_ActivitySetSectorSpawnDiode);
    RUN_TEST(Test_ActivitySetSectorSpawnDiodeSectorIdOutOfRange);
    RUN_TEST(Test_ActivitySetSectorSpawnDiodeSectorIdNotInUse);

    return UNITY_END();
}