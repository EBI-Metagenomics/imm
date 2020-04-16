#include "cass.h"
#include "imm/imm.h"
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);
void test_hmm_likelihood_mute_state(void);
void test_hmm_likelihood_two_mute_states(void);
void test_hmm_likelihood_invalid(void);
void test_hmm_likelihood_no_state(void);
void test_hmm_wrong_states(void);
void test_hmm_viterbi_one_mute_state(void);
void test_hmm_viterbi_two_mute_states(void);
void test_hmm_viterbi_mute_cycle(void);
void test_hmm_viterbi_one_normal_state(void);
void test_hmm_viterbi_two_normal_states(void);
void test_hmm_viterbi_normal_states(void);
void test_hmm_viterbi_profile1(void);
void test_hmm_viterbi_profile2(void);
void test_hmm_viterbi_profile_delete(void);
void test_hmm_viterbi_global_profile(void);
void test_hmm_viterbi_table_states(void);
void test_hmm_viterbi_cycle_mute_ending(void);
void test_hmm_write_read_two_states(void);

double single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq,
                      struct imm_state const* end_state, struct imm_path* path);

int main(void)
{
    test_hmm_state_id();
    test_hmm_del_get_state();
    test_hmm_set_trans();
    test_hmm_likelihood_single_state();
    test_hmm_likelihood_two_states();
    test_hmm_likelihood_mute_state();
    test_hmm_likelihood_two_mute_states();
    test_hmm_likelihood_invalid();
    test_hmm_likelihood_no_state();
    test_hmm_wrong_states();
    test_hmm_viterbi_one_mute_state();
    test_hmm_viterbi_two_mute_states();
    test_hmm_viterbi_mute_cycle();
    test_hmm_viterbi_one_normal_state();
    test_hmm_viterbi_two_normal_states();
    test_hmm_viterbi_normal_states();
    test_hmm_viterbi_profile1();
    test_hmm_viterbi_profile2();
    test_hmm_viterbi_profile_delete();
    test_hmm_viterbi_global_profile();
    test_hmm_viterbi_table_states();
    test_hmm_viterbi_cycle_mute_ending();
    test_hmm_write_read_two_states();
    return cass_status();
}

static inline int                     is_zero(double a) { return imm_lprob_is_zero(a); }
static inline struct imm_state const* cast_c(void const* s) { return imm_state_cast_c(s); }
static inline double                  zero(void) { return imm_lprob_zero(); }
static inline int                     is_valid(double a) { return imm_lprob_is_valid(a); }

void test_hmm_state_id(void)
{
    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state = imm_mute_state_create("State0", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, cast_c(state), log(1.0)) == 0);
    cass_cond(imm_hmm_add_state(hmm, cast_c(state), log(1.0)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_del_get_state(void)
{
    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, cast_c(state0), log(0.5)) == 0);
    cass_cond(imm_hmm_add_state(hmm, cast_c(state1), log(0.5)) == 0);

    cass_cond(imm_hmm_del_state(hmm, cast_c(state0)) == 0);
    cass_cond(imm_hmm_del_state(hmm, cast_c(state1)) == 0);

    cass_cond(imm_hmm_del_state(hmm, cast_c(state0)) == 1);
    cass_cond(imm_hmm_del_state(hmm, cast_c(state1)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_set_trans(void)
{

    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.5));

    cass_cond(imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.5)) == 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_single_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AG = imm_seq_create("AG", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);

    double lprobs[] = {log(0.25), log(0.25), log(0.5), zero()};

    struct imm_normal_state const* state = imm_normal_state_create("State0", abc, lprobs);
    struct imm_hmm*                hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, cast_c(state), log(0.5));
    cass_cond(imm_hmm_normalize(hmm) == 0);

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    cass_close(imm_hmm_likelihood(hmm, A, path), -1.386294361120);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AG, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    cass_cond(is_zero(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize(hmm) == 0);
    cass_cond(imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), zero()) == 0);
    cass_cond(imm_hmm_normalize(hmm) == 0);
    cass_cond(imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), log(0.5)) == 0);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    cass_close(imm_hmm_likelihood(hmm, AA, path), -3.465735902800);
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize(hmm) == 0);
    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    imm_path_append(path, imm_step_create(cast_c(state), 1));
    cass_close(imm_hmm_likelihood(hmm, AA, path), -2.772588722240);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(AA);
    imm_seq_destroy(AG);
}

void test_hmm_likelihood_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* G = imm_seq_create("G", abc);
    struct imm_seq const* GT = imm_seq_create("GT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    double                         lprobs0[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state0 = imm_normal_state_create("State0", abc, lprobs0);

    double                         lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_normal_state const* state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(1.0));
    imm_hmm_add_state(hmm, cast_c(state1), zero());

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state0), 1));
    cass_close(imm_hmm_likelihood(hmm, A, path), -1.3862943611);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state0), 1));
    cass_cond(is_zero(imm_hmm_likelihood(hmm, T, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state1), 1));
    cass_cond(is_zero(imm_hmm_likelihood(hmm, G, path)));
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize(hmm) == 0);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state0), 1));
    cass_close(imm_hmm_likelihood(hmm, G, path), -0.6931471806);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state0), 1));
    imm_path_append(path, imm_step_create(cast_c(state1), 1));
    cass_close(imm_hmm_likelihood(hmm, GT, path), -1.0986122887);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(G);
    imm_seq_destroy(GT);
}

