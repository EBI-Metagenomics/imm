#include "nhmm.h"
#include "unity.h"
#include <math.h>
#include <stdlib.h>

void test_state_normal(void);
void test_state_silent(void);
void test_state_frame(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_state_normal);
    RUN_TEST(test_state_silent);
    RUN_TEST(test_state_frame);
    return UNITY_END();
}

void test_state_normal(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct nhmm_state *s = nhmm_state_create_normal("State0", a, lprobs);

    TEST_ASSERT_EQUAL_STRING("State0", nhmm_state_get_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), nhmm_state_emiss_lprob(s, "A", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), nhmm_state_emiss_lprob(s, "C", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5), nhmm_state_emiss_lprob(s, "G", 1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_state_emiss_lprob(s, "T", 1));

    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(nhmm_state_get_alphabet(s)));

    nhmm_state_set_end_state(s, true);

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
}

void test_state_silent(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    struct nhmm_state *s = nhmm_state_create_silent("State0", a);

    TEST_ASSERT_EQUAL_STRING("State0", nhmm_state_get_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, nhmm_state_emiss_lprob(s, "", 0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_state_emiss_lprob(s, "A", 1));

    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(nhmm_state_get_alphabet(s)));

    nhmm_state_set_end_state(s, true);

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
}

void test_state_frame(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");
    const int A = 0;
    const int C = 1;
    const int G = 2;
    const int T = 3;

    double base_emiss_lprobs[] = {log(0.25), log(0.25), log(0.25), log(0.25)};

    struct nhmm_codon *codon = nhmm_codon_create();
    nhmm_codon_set_ninfs(codon);
    nhmm_codon_set_lprob(codon, A, A, A, log(0.5));
    nhmm_codon_set_lprob(codon, A, G, A, log(0.4));
    nhmm_codon_set_lprob(codon, C, G, T, log(0.1));

    struct nhmm_state *s = nhmm_state_create_frame("State", a, base_emiss_lprobs,
                                                   nhmm_codon_get_lprobs(codon), 0.0);

    TEST_ASSERT_EQUAL_DOUBLE(0.0, exp(nhmm_state_emiss_lprob(s, "", 0)));
    TEST_ASSERT_EQUAL_DOUBLE(0.4, exp(nhmm_state_emiss_lprob(s, "AGA", 3)));
    /* TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_state_emiss_lprob(s, "A", 1)); */

    /* TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(nhmm_state_get_alphabet(s))); */

    /* nhmm_state_set_end_state(s, true); */

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
    nhmm_codon_destroy(codon);
}
