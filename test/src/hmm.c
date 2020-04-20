#include "cass.h"
#include "imm/imm.h"
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_wrong_states(void);

double single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq,
                      struct imm_state const* end_state, struct imm_path** path);

int main(void)
{
    test_hmm_state_id();
    test_hmm_del_get_state();
    test_hmm_set_trans();
    test_hmm_wrong_states();
    return cass_status();
}

static inline struct imm_state const* cast(void const* s) { return imm_state_cast(s); }

void test_hmm_state_id(void)
{
    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state = imm_mute_state_create("State0", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, cast(state), log(1.0)) == 0);
    cass_cond(imm_hmm_add_state(hmm, cast(state), log(1.0)) == 1);

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

    cass_cond(imm_hmm_add_state(hmm, cast(state0), log(0.5)) == 0);
    cass_cond(imm_hmm_add_state(hmm, cast(state1), log(0.5)) == 0);

    cass_cond(imm_hmm_del_state(hmm, cast(state0)) == 0);
    cass_cond(imm_hmm_del_state(hmm, cast(state1)) == 0);

    cass_cond(imm_hmm_del_state(hmm, cast(state0)) == 1);
    cass_cond(imm_hmm_del_state(hmm, cast(state1)) == 1);

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

    imm_hmm_add_state(hmm, cast(state0), log(0.5));
    imm_hmm_add_state(hmm, cast(state1), log(0.5));

    cass_cond(imm_hmm_set_trans(hmm, cast(state0), cast(state1), log(0.5)) == 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_wrong_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("state0", abc);

    imm_hmm_add_state(hmm, cast(state0), log(0.5));
    cass_equal_int(imm_hmm_set_start(hmm, cast(state1), log(0.3)), 1);
    cass_equal_int(imm_hmm_set_trans(hmm, cast(state0), cast(state1), log(0.3)), 1);
    cass_equal_int(imm_hmm_set_trans(hmm, cast(state1), cast(state0), log(0.3)), 1);
    cass_cond(imm_hmm_create_dp(hmm, cast(state1)) == NULL);
    cass_equal_int(imm_hmm_normalize_trans(hmm, cast(state1)), 1);
    cass_equal_int(imm_hmm_normalize_trans(hmm, cast(state0)), 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

double single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq,
                      struct imm_state const* end_state, struct imm_path** path)
{
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, end_state);
    if (dp == NULL) {
        *path = imm_path_create();
        return imm_lprob_invalid();
    }

    struct imm_results const* results = imm_dp_viterbi(dp, seq, 0);
    if (results == NULL) {
        *path = imm_path_create();
        imm_dp_destroy(dp);
        return imm_lprob_invalid();
    }
    struct imm_result const* r = imm_results_get(results, 0);

    *path = imm_path_clone(imm_result_path(r));

    double score = imm_result_loglik(r);
    imm_results_destroy(results);
    imm_dp_destroy(dp);

    return score;
}