void test_hmm_likelihood_mute_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* G = imm_seq_create("G", abc);
    struct imm_seq const* GT = imm_seq_create("GT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state = imm_mute_state_create("State0", abc);

    imm_hmm_add_state(hmm, cast_c(state), log(1.0));

    imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), log(0.1));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, T, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, G, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(state), 0));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), 0.0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, GT, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, GT, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(G);
    imm_seq_destroy(GT);
}

void test_hmm_likelihood_two_mute_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* S0 = imm_mute_state_create("S0", abc);
    struct imm_mute_state const* S1 = imm_mute_state_create("S1", abc);

    imm_hmm_add_state(hmm, cast_c(S0), 0.0);
    imm_hmm_add_state(hmm, cast_c(S1), zero());

    imm_hmm_set_trans(hmm, cast_c(S0), cast_c(S1), 0.0);

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(S0), 0));
    imm_path_append(path, imm_step_create(cast_c(S1), 0));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), 0.0);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S0);
    imm_mute_state_destroy(S1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_likelihood_invalid(void)
{
    struct imm_abc const* abc = imm_abc_create("AC", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* S = imm_mute_state_create("S", abc);
    imm_hmm_add_state(hmm, cast_c(S), 0.0);

    struct imm_mute_state const* M1 = imm_mute_state_create("M1", abc);
    imm_hmm_add_state(hmm, cast_c(M1), zero());

    double const                   lprobs[] = {log(0.8), log(0.2)};
    struct imm_normal_state const* M2 = imm_normal_state_create("M2", abc, lprobs);
    imm_hmm_add_state(hmm, cast_c(M2), zero());

    struct imm_mute_state const* E = imm_mute_state_create("E", abc);
    imm_hmm_add_state(hmm, cast_c(E), zero());

    imm_hmm_set_trans(hmm, cast_c(S), cast_c(M1), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M2), cast_c(E), 0.0);
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(E), 0.0);

    imm_hmm_normalize(hmm);

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(cast_c(S), 0));
    cass_cond(imm_step_create(cast_c(M1), 1) == NULL);
    imm_path_append(path, imm_step_create(cast_c(M2), 1));
    imm_path_append(path, imm_step_create(cast_c(E), 0));
    cass_cond(imm_lprob_is_zero(imm_hmm_likelihood(hmm, C, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S);
    imm_mute_state_destroy(M1);
    imm_normal_state_destroy(M2);
    imm_mute_state_destroy(E);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);
}

void test_hmm_likelihood_no_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_wrong_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("state0", abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    cass_equal_int(imm_hmm_set_start(hmm, cast_c(state1), log(0.3)), 1);
    cass_equal_int(imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.3)), 1);
    cass_equal_int(imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state0), log(0.3)), 1);
    cass_cond(imm_hmm_create_dp(hmm, cast_c(state1)) == NULL);
    cass_equal_int(imm_hmm_normalize_trans(hmm, cast_c(state1)), 1);
    cass_equal_int(imm_hmm_normalize_trans(hmm, cast_c(state0)), 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_one_mute_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state = imm_mute_state_create("state", abc);

    imm_hmm_add_state(hmm, cast_c(state), log(0.5));

    struct imm_path* path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(state), path), log(0.5));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.5));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, C, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, C, path)));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, cast_c(state), imm_lprob_zero());

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, C, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, C, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(C);
}

void test_hmm_viterbi_two_mute_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("state", abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.1));

    struct imm_path* path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(state0), path), log(0.5));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.5));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(state1), path), log(0.1));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.1));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, cast_c(state1), imm_lprob_zero());

    path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(state0), path), log(0.5));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.5));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.1));

    path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(state1), path), log(0.5) + log(0.1));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.5) + log(0.1));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_viterbi_mute_cycle(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("State0", abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));

    struct imm_mute_state const* state1 = imm_mute_state_create("State1", abc);
    imm_hmm_add_state(hmm, cast_c(state1), imm_lprob_zero());

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state0), log(0.2));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_viterbi_one_normal_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AC = imm_seq_create("AC", abc);
    struct imm_seq const* ACT = imm_seq_create("ACT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    double                         lprobs0[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state = imm_normal_state_create("State0", abc, lprobs0);

    imm_hmm_add_state(hmm, cast_c(state), log(0.1));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state), path), log(0.1) + log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.1) + log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, T, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, T, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AC, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AC, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), log(0.1));

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state), path), log(0.1) + log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.1) + log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(state), path), 2 * log(0.1) + 2 * log(0.25));
    cass_close(imm_hmm_likelihood(hmm, AA, path), 2 * log(0.1) + 2 * log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, ACT, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, ACT, path)));
    imm_path_destroy(path);

    imm_hmm_normalize(hmm);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state), path), log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(state), path), 2 * log(0.25));
    cass_close(imm_hmm_likelihood(hmm, AA, path), 2 * log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, ACT, cast_c(state), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, ACT, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(AA);
    imm_seq_destroy(AC);
    imm_seq_destroy(ACT);
}

