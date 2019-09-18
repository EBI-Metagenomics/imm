#include "nhmm.h"
#include "unity.h"
#include <math.h>
#include <stdlib.h>

void test_state_normal(void)
{

    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    double *emission = malloc(4 * sizeof(double));
    emission[0] = log(0.25);
    emission[1] = log(0.25);
    emission[2] = log(0.5);
    emission[3] = -INFINITY;

    struct nhmm_state *s = nhmm_state_create_normal("State0", a, emission);

    TEST_ASSERT_EQUAL_STRING("State0", nhmm_state_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), nhmm_state_emission(s, "A", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), nhmm_state_emission(s, "C", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5), nhmm_state_emission(s, "G", 1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_state_emission(s, "T", 1));

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_state_normal);
    return UNITY_END();
}
