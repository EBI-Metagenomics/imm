#include "dp.h"
#include "bug.h"
#include "compiler.h"
#include "dp_emission.h"
#include "dp_matrix.h"
#include "dp_state_table.h"
#include "dp_task.h"
#include "dp_trans_table.h"
#include "elapsed/elapsed.h"
#include "free.h"
#include "imm/imm.h"
#include "minmax.h"
#include "model.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "profile.h"
#include "result.h"
#include "seq_code.h"
#include "state_idx.h"

struct final_score
{
    imm_float     score;
    uint_fast16_t state;
    uint_fast8_t  seq_len;
};

static inline struct final_score best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                  uint_fast16_t tgt_state, uint_fast32_t row);
static inline struct final_score best_trans_score_first_row(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                            uint_fast16_t tgt_state);
static struct final_score        final_score(struct imm_dp const* dp, struct imm_dp_task* task);
static uint_fast8_t              max_seq(struct model_state const* const* mstates, uint16_t nstates);
static uint_fast8_t              min_seq(struct model_state const* const* mstates, uint16_t nstates);
static inline void               set_score(struct imm_dp const* dp, struct imm_dp_task* task, imm_float trans_score,
                                           uint_fast8_t min_len, uint_fast8_t max_len, uint_fast32_t row, uint_fast16_t state);
static void                      viterbi(struct imm_dp const* dp, struct imm_dp_task* task, struct imm_path* path);
static void viterbi_first_row(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t remain);
static void viterbi_first_row_safe(struct imm_dp const* dp, struct imm_dp_task* task);
static void viterbi_path(struct imm_dp const* dp, struct imm_dp_task const* task, struct imm_path* path,
                         uint_fast16_t end_state, uint_fast8_t end_seq_len);
static void _viterbi(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                     uint_fast32_t const stop_row);
static void _viterbi_safe(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                          uint_fast32_t const stop_row);

void imm_dp_destroy(struct imm_dp const* dp)
{
    free_c(dp->mstates);
    seq_code_destroy(dp->seq_code);
    state_idx_destroy(dp->state_idx);
    dp_emission_destroy(dp->emission);
    dp_trans_table_destroy(dp->trans_table);
    dp_state_table_destroy(dp->state_table);
    free_c(dp);
}

struct imm_result const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_dp_task* task)
{
    if (seq_code_abc(dp->seq_code) != imm_seq_get_abc(task->seq)) {
        imm_error("dp and seq must have the same alphabet");
        return NULL;
    }

    struct imm_state const* end_state = model_state_get_state(dp->mstates[dp->state_table->end_state]);
    if (imm_seq_length(task->seq) < imm_state_min_seq(end_state)) {
        imm_error("sequence is shorter than end_state's lower bound");
        return NULL;
    }

    struct imm_result* result = imm_result_create(task->seq);

    struct imm_path* path = imm_path_create();
    struct elapsed   elapsed = elapsed_init();
    elapsed_start(&elapsed);
    viterbi(dp, task, path);
    elapsed_end(&elapsed);
    result_set(result, path, (imm_float)elapsed_seconds(&elapsed));

    return result;
}

int imm_dp_change_trans(struct imm_dp* dp, struct imm_hmm* hmm, struct imm_state const* src_state,
                        struct imm_state const* tgt_state, imm_float lprob)
{
    if (!imm_lprob_is_valid(lprob)) {
        imm_error("invalid lprob");
        return 1;
    }

    imm_float prev = imm_hmm_get_trans(hmm, src_state, tgt_state);
    if (!imm_lprob_is_valid(prev)) {
        imm_error("transition does not exist");
        return 1;
    }
    if (imm_lprob_is_zero(prev) && imm_lprob_is_zero(lprob))
        return 0;

    uint_fast16_t src = state_idx_find(dp->state_idx, src_state);
    uint_fast16_t tgt = state_idx_find(dp->state_idx, tgt_state);

    if (dp_trans_table_change(dp->trans_table, src, tgt, (imm_float)lprob)) {
        imm_error("dp transition not found");
        return 1;
    }

    BUG(imm_hmm_set_trans(hmm, src_state, tgt_state, lprob) != 0);

    return 0;
}

struct imm_dp* dp_create(struct imm_abc const* abc, struct model_state const** mstates, uint16_t nstates,
                         struct imm_state const* end_state)
{
    struct imm_dp* dp = malloc(sizeof(*dp));
    dp->mstates = mstates;
    dp->seq_code = seq_code_create(abc, min_seq(mstates, nstates), max_seq(mstates, nstates));

    dp->state_idx = state_idx_create();
    dp->state_table = dp_state_table_create(mstates, nstates, end_state, dp->state_idx);
    dp->emission = dp_emission_create(dp->seq_code, mstates, nstates);
    dp->trans_table = dp_trans_table_create(mstates, nstates, dp->state_idx);