void test_hmm_viterbi_two_normal_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* AC = imm_seq_create("AC", abc);
    struct imm_seq const* AT = imm_seq_create("AT", abc);
    struct imm_seq const* ATT = imm_seq_create("ATT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    double                         lprobs0[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state0 = imm_normal_state_create("State0", abc, lprobs0);

    double lprobs1[] = {log(0.25), log(0.25), log(0.5), log(0.5)};

    struct imm_normal_state const* state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.1));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.3));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state0), path), log(0.1) + log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.1) + log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, T, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, T, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AC, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AC, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    double desired = log(0.1) + log(0.25) + log(0.3) + log(0.5);
    cass_close(single_viterbi(hmm, AT, cast_c(state1), path), desired);
    cass_close(imm_hmm_likelihood(hmm, AT, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, ATT, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, ATT, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), log(0.5));

    path = imm_path_create();
    desired = log(0.2) + log(0.25) + 4 * log(0.5);
    cass_close(single_viterbi(hmm, ATT, cast_c(state1), path), desired);
    cass_close(imm_hmm_likelihood(hmm, ATT, path), desired);
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, cast_c(state1), imm_lprob_zero());

    path = imm_path_create();
    desired = log(0.1) + log(0.25) + log(0.3) + 3 * log(0.5);
    cass_close(single_viterbi(hmm, ATT, cast_c(state1), path), desired);
    cass_close(imm_hmm_likelihood(hmm, ATT, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(AC);
    imm_seq_destroy(AT);
    imm_seq_destroy(ATT);
}

void test_hmm_viterbi_normal_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AG = imm_seq_create("AG", abc);
    struct imm_seq const* AGT = imm_seq_create("AGT", abc);
    struct imm_seq const* AGTC = imm_seq_create("AGTC", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    double const                   lprobs0[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state0 = imm_normal_state_create("State0", abc, lprobs0);

    double const                   lprobs1[] = {log(0.5) - log(2.25), log(0.25) - log(2.25),

                              log(0.5) - log(2.25), log(1.0) - log(2.25)};
    struct imm_normal_state const* state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(1.0));
    imm_hmm_add_state(hmm, cast_c(state1), zero());

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), log(1.0));

    imm_hmm_normalize(hmm);

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state0), path), -1.386294361120);

    cass_close(imm_hmm_likelihood(hmm, A, path), -1.386294361120);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AG, cast_c(state0), path), -3.178053830348);

    cass_close(imm_hmm_likelihood(hmm, AG, path), -3.178053830348);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AG, cast_c(state1), path), -3.295836866004);

    cass_close(imm_hmm_likelihood(hmm, AG, path), -3.295836866004);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AGT, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AGT, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AGT, cast_c(state1), path), -4.106767082221);

    cass_close(imm_hmm_likelihood(hmm, AGT, path), -4.106767082221);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AGTC, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AGTC, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AGTC, cast_c(state1), path), -6.303991659557);
    cass_close(imm_hmm_likelihood(hmm, AGTC, path), -6.303991659557);
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), zero());
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), zero());
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state0), zero());
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), zero());

    imm_hmm_set_start(hmm, cast_c(state0), zero());
    imm_hmm_set_start(hmm, cast_c(state1), zero());

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AA, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AA, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, cast_c(state0), 0.0);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state0), path), log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AA, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AA, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), log(0.9));

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state0), path), log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(state0), path), 2 * log(0.25) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, AA, path), 2 * log(0.25) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AA, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(state0), path), log(0.25));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.25));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(state1), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(state0), path), 2 * log(0.25) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, AA, path), 2 * log(0.25) + log(0.9));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(AA);
    imm_seq_destroy(AG);
    imm_seq_destroy(AGT);
    imm_seq_destroy(AGTC);
}

