#include <stdbool.h>
#include "unity.h"
#include "animations.h"

LedStrip_t* ledStrip;

void setUp (void) /* Is run before every test, put unit init calls here. */
{
    ledStrip = LedStrip_initObject();
}
void tearDown (void) /* Is run after every test, put unit clean-up calls here. */
{
    
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
