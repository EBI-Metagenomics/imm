#include "cass/cass.h"
#include "imm.h"
#include <stdio.h>
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);
void test_hmm_likelihood_mute_state(void);
void test_hmm_viterbi_no_state(void);
void test_hmm_viterbi_mute_states(void);
void test_hmm_viterbi_normal_states(void);
void test_hmm_viterbi_profile1(void);
void test_hmm_viterbi_profile2(void);

int main(void)
{
    test_hmm_state_id();
    test_hmm_del_get_state();
    test_hmm_set_trans();
    test_hmm_likelihood_single_state();
    test_hmm_likelihood_two_states();
    test_hmm_likelihood_mute_state();
    test_hmm_viterbi_no_state();
    test_hmm_viterbi_mute_states();
    test_hmm_viterbi_normal_states();
    test_hmm_viterbi_profile1();
    test_hmm_viterbi_profile2();
    return cass_status();
}

void test_hmm_state_id(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_state *state = imm_mute_state_create("State0", abc);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    cass_condition(imm_hmm_add_state(hmm, state, log(1.0)) == 0);
    cass_condition(imm_hmm_add_state(hmm, state, log(1.0)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_del_get_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_state *state0 = imm_mute_state_create("State0", abc);
    struct imm_state *state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    cass_condition(imm_hmm_add_state(hmm, state0, log(0.5)) == 0);
    cass_condition(imm_hmm_add_state(hmm, state1, log(0.5)) == 0);
    cass_condition(imm_hmm_add_state(hmm, NULL, log(0.5)) == 1);

    cass_condition(isnan(imm_hmm_get_trans(hmm, state0, NULL)));
    cass_condition(isnan(imm_hmm_get_trans(hmm, NULL, state0)));
    cass_condition(isnan(imm_hmm_get_trans(hmm, NULL, NULL)));

    cass_condition(imm_hmm_del_state(hmm, state0) == 0);
    cass_condition(imm_hmm_del_state(hmm, state1) == 0);

    cass_condition(imm_hmm_del_state(hmm, state0) == 1);
    cass_condition(imm_hmm_del_state(hmm, state1) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_set_trans(void)
{

    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_state *state0 = imm_mute_state_create("State0", abc);
    struct imm_state *state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, state0, log(0.5));
    imm_hmm_add_state(hmm, state1, log(0.5));

    cass_condition(imm_hmm_set_trans(hmm, state0, NULL, log(0.5)) == 1);
    cass_condition(imm_hmm_set_trans(hmm, NULL, state1, log(0.5)) == 1);
    cass_condition(imm_hmm_set_trans(hmm, state0, state1, log(0.5)) == 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_single_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};

    struct imm_state *state = imm_normal_state_create("State0", abc, lprobs);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, state, log(0.5));
    cass_condition(imm_hmm_normalize(hmm) == 1);

    struct imm_path *path = NULL;
    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_close(imm_hmm_likelihood(hmm, "A", path), -1.386294361120);
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 2);
    cass_condition(isnan(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(isnan(imm_hmm_likelihood(hmm, "AG", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "H", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, NULL, 1);
    cass_condition(isnan(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "AA", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(isnan(imm_hmm_likelihood(hmm, NULL, path)));
    imm_path_destroy(&path);

    cass_condition(isnan(imm_hmm_likelihood(hmm, "A", NULL)));
    cass_condition(isnan(imm_hmm_likelihood(hmm, NULL, NULL)));

    cass_condition(imm_hmm_normalize(hmm) == 1);
    imm_hmm_set_trans(hmm, state, state, -INFINITY);
    cass_condition(imm_hmm_normalize(hmm) == 1);
    imm_hmm_set_trans(hmm, state, state, log(0.5));

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    imm_path_add(&path, state, 1);
    cass_close(imm_hmm_likelihood(hmm, "AA", path), -3.465735902800);
    imm_path_destroy(&path);

    cass_condition(imm_hmm_normalize(hmm) == 0);
    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    imm_path_add(&path, state, 1);
    cass_close(imm_hmm_likelihood(hmm, "AA", path), -2.772588722240);
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_two_states(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    double lprobs0[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_state *state0 = imm_normal_state_create("State0", abc, lprobs0);

    double lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_state *state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, state0, log(1.0));
    imm_hmm_add_state(hmm, state1, -INFINITY);

    imm_hmm_set_trans(hmm, state0, state0, log(0.1));
    imm_hmm_set_trans(hmm, state0, state1, log(0.2));
    imm_hmm_set_trans(hmm, state1, state1, log(1.0));

    struct imm_path *path = NULL;
    imm_path_create(&path);
    imm_path_add(&path, state0, 1);
    cass_close(imm_hmm_likelihood(hmm, "A", path), -1.3862943611);
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state0, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "T", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state1, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    cass_condition(imm_hmm_normalize(hmm) == 0);

    imm_path_create(&path);
    imm_path_add(&path, state0, 1);
    cass_close(imm_hmm_likelihood(hmm, "G", path), -0.6931471806);
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state0, 1);
    imm_path_add(&path, state1, 1);
    cass_close(imm_hmm_likelihood(hmm, "GT", path), -1.0986122887);
    imm_path_destroy(&path);

    cass_condition(imm_normal_state_normalize(state1) == 0);
    imm_path_create(&path);
    imm_path_add(&path, state0, 1);
    imm_path_add(&path, state1, 1);
    cass_close(imm_hmm_likelihood(hmm, "GT", path), -1.9095425049);
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_mute_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_state *state = imm_mute_state_create("State0", abc);

    imm_hmm_add_state(hmm, state, log(1.0));

    imm_hmm_set_trans(hmm, state, state, log(0.1));

    struct imm_path *path = NULL;
    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "T", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state, 1);
    imm_path_add(&path, state, 1);
    cass_condition(imm_isninf(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_no_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", 0)));

    imm_hmm_destroy(hmm);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_mute_states(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_state *state0 = imm_mute_state_create("State0", abc);

    imm_hmm_add_state(hmm, state0, log(0.5));

    imm_hmm_set_trans(hmm, state0, state0, log(0.1));

    cass_close(imm_hmm_viterbi(hmm, "", state0), -0.693147180560);
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state0)));

    struct imm_state *state1 = imm_mute_state_create("State1", abc);
    imm_hmm_add_state(hmm, state1, log(0.2));

    imm_hmm_set_trans(hmm, state0, state1, log(0.2));

    cass_close(imm_hmm_viterbi(hmm, "", state0), -0.693147180560);
    cass_close(imm_hmm_viterbi(hmm, "", state1), -1.609437912434);

    cass_condition(imm_hmm_set_start_lprob(hmm, state1, -INFINITY) == 0);

    cass_close(imm_hmm_viterbi(hmm, "", state0), -0.693147180560);
    cass_close(imm_hmm_viterbi(hmm, "", state1), -2.302585092994);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_normal_states(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    double lprobs0[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_state *state0 = imm_normal_state_create("State0", abc, lprobs0);

    double lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_state *state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, state0, log(1.0));
    imm_hmm_add_state(hmm, state1, -INFINITY);

    imm_hmm_set_trans(hmm, state0, state0, log(0.1));
    imm_hmm_set_trans(hmm, state0, state1, log(0.2));
    imm_hmm_set_trans(hmm, state1, state1, log(1.0));

    cass_condition(imm_normal_state_normalize(state0) == 0);
    cass_condition(imm_normal_state_normalize(state1) == 0);

    imm_hmm_normalize(hmm);

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state1)));

    cass_close(imm_hmm_viterbi(hmm, "A", state0), -1.386294361120);
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state1)));

    cass_close(imm_hmm_viterbi(hmm, "AG", state0), -3.178053830348);
    cass_close(imm_hmm_viterbi(hmm, "AG", state1), -3.295836866004);

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AGT", state0)));
    cass_close(imm_hmm_viterbi(hmm, "AGT", state1), -4.106767082221);

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AGTC", state0)));
    cass_close(imm_hmm_viterbi(hmm, "AGTC", state1), -6.303991659557);

    imm_hmm_set_trans(hmm, state0, state0, -INFINITY);
    imm_hmm_set_trans(hmm, state0, state1, -INFINITY);
    imm_hmm_set_trans(hmm, state1, state0, -INFINITY);
    imm_hmm_set_trans(hmm, state1, state1, -INFINITY);

    imm_hmm_set_start_lprob(hmm, state0, -INFINITY);
    imm_hmm_set_start_lprob(hmm, state1, -INFINITY);

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state1)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state1)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AA", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AA", state1)));

    imm_hmm_set_start_lprob(hmm, state0, 0.0);

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state1)));
    cass_close(imm_hmm_viterbi(hmm, "A", state0), log(0.25));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state1)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AA", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AA", state1)));

    imm_hmm_set_trans(hmm, state0, state0, log(0.9));

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state1)));
    cass_close(imm_hmm_viterbi(hmm, "A", state0), log(0.25));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state1)));
    cass_close(imm_hmm_viterbi(hmm, "AA", state0), 2 * log(0.25) + log(0.9));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AA", state1)));

    imm_hmm_set_trans(hmm, state0, state1, log(0.2));

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", state1)));
    cass_close(imm_hmm_viterbi(hmm, "A", state0), log(0.25));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", state1)));
    cass_close(imm_hmm_viterbi(hmm, "AA", state0), 2 * log(0.25) + log(0.9));
    cass_close(imm_hmm_viterbi(hmm, "AA", state1), log(0.25) + log(0.5 / 2.25) + log(0.2));

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_profile1(void)
{
    struct imm_abc *abc = imm_abc_create("AB");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_state *start = imm_mute_state_create("START", abc);
    struct imm_state *D0 = imm_mute_state_create("D0", abc);
    struct imm_state *end = imm_mute_state_create("END", abc);

    double M0_lprobs[] = {log(0.4), log(0.2)};
    struct imm_state *M0 = imm_normal_state_create("M0", abc, M0_lprobs);

    double I0_lprobs[] = {log(0.5), log(0.5)};
    struct imm_state *I0 = imm_normal_state_create("I0", abc, I0_lprobs);

    imm_hmm_add_state(hmm, start, 0.0);
    imm_hmm_add_state(hmm, D0, -INFINITY);
    imm_hmm_add_state(hmm, end, -INFINITY);

    imm_hmm_add_state(hmm, M0, -INFINITY);
    imm_hmm_add_state(hmm, I0, -INFINITY);

    imm_hmm_set_trans(hmm, start, D0, log(0.1));
    imm_hmm_set_trans(hmm, D0, end, log(1.0));
    imm_hmm_set_trans(hmm, start, M0, log(0.5));
    imm_hmm_set_trans(hmm, M0, end, log(0.8));
    imm_hmm_set_trans(hmm, M0, I0, log(0.1));
    imm_hmm_set_trans(hmm, I0, I0, log(0.2));
    imm_hmm_set_trans(hmm, I0, end, log(1.0));

    cass_close(imm_hmm_viterbi(hmm, "", end), log(0.1) + log(1.0));
    cass_close(imm_hmm_viterbi(hmm, "", D0), log(0.1));
    cass_close(imm_hmm_viterbi(hmm, "", start), log(1.0));

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "", M0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", start)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", D0)));
    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "A", I0)));
    cass_close(imm_hmm_viterbi(hmm, "A", M0), log(0.5) + log(0.4));
    cass_close(imm_hmm_viterbi(hmm, "A", end), log(0.5) + log(0.4) + log(0.8));
    cass_close(imm_hmm_viterbi(hmm, "B", M0), log(0.5) + log(0.2));
    cass_close(imm_hmm_viterbi(hmm, "B", end), log(0.5) + log(0.2) + log(0.8));

    cass_condition(imm_isninf(imm_hmm_viterbi(hmm, "AA", M0)));
    cass_close(imm_hmm_viterbi(hmm, "AA", end), log(0.5) + log(0.4) + log(0.1) + log(0.5));
    cass_close(imm_hmm_viterbi(hmm, "AA", end), log(0.5) + log(0.4) + log(0.1) + log(0.5));
    cass_close(imm_hmm_viterbi(hmm, "AAB", end),
               log(0.5) + log(0.4) + log(0.1) + log(0.2) + 2 * log(0.5));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_mute_state_destroy(D0);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_profile2(void)
{
    struct imm_abc *abc = imm_abc_create("ABCD");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_state *start = imm_mute_state_create("START", abc);

    double ins_lprobs[] = {log(0.1), log(0.1), log(0.1), log(0.7)};

    double M0_lprobs[] = {log(0.4), -INFINITY, log(0.6), -INFINITY};
    double M1_lprobs[] = {log(0.6), -INFINITY, log(0.4), -INFINITY};
    double M2_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05)};

    struct imm_state *M0 = imm_normal_state_create("M0", abc, M0_lprobs);
    struct imm_state *I0 = imm_normal_state_create("I0", abc, ins_lprobs);

    struct imm_state *D1 = imm_mute_state_create("D1", abc);
    struct imm_state *M1 = imm_normal_state_create("M1", abc, M1_lprobs);
    struct imm_state *I1 = imm_normal_state_create("I1", abc, ins_lprobs);

    struct imm_state *D2 = imm_mute_state_create("D2", abc);
    struct imm_state *M2 = imm_normal_state_create("M2", abc, M2_lprobs);

    struct imm_state *end = imm_mute_state_create("END", abc);

    imm_hmm_add_state(hmm, start, 0.0);

    imm_hmm_add_state(hmm, M0, -INFINITY);
    imm_hmm_add_state(hmm, I0, -INFINITY);

    imm_hmm_add_state(hmm, D1, -INFINITY);
    imm_hmm_add_state(hmm, M1, -INFINITY);
    imm_hmm_add_state(hmm, I1, -INFINITY);

    imm_hmm_add_state(hmm, D2, -INFINITY);
    imm_hmm_add_state(hmm, M2, -INFINITY);

    imm_hmm_add_state(hmm, end, -INFINITY);

    imm_hmm_set_trans(hmm, start, M0, 0.0);
    imm_hmm_set_trans(hmm, start, M1, 0.0);
    imm_hmm_set_trans(hmm, start, M2, 0.0);
    imm_hmm_set_trans(hmm, M0, M1, 0.0);
    imm_hmm_set_trans(hmm, M0, M2, 0.0);
    imm_hmm_set_trans(hmm, M0, end, 0.0);
    imm_hmm_set_trans(hmm, M1, M2, 0.0);
    imm_hmm_set_trans(hmm, M1, end, 0.0);
    imm_hmm_set_trans(hmm, M2, end, 0.0);

    imm_hmm_set_trans(hmm, M0, I0, log(0.2));
    imm_hmm_set_trans(hmm, M0, D1, log(0.1));
    imm_hmm_set_trans(hmm, I0, I0, log(0.5));
    imm_hmm_set_trans(hmm, I0, M1, log(0.5));

    imm_hmm_set_trans(hmm, M1, D2, log(0.1));
    imm_hmm_set_trans(hmm, M1, I1, log(0.2));
    imm_hmm_set_trans(hmm, I1, I1, log(0.5));
    imm_hmm_set_trans(hmm, I1, M2, log(0.5));
    imm_hmm_set_trans(hmm, D1, D2, log(0.3));
    imm_hmm_set_trans(hmm, D1, M2, log(0.7));

    imm_hmm_set_trans(hmm, D2, end, log(1.0));

    cass_close(imm_hmm_viterbi(hmm, "A", M2), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "B", M2), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "C", M2), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "D", M2), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "A", end), log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "B", end), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "C", end), log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "D", end), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "A", M1), log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "C", M1), log(0.4));

    cass_close(imm_hmm_viterbi(hmm, "CA", end), 2 * log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "CD", I0), log(0.6) + log(0.2) + log(0.7));

    cass_close(imm_hmm_viterbi(hmm, "CDDDA", end),
               log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6));

    cass_close(imm_hmm_viterbi(hmm, "CDDDAB", end),
               log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) + log(0.05));

    cass_close(imm_hmm_viterbi(hmm, "CDDDABA", M2), log(0.6) + log(0.2) + 3 * log(0.7) +
                                                        3 * log(0.5) + log(0.6) + log(0.2) +
                                                        log(0.1) + log(0.5) + log(0.05));

    cass_close(imm_hmm_viterbi(hmm, "CDDDABA", M1),
               log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) + log(0.6));

    cass_close(imm_hmm_viterbi(hmm, "CDDDABA", end),
               log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) + log(0.6));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(D1);
    imm_normal_state_destroy(M1);
    imm_normal_state_destroy(I1);
    imm_mute_state_destroy(D2);
    imm_normal_state_destroy(M2);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
}