void test_hmm_viterbi_profile1(void)
{
    struct imm_abc const* abc = imm_abc_create("AB", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* B = imm_seq_create("B", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AAB = imm_seq_create("AAB", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    struct imm_mute_state const* D0 = imm_mute_state_create("D0", abc);
    struct imm_mute_state const* end = imm_mute_state_create("END", abc);

    double                         M0_lprobs[] = {log(0.4), log(0.2)};
    struct imm_normal_state const* M0 = imm_normal_state_create("M0", abc, M0_lprobs);

    double                         I0_lprobs[] = {log(0.5), log(0.5)};
    struct imm_normal_state const* I0 = imm_normal_state_create("I0", abc, I0_lprobs);

    imm_hmm_add_state(hmm, cast_c(start), 0.0);
    imm_hmm_add_state(hmm, cast_c(D0), zero());
    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_add_state(hmm, cast_c(M0), zero());
    imm_hmm_add_state(hmm, cast_c(I0), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(D0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(D0), cast_c(end), log(1.0));
    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(end), log(0.8));
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(I0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(I0), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(end), log(1.0));

    struct imm_path* path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(end), path), log(0.1) + log(1.0));

    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.1) + log(1.0));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(D0), path), log(0.1));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(0.1));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, EMPTY, cast_c(start), path), log(1.0));
    cass_close(imm_hmm_likelihood(hmm, EMPTY, path), log(1.0));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, cast_c(M0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(start), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(D0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, A, cast_c(I0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(M0), path), log(0.5) + log(0.4));

    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.5) + log(0.4));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(end), path), log(0.5) + log(0.4) + log(0.8));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.5) + log(0.4) + log(0.8));

    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, B, cast_c(M0), path), log(0.5) + log(0.2));

    cass_close(imm_hmm_likelihood(hmm, B, path), log(0.5) + log(0.2));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, B, cast_c(end), path), log(0.5) + log(0.2) + log(0.8));
    cass_close(imm_hmm_likelihood(hmm, B, path), log(0.5) + log(0.2) + log(0.8));

    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, AA, cast_c(M0), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, AA, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    double desired = log(0.5) + log(0.4) + log(0.1) + log(0.5);
    cass_close(single_viterbi(hmm, AA, cast_c(end), path), desired);
    cass_close(imm_hmm_likelihood(hmm, AA, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.5) + log(0.4) + log(0.1) + log(0.5);
    cass_close(single_viterbi(hmm, AA, cast_c(end), path), desired);
    cass_close(imm_hmm_likelihood(hmm, AA, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.5) + log(0.4) + log(0.1) + log(0.2) + 2 * log(0.5);
    cass_close(single_viterbi(hmm, AAB, cast_c(end), path), desired);
    cass_close(imm_hmm_likelihood(hmm, AAB, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_mute_state_destroy(D0);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(B);
    imm_seq_destroy(AA);
    imm_seq_destroy(AAB);
}

void test_hmm_viterbi_profile2(void)
{
    struct imm_abc const* abc = imm_abc_create("ABCD", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* B = imm_seq_create("B", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_seq const* D = imm_seq_create("D", abc);
    struct imm_seq const* CA = imm_seq_create("CA", abc);
    struct imm_seq const* CD = imm_seq_create("CD", abc);
    struct imm_seq const* CDDDA = imm_seq_create("CDDDA", abc);
    struct imm_seq const* CDDDAB = imm_seq_create("CDDDAB", abc);
    struct imm_seq const* CDDDABA = imm_seq_create("CDDDABA", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);

    double ins_lprobs[] = {log(0.1), log(0.1), log(0.1), log(0.7)};

    double M0_lprobs[] = {log(0.4), zero(), log(0.6), zero()};
    double M1_lprobs[] = {log(0.6), zero(), log(0.4), zero()};
    double M2_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05)};

    struct imm_normal_state const* M0 = imm_normal_state_create("M0", abc, M0_lprobs);
    struct imm_normal_state const* I0 = imm_normal_state_create("I0", abc, ins_lprobs);

    struct imm_mute_state const*   D1 = imm_mute_state_create("D1", abc);
    struct imm_normal_state const* M1 = imm_normal_state_create("M1", abc, M1_lprobs);
    struct imm_normal_state const* I1 = imm_normal_state_create("I1", abc, ins_lprobs);

    struct imm_mute_state const*   D2 = imm_mute_state_create("D2", abc);
    struct imm_normal_state const* M2 = imm_normal_state_create("M2", abc, M2_lprobs);

    struct imm_mute_state const* end = imm_mute_state_create("END", abc);

    imm_hmm_add_state(hmm, cast_c(start), 0.0);

    imm_hmm_add_state(hmm, cast_c(M0), zero());
    imm_hmm_add_state(hmm, cast_c(I0), zero());

    imm_hmm_add_state(hmm, cast_c(D1), zero());
    imm_hmm_add_state(hmm, cast_c(M1), zero());
    imm_hmm_add_state(hmm, cast_c(I1), zero());

    imm_hmm_add_state(hmm, cast_c(D2), zero());
    imm_hmm_add_state(hmm, cast_c(M2), zero());

    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M0), 0.0);
    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M1), 0.0);
    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(M1), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(end), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(end), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M2), cast_c(end), 0.0);

    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(I0), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(D1), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(I0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(M1), log(0.5));

    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(D2), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(I1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(I1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(M2), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(D2), log(0.3));
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(M2), log(0.7));

    imm_hmm_set_trans(hmm, cast_c(D2), cast_c(end), log(1.0));

    struct imm_path* path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(M2), path), log(0.05));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.05));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, B, cast_c(M2), path), log(0.05));
    cass_close(imm_hmm_likelihood(hmm, B, path), log(0.05));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, C, cast_c(M2), path), log(0.05));
    cass_close(imm_hmm_likelihood(hmm, C, path), log(0.05));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, D, cast_c(M2), path), log(0.05));
    cass_close(imm_hmm_likelihood(hmm, D, path), log(0.05));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(end), path), log(0.6));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.6));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, B, cast_c(end), path), log(0.05));
    cass_close(imm_hmm_likelihood(hmm, B, path), log(0.05));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, C, cast_c(end), path), log(0.6));
    cass_close(imm_hmm_likelihood(hmm, C, path), log(0.6));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, D, cast_c(end), path), log(0.05));
    cass_close(imm_hmm_likelihood(hmm, D, path), log(0.05));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(M1), path), log(0.6));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.6));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, C, cast_c(M1), path), log(0.4));
    cass_close(imm_hmm_likelihood(hmm, C, path), log(0.4));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CA, cast_c(end), path), 2 * log(0.6));
    cass_close(imm_hmm_likelihood(hmm, CA, path), 2 * log(0.6));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CD, cast_c(I0), path), log(0.6) + log(0.2) + log(0.7));
    cass_close(imm_hmm_likelihood(hmm, CD, path), log(0.6) + log(0.2) + log(0.7));
    imm_path_destroy(path);

    path = imm_path_create();
    double desired = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6);
    cass_close(single_viterbi(hmm, CDDDA, cast_c(end), path), desired);
    cass_close(imm_hmm_likelihood(hmm, CDDDA, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) + log(0.05);
    cass_close(single_viterbi(hmm, CDDDAB, cast_c(end), path), desired);
    cass_close(imm_hmm_likelihood(hmm, CDDDAB, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) + log(0.2) +
              log(0.1) + log(0.5) + log(0.05);
    cass_close(single_viterbi(hmm, CDDDABA, cast_c(M2), path), desired);
    cass_close(imm_hmm_likelihood(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) + log(0.6);

    cass_close(single_viterbi(hmm, CDDDABA, cast_c(M1), path), desired);
    cass_close(imm_hmm_likelihood(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) + log(0.6);

    cass_close(single_viterbi(hmm, CDDDABA, cast_c(end), path), desired);
    cass_close(imm_hmm_likelihood(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

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
    imm_seq_destroy(A);
    imm_seq_destroy(B);
    imm_seq_destroy(C);
    imm_seq_destroy(D);
    imm_seq_destroy(CA);
    imm_seq_destroy(CD);
    imm_seq_destroy(CDDDA);
    imm_seq_destroy(CDDDAB);
    imm_seq_destroy(CDDDABA);
}

void test_hmm_viterbi_profile_delete(void)
{
    struct imm_abc const* abc = imm_abc_create("AB", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AB = imm_seq_create("AB", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    double                         N0_lprobs[] = {log(0.5), zero()};
    struct imm_normal_state const* N0 = imm_normal_state_create("N0", abc, N0_lprobs);

    struct imm_mute_state const* M = imm_mute_state_create("M", abc);

    double                         N1_lprobs[] = {log(0.5), zero()};
    struct imm_normal_state const* N1 = imm_normal_state_create("N1", abc, N1_lprobs);

    double                         N2_lprobs[] = {zero(), log(0.5)};
    struct imm_normal_state const* N2 = imm_normal_state_create("N2", abc, N2_lprobs);

    imm_hmm_add_state(hmm, cast_c(N2), zero());
    imm_hmm_add_state(hmm, cast_c(N1), zero());
    imm_hmm_add_state(hmm, cast_c(M), zero());
    imm_hmm_add_state(hmm, cast_c(N0), 0);

    imm_hmm_set_trans(hmm, cast_c(N0), cast_c(N1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(N0), cast_c(M), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(N1), cast_c(N2), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(M), cast_c(N2), log(0.5));

    struct imm_path* path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(N0), path), log(0.5));
    cass_close(imm_hmm_likelihood(hmm, A, path), log(0.5));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(M), path), 2 * log(0.5));
    cass_close(imm_hmm_likelihood(hmm, A, path), 2 * log(0.5));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AB, cast_c(N2), path), 4 * log(0.5));
    cass_close(imm_hmm_likelihood(hmm, AB, path), 4 * log(0.5));
    imm_path_destroy(path);

    cass_cond(imm_hmm_del_state(hmm, cast_c(N2)) == 0);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(M), path), 2 * log(0.5));
    cass_close(imm_hmm_likelihood(hmm, A, path), 2 * log(0.5));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(N0);
    imm_mute_state_destroy(M);
    imm_normal_state_destroy(N1);
    imm_normal_state_destroy(N2);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(AB);
}

