#include "cass.h"
#include "imm/imm.h"
#include <stdlib.h>

#define TMP_FOLDER "test_hmm_io.tmp"

void test_hmm_write_io_two_states(void);

int main(void)
{
    test_hmm_write_io_two_states();
    return cass_status();
}

static inline double zero(void) { return imm_lprob_zero(); }

void test_hmm_write_io_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);

    double                         lprobs1[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state const* state1 = imm_normal_state_create("state1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_mute_state_parent(state0), log(0.5));
    imm_hmm_set_start(hmm, imm_mute_state_parent(state0), log(0.1));
    imm_hmm_add_state(hmm, imm_normal_state_parent(state1), log(0.001));
    imm_hmm_set_trans(hmm, imm_mute_state_parent(state0), imm_normal_state_parent(state1),
                      log(0.9));

    struct imm_dp const* dp = imm_hmm_create_dp(hmm, imm_normal_state_parent(state1));

    struct imm_results const* results = imm_dp_viterbi(dp, C, 0);
    cass_cond(results != NULL);
    cass_equal_int(imm_results_size(results), 1);
    struct imm_path const* path = imm_result_path(imm_results_get(results, 0));
    double                 score = imm_result_loglik(imm_results_get(results, 0));
    cass_close(score, log(0.25) + log(0.1) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, C, path), log(0.25) + log(0.1) + log(0.9));
    imm_results_destroy(results);

    struct imm_io const* io = imm_io_create(hmm, dp);
    FILE*                file = fopen(TMP_FOLDER "/two_states.imm", "w");
    cass_cond(file != NULL);
    cass_equal_int(imm_io_write(io, file), 0);
    fclose(file);

    imm_dp_destroy(dp);
    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);
    imm_io_destroy(io);

    file = fopen(TMP_FOLDER "/two_states.imm", "r");
    cass_cond(file != NULL);
    io = imm_io_create_from_file(file);
    cass_cond(io != NULL);
    fclose(file);

    cass_equal_uint64(imm_io_nstates(io), 2);

    abc = imm_io_abc(io);
    hmm = imm_io_hmm(io);
    dp = imm_io_dp(io);
    C = imm_seq_create("C", abc);

    results = imm_dp_viterbi(dp, C, 0);
    cass_cond(results != NULL);
    cass_equal_int(imm_results_size(results), 1);
    path = imm_result_path(imm_results_get(results, 0));
    score = imm_result_loglik(imm_results_get(results, 0));
    cass_close(score, log(0.25) + log(0.1) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, C, path), log(0.25) + log(0.1) + log(0.9));
    imm_results_destroy(results);

    for (uint32_t i = 0; i < imm_io_nstates(io); ++i) {
        struct imm_state const* state = imm_io_state(io, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "state0") == 0);
        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "state1") == 0);
        }
    }

    imm_seq_destroy(C);
    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_io_destroy_states(io);
    imm_io_destroy(io);
}
