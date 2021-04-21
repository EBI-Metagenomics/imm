#include "dp.h"
#include "cpath.h"
#include "dp_emission.h"
#include "dp_matrix.h"
#include "dp_state_table.h"
#include "dp_task.h"
#include "dp_trans_table.h"
#include "elapsed/elapsed.h"
#include "hmm.h"
#include "imm/imm.h"
#include "model.h"
#include "profile.h"
#include "result.h"
#include "seq_code.h"
#include "state_idx.h"
#include "std.h"

struct final_score
{
    imm_float score;
    unsigned  state;
    unsigned  seq_len;
};

static inline struct final_score best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                  unsigned tgt_state, uint_fast32_t row, uint16_t* best_trans,
                                                  uint8_t* best_len);
static inline struct final_score best_trans_score_first_row(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                            unsigned tgt_state, uint16_t* best_trans,
                                                            uint8_t* best_len);
static struct final_score        final_score(struct imm_dp const* dp, struct imm_dp_task* task);
static unsigned                  max_seq(struct imm_state** states, uint16_t nstates);
static unsigned                  min_seq(struct imm_state** states, uint16_t nstates);
static inline void set_score(struct imm_dp const* dp, struct imm_dp_task* task, imm_float trans_score, unsigned min_len,
                             unsigned max_len, uint_fast32_t row, unsigned state);
static void        viterbi(struct imm_dp const* dp, struct imm_dp_task* task, struct imm_path* path);
static void        viterbi_first_row(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t remain);
static void        viterbi_first_row_safe(struct imm_dp const* dp, struct imm_dp_task* task);
static void        viterbi_path(struct imm_dp const* dp, struct imm_dp_task const* task, struct imm_path* path,
                                unsigned end_state, unsigned end_seq_len);
static void        _viterbi(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                            uint_fast32_t const stop_row);
static void        _viterbi_safe(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                                 uint_fast32_t const stop_row);

void imm_dp_destroy(struct imm_dp const* dp)
{
    free((void*)dp->states);
    seq_code_destroy(dp->seq_code);
    state_idx_destroy(dp->state_idx);
    dp_emission_destroy(dp->emission);
    dp_trans_table_destroy(dp->trans_table);
    dp_state_table_destroy(dp->state_table);
    free((void*)dp);
}

struct imm_result const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_dp_task* task)
{
    if (seq_code_abc(dp->seq_code) != imm_seq_get_abc(task->seq)) {
        error("dp and seq must have the same alphabet");
        return NULL;
    }