void test_hmm_viterbi_global_profile(void)
{
    struct imm_abc const* abc = imm_abc_create("ABCZ", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AAB = imm_seq_create("AAB", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_seq const* CC = imm_seq_create("CC", abc);
    struct imm_seq const* CCC = imm_seq_create("CCC", abc);
    struct imm_seq const* CCA = imm_seq_create("CCA", abc);
    struct imm_seq const* CCAB = imm_seq_create("CCAB", abc);
    struct imm_seq const* CCABB = imm_seq_create("CCABB", abc);
    struct imm_seq const* CCABA = imm_seq_create("CCABA", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);

    double                         B_lprobs[] = {log(0.01), log(0.01), log(1.0), zero()};
    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);

    double M0_lprobs[] = {log(0.9), log(0.01), log(0.01), zero()};

    struct imm_normal_state const* M0 = imm_normal_state_create("M0", abc, M0_lprobs);

    double                         M1_lprobs[] = {log(0.01), log(0.9), zero(), zero()};
    struct imm_normal_state const* M1 = imm_normal_state_create("M1", abc, M1_lprobs);

    double                         M2_lprobs[] = {log(0.5), log(0.5), zero(), zero()};
    struct imm_normal_state const* M2 = imm_normal_state_create("M2", abc, M2_lprobs);

    struct imm_mute_state const* E = imm_mute_state_create("E", abc);
    struct imm_mute_state const* end = imm_mute_state_create("END", abc);

    double                         Z_lprobs[] = {zero(), zero(), zero(), log(1.0)};
    struct imm_normal_state const* Z = imm_normal_state_create("Z", abc, Z_lprobs);

    double                         ins_lprobs[] = {log(0.1), log(0.1), log(0.1), zero()};
    struct imm_normal_state const* I0 = imm_normal_state_create("I0", abc, ins_lprobs);
    struct imm_normal_state const* I1 = imm_normal_state_create("I1", abc, ins_lprobs);

    struct imm_mute_state const* D1 = imm_mute_state_create("D1", abc);
    struct imm_mute_state const* D2 = imm_mute_state_create("D2", abc);

    imm_hmm_add_state(hmm, cast_c(start), log(1.0));
    imm_hmm_add_state(hmm, cast_c(B), zero());
    imm_hmm_add_state(hmm, cast_c(M0), zero());
    imm_hmm_add_state(hmm, cast_c(M1), zero());
    imm_hmm_add_state(hmm, cast_c(M2), zero());
    imm_hmm_add_state(hmm, cast_c(D1), zero());
    imm_hmm_add_state(hmm, cast_c(D2), zero());
    imm_hmm_add_state(hmm, cast_c(I0), zero());
    imm_hmm_add_state(hmm, cast_c(I1), zero());
    imm_hmm_add_state(hmm, cast_c(E), zero());
    imm_hmm_add_state(hmm, cast_c(Z), zero());
    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(B), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(B), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M0), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M1), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M2), 0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(M1), 0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(M2), 0);
    imm_hmm_set_trans(hmm, cast_c(M2), cast_c(E), 0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(E), 0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(E), 0);

    imm_hmm_set_trans(hmm, cast_c(E), cast_c(end), 0);

    imm_hmm_set_trans(hmm, cast_c(E), cast_c(Z), 0);
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(Z), 0);
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(B), 0);

    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(D1), 0);
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(D2), 0);
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(M2), 0);
    imm_hmm_set_trans(hmm, cast_c(D2), cast_c(E), 0);

    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(I0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(I0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(M1), log(0.5));

    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(I1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(I1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(M2), log(0.5));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, C, cast_c(start), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, C, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, C, cast_c(B), path), 0);
    cass_close(imm_hmm_likelihood(hmm, C, path), 0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CC, cast_c(B), path), 0);
    cass_close(imm_hmm_likelihood(hmm, CC, path), 0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CCC, cast_c(B), path), 0);
    cass_close(imm_hmm_likelihood(hmm, CCC, path), 0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CCA, cast_c(B), path), log(0.01));
    cass_close(imm_hmm_likelihood(hmm, CCA, path), log(0.01));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CCA, cast_c(M0), path), log(0.9));
    cass_close(imm_hmm_likelihood(hmm, CCA, path), log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CCAB, cast_c(M1), path), 2 * log(0.9));
    cass_close(imm_hmm_likelihood(hmm, CCAB, path), 2 * log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CCAB, cast_c(I0), path), log(0.9 * 0.5 * 0.1));
    cass_close(imm_hmm_likelihood(hmm, CCAB, path), log(0.9 * 0.5 * 0.1));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CCABB, cast_c(I0), path), log(0.9) + 2 * (log(0.05)));
    cass_close(imm_hmm_likelihood(hmm, CCABB, path), log(0.9) + 2 * (log(0.05)));

    imm_path_destroy(path);

    path = imm_path_create();
    double desired = log(0.9) + log(0.5) + log(0.1) + log(0.5) + log(0.01);
    cass_close(single_viterbi(hmm, CCABA, cast_c(M1), path), desired);
    cass_close(imm_hmm_likelihood(hmm, CCABA, path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(D1), path), log(0.01) + log(0.9));

    cass_close(imm_hmm_likelihood(hmm, AA, path), log(0.01) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(D2), path), log(0.01) + log(0.9));

    cass_close(imm_hmm_likelihood(hmm, AA, path), log(0.01) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, AA, cast_c(E), path), log(0.01) + log(0.9));

    cass_close(imm_hmm_likelihood(hmm, AA, path), log(0.01) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.01) + log(0.9) + log(0.5);
    cass_close(single_viterbi(hmm, AAB, cast_c(M2), path), desired);
    cass_close(imm_hmm_likelihood(hmm, AAB, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(B);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(M1);
    imm_normal_state_destroy(M2);
    imm_normal_state_destroy(I0);
    imm_normal_state_destroy(I1);
    imm_mute_state_destroy(D1);
    imm_mute_state_destroy(D2);
    imm_normal_state_destroy(Z);
    imm_mute_state_destroy(E);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(AA);
    imm_seq_destroy(AAB);
    imm_seq_destroy(C);
    imm_seq_destroy(CC);
    imm_seq_destroy(CCC);
    imm_seq_destroy(CCA);
    imm_seq_destroy(CCAB);
    imm_seq_destroy(CCABB);
    imm_seq_destroy(CCABA);
}

void test_hmm_viterbi_table_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGTX", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* seqT = imm_seq_create("T", abc);
    struct imm_seq const* TAT = imm_seq_create("TAT", abc);
    struct imm_seq const* TATX = imm_seq_create("TATX", abc);
    struct imm_seq const* TATA = imm_seq_create("TATA", abc);
    struct imm_seq const* TATTX = imm_seq_create("TATTX", abc);
    struct imm_seq const* TATTXX = imm_seq_create("TATTXX", abc);
    struct imm_seq const* CAXCA = imm_seq_create("CAXCA", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* S = imm_mute_state_create("S", abc);
    struct imm_seq_table*        table = imm_seq_table_create(abc);
    imm_seq_table_add(table, EMPTY, log(0.5));
    imm_seq_table_add(table, A, log(0.1));
    imm_seq_table_add(table, TAT, log(0.2));
    cass_cond(imm_seq_table_lprob(table, EMPTY) == log(0.5));
    cass_cond(imm_seq_table_lprob(table, A) == log(0.1));
    cass_cond(imm_seq_table_lprob(table, TAT) == log(0.2));
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, seqT)));
    struct imm_table_state* T = imm_table_state_create("T", table);
    imm_seq_table_destroy(table);

    struct imm_mute_state const* D = imm_mute_state_create("D", abc);

    double                         N0_lprobs[] = {zero(), log(0.5), log(0.5), zero(), zero()};
    struct imm_normal_state const* N0 = imm_normal_state_create("N0", abc, N0_lprobs);

    double N1_lprobs[] = {log(0.25), log(0.25), log(0.25), log(0.25), zero()};
    struct imm_normal_state const* N1 = imm_normal_state_create("N1", abc, N1_lprobs);

    struct imm_mute_state const* E = imm_mute_state_create("E", abc);

    double Z_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};

    struct imm_normal_state const* Z = imm_normal_state_create("Z", abc, Z_lprobs);

    imm_hmm_add_state(hmm, cast_c(S), log(1.0));
    imm_hmm_add_state(hmm, cast_c(T), zero());
    imm_hmm_add_state(hmm, cast_c(N0), zero());
    imm_hmm_add_state(hmm, cast_c(D), zero());
    imm_hmm_add_state(hmm, cast_c(N1), zero());
    imm_hmm_add_state(hmm, cast_c(E), zero());
    imm_hmm_add_state(hmm, cast_c(Z), zero());

    cass_cond(imm_hmm_set_trans(hmm, cast_c(S), cast_c(T), imm_lprob_invalid()) == 1);
    cass_cond(imm_hmm_set_trans(hmm, cast_c(S), cast_c(T), imm_lprob_invalid()) == 1);

    imm_hmm_set_trans(hmm, cast_c(S), cast_c(T), log(1.0));
    imm_hmm_set_trans(hmm, cast_c(T), cast_c(D), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(T), cast_c(N0), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(D), cast_c(N1), log(0.3));
    imm_hmm_set_trans(hmm, cast_c(N0), cast_c(N1), log(0.4));
    imm_hmm_set_trans(hmm, cast_c(N1), cast_c(E), log(1.0));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(Z), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(Z), log(2.0));
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(T), log(0.6));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(single_viterbi(hmm, TATX, cast_c(E), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, TATX, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, TATA, cast_c(N1), path), -6.502290170873972);
    cass_close(imm_hmm_likelihood(hmm, TATA, path), -6.502290170873972);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, TATA, cast_c(E), path), -6.502290170873972);
    cass_close(imm_hmm_likelihood(hmm, TATA, path), -6.502290170873972);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, TATTX, cast_c(Z), path), -7.195437351433918);
    cass_close(imm_hmm_likelihood(hmm, TATTX, path), -7.195437351433918);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, TATTXX, cast_c(Z), path), -6.502290170873972);
    cass_close(imm_hmm_likelihood(hmm, TATTXX, path), -6.502290170873972);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(single_viterbi(hmm, CAXCA, cast_c(E), path), -11.800607537422009);
    cass_close(imm_hmm_likelihood(hmm, CAXCA, path), -11.800607537422009);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S);
    imm_table_state_destroy(T);
    imm_mute_state_destroy(D);
    imm_normal_state_destroy(N0);
    imm_normal_state_destroy(N1);
    imm_mute_state_destroy(E);
    imm_normal_state_destroy(Z);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(seqT);
    imm_seq_destroy(TAT);
    imm_seq_destroy(TATX);
    imm_seq_destroy(TATA);
    imm_seq_destroy(TATTX);
    imm_seq_destroy(TATTXX);
    imm_seq_destroy(CAXCA);
}