    return dp;
}

void dp_create_from_model(struct imm_model* model)
{
    struct imm_dp* dp = malloc(sizeof(*dp));

    dp->mstates = (struct model_state const**)model->mstates;
    dp->seq_code = model->seq_code;
    dp->emission = model->emission;
    dp->trans_table = model->trans_table;
    dp->state_table = model->state_table;

    dp->state_idx = state_idx_create();
    for (uint16_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i)
        state_idx_add(dp->state_idx, dp->mstates[i]->state, i);

    model->dp = dp;
}

struct dp_emission const* dp_get_emission(struct imm_dp const* dp) { return dp->emission; }

struct model_state const* const* dp_get_mstates(struct imm_dp const* dp) { return dp->mstates; }

struct seq_code const* dp_get_seq_code(struct imm_dp const* dp) { return dp->seq_code; }

struct dp_state_table const* dp_get_state_table(struct imm_dp const* dp) { return dp->state_table; }

struct dp_trans_table* dp_get_trans_table(struct imm_dp const* dp) { return dp->trans_table; }

static inline struct final_score best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                  uint_fast16_t tgt_state, uint_fast32_t row)
{
    imm_float     score = (imm_float)imm_lprob_zero();
    uint_fast16_t prev_state = INVALID_STATE;
    uint_fast8_t  prev_seq_len = INVALID_SEQ_LEN;

    for (uint_fast16_t i = 0; i < dp_trans_table_ntrans(dp->trans_table, tgt_state); ++i) {

        uint_fast16_t src_state = dp_trans_table_source_state(dp->trans_table, tgt_state, i);
        uint_fast8_t  min_seq = dp_state_table_min_seq(dp->state_table, src_state);

        if (UNLIKELY(row < min_seq) || (min_seq == 0 && src_state > tgt_state))
            continue;

        uint_fast8_t max_seq = dp_state_table_max_seq(dp->state_table, src_state);
        max_seq = (uint_fast8_t)MIN(max_seq, row);
        for (uint_fast8_t len = min_seq; len <= max_seq; ++len) {

            imm_float v0 = dp_matrix_get_score(matrix, row - len, src_state, len);
            imm_float v1 = dp_trans_table_score(dp->trans_table, tgt_state, i);
            imm_float v = v0 + v1;

            if (v > score) {
                score = v;
                prev_state = src_state;
                prev_seq_len = len;
            }
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

static inline struct final_score best_trans_score_first_row(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                            uint_fast16_t tgt_state)
{
    imm_float     score = (imm_float)imm_lprob_zero();
    uint_fast16_t prev_state = INVALID_STATE;
    uint_fast8_t  prev_seq_len = INVALID_SEQ_LEN;

    for (uint_fast16_t i = 0; i < dp_trans_table_ntrans(dp->trans_table, tgt_state); ++i) {

        uint_fast16_t src_state = dp_trans_table_source_state(dp->trans_table, tgt_state, i);
        uint_fast8_t  min_seq = dp_state_table_min_seq(dp->state_table, src_state);

        if (min_seq > 0 || src_state > tgt_state)
            continue;

        imm_float v0 = dp_matrix_get_score(matrix, 0, src_state, 0);
        imm_float v1 = dp_trans_table_score(dp->trans_table, tgt_state, i);
        imm_float v = v0 + v1;

        if (v > score) {
            score = v;
            prev_state = src_state;
            prev_seq_len = 0;
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

static struct final_score final_score(struct imm_dp const* dp, struct imm_dp_task* task)
{
    imm_float     score = (imm_float)imm_lprob_zero();
    uint_fast16_t end_state = dp_state_table_end_state(dp->state_table);

    uint_fast16_t final_state = INVALID_STATE;
    uint_fast8_t  final_seq_len = INVALID_SEQ_LEN;

    uint_fast32_t length = eseq_length(task->eseq);
    uint_fast8_t  max_seq = dp_state_table_max_seq(dp->state_table, end_state);

    for (uint_fast8_t len = (uint_fast8_t)MIN(max_seq, length);; --len) {

        imm_float s = dp_matrix_get_score(task->matrix, length - len, end_state, len);
        if (s > score) {
            score = s;
            final_state = end_state;
            final_seq_len = len;
        }

        if (dp_state_table_min_seq(dp->state_table, end_state) == len)
            break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == INVALID_STATE)
        fscore.score = (imm_float)imm_lprob_invalid();

    return fscore;
}

static uint_fast8_t max_seq(struct model_state const* const* mstates, uint16_t nstates)
{
    uint_fast8_t max = imm_state_max_seq(model_state_get_state(mstates[0]));
    for (uint_fast16_t i = 1; i < nstates; ++i)
        max = (uint_fast8_t)MAX(max, imm_state_max_seq(model_state_get_state(mstates[i])));

    return max;
}

static uint_fast8_t min_seq(struct model_state const* const* mstates, uint16_t nstates)
{
    uint_fast8_t min = imm_state_min_seq(model_state_get_state(mstates[0]));
    for (uint_fast16_t i = 1; i < nstates; ++i)
        min = (uint_fast8_t)MIN(min, imm_state_min_seq(model_state_get_state(mstates[i])));

    return min;
}

static inline void set_score(struct imm_dp const* dp, struct imm_dp_task* task, imm_float trans_score,
                             uint_fast8_t min_len, uint_fast8_t max_len, uint_fast32_t row, uint_fast16_t state)
{
    for (uint_fast8_t len = min_len; len <= max_len; ++len) {

        uint_fast16_t seq_code = eseq_get(task->eseq, row, len);
        seq_code -= seq_code_offset(dp->seq_code, min_len);

        imm_float score = trans_score + dp_emission_score(dp->emission, state, seq_code);
        dp_matrix_set_score(task->matrix, row, state, len, score);
    }
}

static void viterbi(struct imm_dp const* dp, struct imm_dp_task* task, struct imm_path* path)
{
    uint_fast32_t const len = eseq_length(task->eseq);

    if (len >= 1 + DP_STATE_TABLE_MAX_SEQ) {
        viterbi_first_row_safe(dp, task);
        _viterbi_safe(dp, task, 1, len - DP_STATE_TABLE_MAX_SEQ);
        _viterbi(dp, task, len - DP_STATE_TABLE_MAX_SEQ + 1, len);
    } else {
        viterbi_first_row(dp, task, len);
        _viterbi(dp, task, 1, len);
    }

    struct final_score const fscore = final_score(dp, task);
    viterbi_path(dp, task, path, fscore.state, fscore.seq_len);
}

static void viterbi_first_row(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t remain)
{
    for (uint_fast16_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

        struct final_score tscore = best_trans_score_first_row(dp, task->matrix, i);
        dp_matrix_set_prev_step(task->matrix, 0, i, tscore.state, tscore.seq_len);

        uint_fast8_t min_len = dp_state_table_min_seq(dp->state_table, i);
        uint_fast8_t max_len = (uint_fast8_t)MIN(dp_state_table_max_seq(dp->state_table, i), remain);

        tscore.score = MAX(dp_state_table_start_lprob(dp->state_table, i), tscore.score);
        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_first_row_safe(struct imm_dp const* dp, struct imm_dp_task* task)
{
    for (uint_fast16_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

        struct final_score tscore = best_trans_score_first_row(dp, task->matrix, i);
        dp_matrix_set_prev_step(task->matrix, 0, i, tscore.state, tscore.seq_len);

        uint_fast8_t min_len = dp_state_table_min_seq(dp->state_table, i);
        uint_fast8_t max_len = dp_state_table_max_seq(dp->state_table, i);

        tscore.score = MAX(dp_state_table_start_lprob(dp->state_table, i), tscore.score);
        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_path(struct imm_dp const* dp, struct imm_dp_task const* task, struct imm_path* path,
                         uint_fast16_t end_state, uint_fast8_t end_seq_len)
{
    uint_fast32_t row = eseq_length(task->eseq);
    uint_fast16_t state = end_state;
    uint_fast8_t  seq_len = end_seq_len;

    while (seq_len != INVALID_SEQ_LEN) {
        struct imm_state const* s = model_state_get_state(dp->mstates[state]);
        struct imm_step*        new_step = imm_step_create(s, seq_len);
        imm_path_prepend(path, new_step);
        row -= seq_len;
        dp_matrix_get_prev_step(task->matrix, row, state, &state, &seq_len);
    }
}

static void _viterbi(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                     uint_fast32_t const stop_row)
{
    for (uint_fast32_t r = start_row; r <= stop_row; ++r) {

        for (uint_fast16_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

            struct final_score tscore = best_trans_score(dp, task->matrix, i, r);
            dp_matrix_set_prev_step(task->matrix, r, i, tscore.state, tscore.seq_len);

            uint_fast8_t min_len = dp_state_table_min_seq(dp->state_table, i);
            uint_fast8_t max_len = (uint_fast8_t)MIN(dp_state_table_max_seq(dp->state_table, i), stop_row - r);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}

static void _viterbi_safe(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                          uint_fast32_t const stop_row)
{
    for (uint_fast32_t r = start_row; r <= stop_row; ++r) {

        for (uint_fast16_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

            struct final_score tscore = best_trans_score(dp, task->matrix, i, r);
            dp_matrix_set_prev_step(task->matrix, r, i, tscore.state, tscore.seq_len);

            uint_fast8_t min_len = dp_state_table_min_seq(dp->state_table, i);
            uint_fast8_t max_len = dp_state_table_max_seq(dp->state_table, i);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}
