#include "cass.h"
#include "imm/imm.h"

void test_window_0(void);
void test_window_1(void);
void test_window_2(void);
void test_window_3(void);
void test_window_4(void);
void test_window_5(void);
void test_window_6(void);
void test_window_7(void);
void test_window_8(void);
void test_window_9(void);
void test_window_10(void);
void test_window_11(void);
void test_window_12(void);
void test_window_13(void);
void test_window_14(void);
void test_window_15(void);

int main(void)
{
    test_window_0();
    test_window_1();
    test_window_2();
    test_window_3();
    test_window_4();
    test_window_5();
    test_window_6();
    test_window_7();
    test_window_8();
    test_window_9();
    test_window_10();
    test_window_11();
    test_window_12();
    test_window_13();
    test_window_14();
    test_window_15();
    return cass_status();
}

void test_window_0(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("", abc);

    cass_cond(imm_window_create(seq, 0) == NULL);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_1(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("", abc);

    cass_cond(imm_window_create(seq, 1) == NULL);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_2(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("A", abc);

    struct imm_window* window = imm_window_create(seq, 1);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 1);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "A", 1) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_3(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("AC", abc);

    struct imm_window* window = imm_window_create(seq, 1);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 1);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "A", 1) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 1);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "C", 1) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_4(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACT", abc);

    struct imm_window* window = imm_window_create(seq, 1);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 1);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "A", 1) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 1);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "C", 1) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 1);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "T", 1) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_5(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACT", abc);

    struct imm_window* window = imm_window_create(seq, 2);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "AC", 2) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "CT", 2) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_6(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACT", abc);

    struct imm_window* window = imm_window_create(seq, 3);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACT", 3) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_7(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACT", abc);

    struct imm_window* window = imm_window_create(seq, 4);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACT", 3) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_8(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTA", abc);

    struct imm_window* window = imm_window_create(seq, 2);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "AC", 2) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "CT", 2) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "TA", 2) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_9(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTA", abc);

    struct imm_window* window = imm_window_create(seq, 3);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACT", 3) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "CTA", 3) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_10(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTA", abc);

    struct imm_window* window = imm_window_create(seq, 4);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 4);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACTA", 4) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_11(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTAG", abc);

    struct imm_window* window = imm_window_create(seq, 2);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "AC", 2) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "CT", 2) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "TA", 2) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 2);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "AG", 2) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_12(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTAG", abc);

    struct imm_window* window = imm_window_create(seq, 3);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACT", 3) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "CTA", 3) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "TAG", 3) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_13(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTAG", abc);

    struct imm_window* window = imm_window_create(seq, 4);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 4);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACTA", 4) == 0);

    subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 3);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "TAG", 3) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_14(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTAG", abc);

    struct imm_window* window = imm_window_create(seq, 5);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 5);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACTAG", 5) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}

void test_window_15(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 'X');
    struct imm_seq const* seq = imm_seq_create("ACTAG", abc);

    struct imm_window* window = imm_window_create(seq, 6);

    struct imm_subseq subseq = imm_window_next(window);
    cass_cond(imm_seq_length(imm_subseq_cast(&subseq)) == 5);
    cass_cond(strncmp(imm_seq_string(imm_subseq_cast(&subseq)), "ACTAG", 5) == 0);

    cass_cond(imm_window_end(window));

    imm_window_destroy(window);

    imm_seq_destroy(seq);
    imm_abc_destroy(abc);
}