void test_hmm_viterbi_cycle_mute_ending(void)
{
    struct imm_abc const* abc = imm_abc_create("AB", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, cast_c(start), log(1.0));

    struct imm_mute_state const* B = imm_mute_state_create("B", abc);
    imm_hmm_add_state(hmm, cast_c(B), zero());

    double                         lprobs[] = {log(0.01), log(0.01)};
    struct imm_normal_state const* M = imm_normal_state_create("M", abc, lprobs);
    imm_hmm_add_state(hmm, cast_c(M), zero());

    struct imm_mute_state const* E = imm_mute_state_create("E", abc);
    imm_hmm_add_state(hmm, cast_c(E), zero());

    struct imm_mute_state const* J = imm_mute_state_create("J", abc);
    imm_hmm_add_state(hmm, cast_c(J), zero());

    struct imm_mute_state const* end = imm_mute_state_create("END", abc);
    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(B), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(M), cast_c(E), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(end), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(J), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(J), cast_c(B), log(0.1));

    struct imm_path* path = imm_path_create();
    cass_close(single_viterbi(hmm, A, cast_c(end), path), -13.815510557964272);

    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_mute_state_destroy(B);
    imm_normal_state_destroy(M);
    imm_mute_state_destroy(E);
    imm_mute_state_destroy(J);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
}

