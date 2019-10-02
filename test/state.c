#include "imm.h"
#include "unity.h"
#include <math.h>
#include <stdlib.h>

void test_state_normal(void);
void test_state_silent(void);
void test_state_frame1(void);
void test_state_frame2(void);
void test_state_frame3(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_state_normal);
    RUN_TEST(test_state_silent);
    RUN_TEST(test_state_frame1);
    RUN_TEST(test_state_frame2);
    RUN_TEST(test_state_frame3);
    return UNITY_END();
}

void test_state_normal(void)
{
    struct imm_alphabet *a = imm_alphabet_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_state *s = imm_state_create_normal("State0", a, lprobs);

    TEST_ASSERT_EQUAL_STRING("State0", imm_state_get_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), imm_state_emiss_lprob(s, "A", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), imm_state_emiss_lprob(s, "C", 1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5), imm_state_emiss_lprob(s, "G", 1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_state_emiss_lprob(s, "T", 1));

    imm_state_set_end_state(s, true);

    imm_state_destroy(s);
    imm_alphabet_destroy(a);
}

void test_state_silent(void)
{
    struct imm_alphabet *a = imm_alphabet_create("ACGT");

    struct imm_state *s = imm_state_create_silent("State0", a);

    TEST_ASSERT_EQUAL_STRING("State0", imm_state_get_name(s));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, imm_state_emiss_lprob(s, "", 0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_state_emiss_lprob(s, "A", 1));

    imm_state_set_end_state(s, true);

    imm_state_destroy(s);
    imm_alphabet_destroy(a);
}

