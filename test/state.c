#include "nhmm.h"
#include "unity.h"
#include <math.h>
#include <stdlib.h>

void setUp(void) {}

void tearDown(void) {}

void test_state_normal(void);
void test_state_silent(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_state_normal);
    RUN_TEST(test_state_silent);
    return UNITY_END();
}

void test_state_normal(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    double *emission_lprobs = malloc(4 * sizeof(double));
    emission_lprobs[0] = log(0.25);
    emission_lprobs[1] = log(0.25);
    emission_lprobs[2] = log(0.5);
    emission_lprobs[3] = -INFINITY;

    struct nhmm_state *s = nhmm_state_create_normal("State0", a, emission_lprobs);

    TEST_ASSERT_EQUAL_STRING("State0", nhmm_state_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), nhmm_state_emission_lprob(s, "A", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), nhmm_state_emission_lprob(s, "C", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5), nhmm_state_emission_lprob(s, "G", 1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_state_emission_lprob(s, "T", 1));

    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(nhmm_state_alphabet(s)));

    nhmm_state_set_end_state(s, true);

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
}

void test_state_silent(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    struct nhmm_state *s = nhmm_state_create_silent("State0", a);

    TEST_ASSERT_EQUAL_STRING("State0", nhmm_state_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, nhmm_state_emission_lprob(s, "", 0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_state_emission_lprob(s, "A", 1));

    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(nhmm_state_alphabet(s)));

    nhmm_state_set_end_state(s, true);

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
}