double single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq,
                      struct imm_state const* end_state, struct imm_path* path)
{
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, end_state);
    if (dp == NULL)
        return imm_lprob_invalid();

    struct imm_results const* results = imm_dp_viterbi(dp, seq, 0);
    if (results == NULL) {
        imm_dp_destroy(dp);
        return imm_lprob_invalid();
    }
    struct imm_result const* r = imm_results_get(results, 0);

    struct imm_path const* src = imm_result_path(r);
    for (struct imm_step const* step = imm_path_first(src); step;
         step = imm_path_next(src, step))
        imm_path_append(path, imm_step_create(imm_step_state(step), imm_step_seq_len(step)));

    double score = imm_result_loglik(r);
    imm_results_destroy(results);
    imm_dp_destroy(dp);

    return score;
}

void test_hmm_write_read_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);

    double                         lprobs1[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state1 = imm_normal_state_create("state1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    imm_hmm_set_start(hmm, cast_c(state0), log(0.1));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.9));

    struct imm_dp const* dp = imm_hmm_create_dp(hmm, cast_c(state0));

    FILE* file = fopen("test_hmm.tmp/one_mute_state.imm", "w");
    cass_cond(file != NULL);
    cass_equal_int(imm_io_write(file, hmm, dp), 0);
    fclose(file);

    imm_dp_destroy(dp);
    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);

    file = fopen("test_hmm.tmp/one_mute_state.imm", "r");
    cass_cond(file != NULL);
    struct imm_io const* io = imm_io_read(file);
    cass_cond(io != NULL);
    fclose(file);

    cass_equal_uint64(imm_io_nstates(io), 2);
    for (uint32_t i = 0; i < imm_io_nstates(io); ++i) {
        struct imm_state const* state = imm_io_state(io, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {

            cass_cond(strcmp(imm_state_get_name(state), "state0") == 0);
            struct imm_mute_state const *mute_state = imm_state_get_impl_c(state);
            imm_mute_state_destroy(mute_state);

        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {

            cass_cond(strcmp(imm_state_get_name(state), "state1") == 0);
            struct imm_normal_state const *normal_state = imm_state_get_impl_c(state);
            imm_normal_state_destroy(normal_state);
        }
    }

    imm_abc_destroy(imm_io_abc(io));
    imm_hmm_destroy(imm_io_hmm(io));
    imm_io_destroy(io);
}
