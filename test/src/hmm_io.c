#include "cass.h"
#include "imm/imm.h"
#include <stdlib.h>

void test_hmm_write_io_two_states(void);

double single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq,
                      struct imm_state const* end_state, struct imm_path** path);

int main(void)
{
    test_hmm_write_io_two_states();
    return cass_status();
}

static inline struct imm_state const* cast_c(void const* s) { return imm_state_cast_c(s); }
static inline double                  zero(void) { return imm_lprob_zero(); }

void test_hmm_write_io_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);

    double                         lprobs1[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state1 = imm_normal_state_create("state1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    imm_hmm_set_start(hmm, cast_c(state0), log(0.1));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.001));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.9));

    /* TODO: use the same dp instead of calling single_viterbi */
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, cast_c(state1));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, C, cast_c(state1), &path), log(0.25) + log(0.1) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, C, path), log(0.25) + log(0.1) + log(0.9));
    imm_path_destroy(path);

    FILE* file = fopen("test_hmm.tmp/two_states.imm", "w");
    cass_cond(file != NULL);
    cass_equal_int(imm_io_write(file, hmm, dp), 0);
    fclose(file);

    imm_dp_destroy(dp);
    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);

    file = fopen("test_hmm.tmp/two_states.imm", "r");
    cass_cond(file != NULL);
    struct imm_io const* io = imm_io_read(file);
    cass_cond(io != NULL);
    fclose(file);

    cass_equal_uint64(imm_io_nstates(io), 2);

    abc = imm_io_abc(io);
    hmm = imm_io_hmm(io);
    dp = imm_io_dp(io);
    C = imm_seq_create("C", abc);

    /* path = imm_path_create(); */
    /* cass_close(single_viterbi(hmm, C, cast_c(state1), &path), log(0.25) + log(0.1) +
     * log(0.9)); */
    /* cass_close(imm_hmm_likelihood(hmm, C, path), log(0.25) + log(0.1) + log(0.9)); */

    struct imm_results const* results = imm_dp_viterbi(dp, C, 0);
    cass_cond(results != NULL);
    struct imm_result const* r = imm_results_get(results, 0);
    /* struct imm_path const *path_c = imm_result_path(r); */
    cass_close(imm_result_loglik(r), log(0.25) + log(0.1) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, C, imm_result_path(r)),
               log(0.25) + log(0.1) + log(0.9));
    imm_results_destroy(results);

    for (uint32_t i = 0; i < imm_io_nstates(io); ++i) {
        struct imm_state const* state = imm_io_state(io, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {

            cass_cond(strcmp(imm_state_get_name(state), "state0") == 0);
            /* struct imm_mute_state const* mute_state = imm_state_get_impl_c(state); */

        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {

            cass_cond(strcmp(imm_state_get_name(state), "state1") == 0);
            /* struct imm_normal_state const* normal_state = imm_state_get_impl_c(state); */
        }
    }

    imm_seq_destroy(C);
    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_io_destroy_states(io);
    imm_io_destroy(io);
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
