#include "unity.h"

// void setUp (void) {} /* Is run before every test, put unit init calls here. */
// void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

// void test_template(void)
// {
    
// }
extern void test_template(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_template);

    return UNITY_END();
}