    struct imm_state const* end_state = dp->states[dp->state_table->end_state];
    if (imm_seq_length(task->seq) < imm_state_min_seq(end_state)) {
        error("sequence is shorter than end_state's lower bound");
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

int imm_dp_change_trans(struct imm_dp* dp, struct imm_hmm* hmm, struct imm_state* src_state,
                        struct imm_state* tgt_state, imm_float lprob)
{
    if (!imm_lprob_is_valid(lprob)) {
        error("invalid lprob");
        return IMM_ILLEGALARG;
    }

    imm_float prev = imm_hmm_get_trans(hmm, src_state, tgt_state);
    if (!imm_lprob_is_valid(prev)) {
        error("transition does not exist");
        return IMM_ILLEGALARG;
    }
    if (imm_lprob_is_zero(prev) && imm_lprob_is_zero(lprob))
        return IMM_SUCCESS;

    unsigned src = state_idx_find(dp->state_idx, src_state);
    unsigned tgt = state_idx_find(dp->state_idx, tgt_state);

    if (dp_trans_table_change(dp->trans_table, src, tgt, (imm_float)lprob)) {
        error("dp transition not found");
        return 1;
    }

    BUG(!imm_hmm_set_trans(hmm, src_state, tgt_state, lprob));

    return IMM_SUCCESS;
}

struct imm_dp* dp_create(struct imm_hmm const* hmm, struct imm_state** states, struct imm_state const* end_state)
{
    struct imm_dp* dp = xmalloc(sizeof(*dp));
    dp->states = states;
    dp->seq_code = seq_code_create(hmm->abc, min_seq(states, hmm->nstates), max_seq(states, hmm->nstates));
    BUG(!dp->seq_code);

    dp->state_idx = NULL;
    dp->state_table = NULL;
    dp->emission = NULL;
    dp->trans_table = NULL;

    dp->state_idx = state_idx_create(hmm->nstates);
    if (!dp->state_idx)
        goto err;

    dp->state_table = dp_state_table_create(hmm, states, end_state, dp->state_idx);
    if (!dp->state_table)
        goto err;

    dp->emission = dp_emission_create(dp->seq_code, states, hmm->nstates);
    if (!dp->emission)
        goto err;

    dp->trans_table = dp_trans_table_create(hmm, states, dp->state_idx);
    if (!dp->trans_table)
        goto err;

    return dp;

err:
    if (dp->trans_table)
        dp_trans_table_destroy(dp->trans_table);

    if (dp->emission)
        dp_emission_destroy(dp->emission);

    if (dp->state_table)
        dp_state_table_destroy(dp->state_table);

    if (dp->state_idx)
        state_idx_destroy(dp->state_idx);

    if (dp->seq_code)
        seq_code_destroy(dp->seq_code);
    free(dp);
    return NULL;
}

void dp_create_from_model(struct imm_model* model)
{
    struct imm_dp* dp = xmalloc(sizeof(*dp));

    dp->states = model->states;
    dp->seq_code = model->seq_code;
    dp->emission = model->emission;
    dp->trans_table = model->trans_table;
    dp->state_table = model->state_table;

    dp->state_idx = state_idx_create(dp_state_table_nstates(dp->state_table));
    for (uint16_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i)
        state_idx_add(dp->state_idx, dp->states[i]);

    model->dp = dp;
}

struct dp_emission const* dp_get_emission(struct imm_dp const* dp) { return dp->emission; }

struct imm_state** dp_get_states(struct imm_dp const* dp) { return dp->states; }

struct seq_code const* dp_get_seq_code(struct imm_dp const* dp) { return dp->seq_code; }

struct dp_state_table const* dp_get_state_table(struct imm_dp const* dp) { return dp->state_table; }

struct dp_trans_table* dp_get_trans_table(struct imm_dp const* dp) { return dp->trans_table; }

static inline struct final_score best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                  unsigned tgt_state, uint_fast32_t row, uint16_t* best_trans,
                                                  uint8_t* best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned  prev_state = INVALID_STATE;
    unsigned  prev_seq_len = INVALID_SEQ_LEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < dp_trans_table_ntrans(dp->trans_table, tgt_state); ++i) {

        unsigned src_state = dp_trans_table_source_state(dp->trans_table, tgt_state, i);
        unsigned min_seq = dp_state_table_min_seq(dp->state_table, src_state);

        if (UNLIKELY(row < min_seq) || (min_seq == 0 && src_state > tgt_state))
            continue;

        unsigned max_seq = dp_state_table_max_seq(dp->state_table, src_state);
        max_seq = (unsigned)MIN(max_seq, row);
        for (unsigned len = min_seq; len <= max_seq; ++len) {

            imm_float v0 = dp_matrix_get_score(matrix, row - len, src_state, len);
            imm_float v1 = dp_trans_table_score(dp->trans_table, tgt_state, i);
            imm_float v = v0 + v1;

            if (v > score) {
                score = v;
                prev_state = src_state;
                prev_seq_len = len;
                *best_trans = (uint16_t)i;
                *best_len = (uint8_t)(len - min_seq);
            }
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

static inline struct final_score best_trans_score_first_row(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                                            unsigned tgt_state, uint16_t* best_trans, uint8_t* best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned  prev_state = INVALID_STATE;
    unsigned  prev_seq_len = INVALID_SEQ_LEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < dp_trans_table_ntrans(dp->trans_table, tgt_state); ++i) {

        unsigned src_state = dp_trans_table_source_state(dp->trans_table, tgt_state, i);
        unsigned min_seq = dp_state_table_min_seq(dp->state_table, src_state);

        if (min_seq > 0 || src_state > tgt_state)
            continue;

        imm_float v0 = dp_matrix_get_score(matrix, 0, src_state, 0);
        imm_float v1 = dp_trans_table_score(dp->trans_table, tgt_state, i);
        imm_float v = v0 + v1;

        if (v > score) {
            score = v;
            prev_state = src_state;
            prev_seq_len = 0;
            *best_trans = (uint16_t)i;
            *best_len = 0;
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

static struct final_score final_score(struct imm_dp const* dp, struct imm_dp_task* task)
{
    imm_float score = imm_lprob_zero();
    unsigned  end_state = dp_state_table_end_state(dp->state_table);

    unsigned final_state = INVALID_STATE;
    unsigned final_seq_len = INVALID_SEQ_LEN;

    uint_fast32_t length = eseq_length(task->eseq);
    unsigned      max_seq = dp_state_table_max_seq(dp->state_table, end_state);

    for (unsigned len = (unsigned)MIN(max_seq, length);; --len) {

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

static unsigned max_seq(struct imm_state** states, uint16_t nstates)
{
    unsigned max = imm_state_max_seq(states[0]);
    for (unsigned i = 1; i < nstates; ++i)
        max = (unsigned)MAX(max, imm_state_max_seq(states[i]));

    return max;
}

static unsigned min_seq(struct imm_state** states, uint16_t nstates)
{
    unsigned min = imm_state_min_seq(states[0]);
    for (unsigned i = 1; i < nstates; ++i)
        min = (unsigned)MIN(min, imm_state_min_seq(states[i]));

    return min;
}

static inline void set_score(struct imm_dp const* dp, struct imm_dp_task* task, imm_float trans_score, unsigned min_len,
                             unsigned max_len, uint_fast32_t row, unsigned state)
{
    for (unsigned len = min_len; len <= max_len; ++len) {

        unsigned seq_code = eseq_get(task->eseq, row, len);
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
    for (unsigned i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

        uint16_t           trans = 0;
        uint8_t            len = 0;
        struct final_score tscore = best_trans_score_first_row(dp, task->matrix, i, &trans, &len);
        if (tscore.state != INVALID_STATE) {
            cpath_set_trans(&task->cpath, 0, i, trans);
            cpath_set_len(&task->cpath, 0, i, len);
            BUG(cpath_get_trans(&task->cpath, 0, i) != trans);
            BUG(cpath_get_len(&task->cpath, 0, i) != len);
        } else {
            cpath_set_invalid(&task->cpath, 0, i);
            BUG(cpath_valid(&task->cpath, 0, i));
        }

        unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
        unsigned max_len = (unsigned)MIN(dp_state_table_max_seq(dp->state_table, i), remain);

        if (dp->state_table->start_state == i)
            tscore.score = MAX(dp->state_table->start_lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_first_row_safe(struct imm_dp const* dp, struct imm_dp_task* task)
{
    for (unsigned i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

        uint16_t           trans = 0;
        uint8_t            len = 0;
        struct final_score tscore = best_trans_score_first_row(dp, task->matrix, i, &trans, &len);
        if (tscore.state != INVALID_STATE) {
            cpath_set_trans(&task->cpath, 0, i, trans);
            cpath_set_len(&task->cpath, 0, i, len);
            BUG(cpath_get_trans(&task->cpath, 0, i) != trans);
            BUG(cpath_get_len(&task->cpath, 0, i) != len);
        } else {
            cpath_set_invalid(&task->cpath, 0, i);
            BUG(cpath_valid(&task->cpath, 0, i));
        }

        unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
        unsigned max_len = dp_state_table_max_seq(dp->state_table, i);

        if (dp->state_table->start_state == i)
            tscore.score = MAX(dp->state_table->start_lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_path(struct imm_dp const* dp, struct imm_dp_task const* task, struct imm_path* path,
                         unsigned end_state, unsigned end_seq_len)
{
    uint_fast32_t row = eseq_length(task->eseq);
    unsigned      state = end_state;
    unsigned      seq_len = end_seq_len;
    bool          valid = seq_len != INVALID_SEQ_LEN;

    while (valid) {
        struct imm_state const* s = dp->states[state];
        struct imm_step*        new_step = imm_step_create(s, (uint8_t)seq_len);
        imm_path_prepend(path, new_step);
        row -= seq_len;

        valid = cpath_valid(&task->cpath, row, state);
        if (valid) {
            unsigned trans = (unsigned)cpath_get_trans(&task->cpath, row, state);
            unsigned len = (unsigned)cpath_get_len(&task->cpath, row, state);
            state = dp_trans_table_source_state(dp->trans_table, state, trans);
            seq_len = (unsigned)(len + dp_state_table_min_seq(dp->state_table, state));
        }
    }
}

static void _viterbi(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                     uint_fast32_t const stop_row)
{
    for (uint_fast32_t r = start_row; r <= stop_row; ++r) {

        for (unsigned i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

            uint16_t           trans = 0;
            uint8_t            len = 0;
            struct final_score tscore = best_trans_score(dp, task->matrix, i, r, &trans, &len);
            if (tscore.state != INVALID_STATE) {
                cpath_set_trans(&task->cpath, r, i, trans);
                cpath_set_len(&task->cpath, r, i, len);
                BUG(cpath_get_trans(&task->cpath, r, i) != trans);
                BUG(cpath_get_len(&task->cpath, r, i) != len);
            } else {
                cpath_set_invalid(&task->cpath, r, i);
                BUG(cpath_valid(&task->cpath, r, i));
            }

            unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
            unsigned max_len = (unsigned)MIN(dp_state_table_max_seq(dp->state_table, i), stop_row - r);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}

static void _viterbi_safe(struct imm_dp const* dp, struct imm_dp_task* task, uint_fast32_t const start_row,
                          uint_fast32_t const stop_row)
{
    for (uint_fast32_t r = start_row; r <= stop_row; ++r) {

        for (unsigned i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

            uint16_t           trans = 0;
            uint8_t            len = 0;
            struct final_score tscore = best_trans_score(dp, task->matrix, i, r, &trans, &len);
            if (tscore.state != INVALID_STATE) {
                cpath_set_trans(&task->cpath, r, i, trans);
                cpath_set_len(&task->cpath, r, i, len);
                BUG(cpath_get_trans(&task->cpath, r, i) != trans);
                BUG(cpath_get_len(&task->cpath, r, i) != len);
            } else {
                cpath_set_invalid(&task->cpath, r, i);
                BUG(cpath_valid(&task->cpath, r, i));
            }

            unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
            unsigned max_len = dp_state_table_max_seq(dp->state_table, i);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}