void test_state_frame1(void)
{
    struct imm_alphabet *a = imm_alphabet_create("ACGT");
    const int A = 0;
    const int G = 2;
    const int T = 3;

    double base_emiss_lprobs[] = {log(0.2), log(0.2), log(0.2), log(0.2)};

    struct imm_codon *codon = imm_codon_create();
    imm_codon_set_ninfs(codon);

    imm_codon_set_lprob(codon, A, T, G, log(0.8 / 0.9));
    imm_codon_set_lprob(codon, A, T, T, log(0.1 / 0.9));

    struct imm_state *s =
        imm_state_create_frame("State", a, base_emiss_lprobs, codon, 0.1);
    imm_state_normalize(s);

    TEST_ASSERT_EQUAL_DOUBLE(-5.914503505971854, imm_state_emiss_lprob(s, "A", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-2.915843423869834, imm_state_emiss_lprob(s, "AT", 2));
    TEST_ASSERT_EQUAL_DOUBLE(-6.905597115665666, imm_state_emiss_lprob(s, "ATA", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-0.534773288204706, imm_state_emiss_lprob(s, "ATG", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-2.590237330499946, imm_state_emiss_lprob(s, "ATT", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-6.881032208841384, imm_state_emiss_lprob(s, "ATTA", 4));
    TEST_ASSERT_EQUAL_DOUBLE(-12.08828960987379, imm_state_emiss_lprob(s, "ATTAA", 5));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_state_emiss_lprob(s, "ATTAAT", 6));

    imm_state_destroy(s);
    imm_alphabet_destroy(a);
    imm_codon_destroy(codon);
}

void test_state_frame2(void)
{
    struct imm_alphabet *a = imm_alphabet_create("ACGT");
    const int A = 0;
    const int G = 2;
    const int T = 3;

    double base_emiss_lprobs[] = {log(0.1), log(0.2), log(0.3), log(0.4)};

    struct imm_codon *codon = imm_codon_create();
    imm_codon_set_ninfs(codon);

    imm_codon_set_lprob(codon, A, T, G, log(0.8 / 0.9));
    imm_codon_set_lprob(codon, A, T, T, log(0.1 / 0.9));

    struct imm_state *s =
        imm_state_create_frame("State", a, base_emiss_lprobs, codon, 0.1);
    imm_state_normalize(s);

    TEST_ASSERT_EQUAL_DOUBLE(-5.914503505971854, imm_state_emiss_lprob(s, "A", 1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_state_emiss_lprob(s, "C", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-6.032286541628237, imm_state_emiss_lprob(s, "G", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-5.809142990314027, imm_state_emiss_lprob(s, "T", 1));

    TEST_ASSERT_EQUAL_DOUBLE(-2.9159357500274385, imm_state_emiss_lprob(s, "AT", 2));
    TEST_ASSERT_EQUAL_DOUBLE(-7.821518343902165, imm_state_emiss_lprob(s, "ATA", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-0.5344319079005616, imm_state_emiss_lprob(s, "ATG", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-7.129480084106424, imm_state_emiss_lprob(s, "ATC", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-2.57514520832882, imm_state_emiss_lprob(s, "ATT", 3));

    TEST_ASSERT_EQUAL_DOUBLE(-7.789644584138959, imm_state_emiss_lprob(s, "ATTA", 4));
    TEST_ASSERT_EQUAL_DOUBLE(-5.036637096635257, imm_state_emiss_lprob(s, "ACTG", 4));

    TEST_ASSERT_EQUAL_DOUBLE(-13.920871073622099,
                             imm_state_emiss_lprob(s, "ATTAA", 5));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_state_emiss_lprob(s, "ATTAAT", 6));

    imm_state_destroy(s);
    imm_alphabet_destroy(a);
    imm_codon_destroy(codon);
}

void test_state_frame3(void)
{
    struct imm_alphabet *a = imm_alphabet_create("ACGT");
    const int A = 0;
    const int C = 1;
    const int G = 2;
    const int T = 3;

    double base_emiss_lprobs[] = {log(0.1), log(0.2), log(0.3), log(0.4)};

    struct imm_codon *codon = imm_codon_create();
    imm_codon_set_ninfs(codon);

    imm_codon_set_lprob(codon, A, T, G, log(0.8));
    imm_codon_set_lprob(codon, A, T, T, log(0.1));
    imm_codon_set_lprob(codon, G, T, C, log(0.4));

    TEST_ASSERT_EQUAL_INT(0, imm_codon_normalize(codon));

    struct imm_state *s =
        imm_state_create_frame("State", a, base_emiss_lprobs, codon, 0.1);
    imm_state_normalize(s);

    TEST_ASSERT_EQUAL_DOUBLE(-6.282228286097171, imm_state_emiss_lprob(s, "A", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-7.0931585023135, imm_state_emiss_lprob(s, "C", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-5.99454621364539, imm_state_emiss_lprob(s, "G", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-5.840395533818132, imm_state_emiss_lprob(s, "T", 1));
    TEST_ASSERT_EQUAL_DOUBLE(-3.283414346005771, imm_state_emiss_lprob(s, "AT", 2));
    TEST_ASSERT_EQUAL_DOUBLE(-9.395743595307545, imm_state_emiss_lprob(s, "CG", 2));
    TEST_ASSERT_EQUAL_DOUBLE(-8.18911998648269, imm_state_emiss_lprob(s, "ATA", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-0.9021560981322401, imm_state_emiss_lprob(s, "ATG", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-2.9428648000333952, imm_state_emiss_lprob(s, "ATT", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-7.314811395663229, imm_state_emiss_lprob(s, "ATC", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-1.5951613351178675, imm_state_emiss_lprob(s, "GTC", 3));
    TEST_ASSERT_EQUAL_DOUBLE(-8.157369364264277, imm_state_emiss_lprob(s, "ATTA", 4));
    TEST_ASSERT_EQUAL_DOUBLE(-4.711642430498609, imm_state_emiss_lprob(s, "GTTC", 4));
    TEST_ASSERT_EQUAL_DOUBLE(-5.404361876760574, imm_state_emiss_lprob(s, "ACTG", 4));
    TEST_ASSERT_EQUAL_DOUBLE(-14.288595853747417,
                             imm_state_emiss_lprob(s, "ATTAA", 5));
    TEST_ASSERT_EQUAL_DOUBLE(-12.902301492627526,
                             imm_state_emiss_lprob(s, "GTCAA", 5));

    imm_state_destroy(s);
    imm_alphabet_destroy(a);
    imm_codon_destroy(codon);
}
