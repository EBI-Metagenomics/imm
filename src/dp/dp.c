#include "dp/dp.h"
#include "dp/emis.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "elapsed/elapsed.h"
#include "error.h"
#include "imm/dp.h"
#include "imm/lprob.h"
#include "imm/prod.h"
#include "imm/state.h"
#include "imm/subseq.h"
#include "imm/trans.h"
#include "task.h"
#include <assert.h>
#include <limits.h>

struct final_score
{
    imm_float score;
    unsigned state;
    unsigned seq_len;
};

struct unsafe_pair
{
    unsigned src;
    unsigned dst;
    unsigned trans;
};

static unsigned find_unsafe_states(struct imm_dp const *dp,
                                   struct unsafe_pair *x)
{
    unsigned n = 0;
    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        for (unsigned t = 0; t < trans_table_ntrans(&dp->trans_table, dst); ++t)
        {
            unsigned src = trans_table_source_state(&dp->trans_table, dst, t);
            struct span span = state_table_span(&dp->state_table, src);
            if (span.min == 0 && dst < src)
            {
                n++;
                x->src = src;
                x->dst = dst;
                x->trans = t;
            }
        }
    }
    return n;
}

static struct final_score best_trans_score(struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned tgt_state, unsigned row,
                                           uint16_t *best_trans,
                                           uint8_t *best_len);

static struct final_score
best_trans_score_first_row(struct imm_dp const *dp, struct matrix const *matrix,
                           unsigned tgt_state, uint16_t *best_trans,
                           uint8_t *best_len);

static struct final_score final_score(struct imm_dp const *dp,
                                      struct imm_task *task);

static void set_score(struct imm_dp const *dp, struct imm_task *task,
                      imm_float trans_score, unsigned min_len, unsigned max_len,
                      unsigned row, unsigned state)
{
    for (unsigned len = min_len; len <= max_len; ++len)
    {
        unsigned seq_code = eseq_get(&task->eseq, row, len, min_len);
        imm_float score = trans_score + emis_score(&dp->emis, state, seq_code);
        matrix_set_score(&task->matrix, row, state, len, score);
    }
}

static enum imm_rc viterbi(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_prod *prod);
static void viterbi_first_row(struct imm_dp const *dp, struct imm_task *task,
                              unsigned remain);
static void viterbi_first_row_safe(struct imm_dp const *dp,
                                   struct imm_task *task);
static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
                                struct imm_path *path, unsigned end_state,
                                unsigned end_seq_len);
static void _viterbi(struct imm_dp const *dp, struct imm_task *task,
                     unsigned const start_row, unsigned const stop_row,
                     unsigned seqlen, struct unsafe_pair *upair);
static void _viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                          unsigned const start_row, unsigned const stop_row,
                          struct unsafe_pair *upair);

void imm_dp_init(struct imm_dp *dp, struct imm_code const *code)
{
    dp->code = code;
    imm_emis_init(&dp->emis);
    imm_trans_table_init(&dp->trans_table);
    imm_state_table_init(&dp->state_table);
}

void imm_dp_del(struct imm_dp *dp)
{
    if (dp)
    {
        imm_emis_del(&dp->emis);
        imm_trans_table_del(&dp->trans_table);
        imm_state_table_del(&dp->state_table);
    }
}

enum imm_rc imm_dp_reset(struct imm_dp *dp, struct dp_args const *args)
{
    enum imm_rc rc = IMM_OK;

    if ((rc = imm_emis_reset(&dp->emis, dp->code, args->states, args->nstates)))
        return rc;

    if ((rc = imm_trans_table_reset(&dp->trans_table, args))) return rc;

    if ((rc = imm_state_table_reset(&dp->state_table, args))) return rc;

    return rc;
}

enum imm_rc imm_dp_viterbi(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_prod *prod)
{
    imm_prod_reset(prod);
    if (!task->seq) return error(IMM_NOT_SET_SEQ);

    if (dp->code->abc != imm_seq_abc(task->seq))
        return error(IMM_DIFFERENT_ABC);

    unsigned end_state = dp->state_table.end_state_idx;
    unsigned min = state_table_span(&dp->state_table, end_state).min;
    if (imm_seq_size(task->seq) < min) return error(IMM_SEQ_TOO_SHORT);

    struct elapsed elapsed = ELAPSED_INIT;
    if (elapsed_start(&elapsed)) return error(IMM_ELAPSED_LIB_FAILED);

    enum imm_rc rc = viterbi(dp, task, prod);

    if (elapsed_stop(&elapsed)) return error(IMM_ELAPSED_LIB_FAILED);

    prod->mseconds = elapsed_milliseconds(&elapsed);

    return rc;
}

unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx, unsigned dst_idx)
{
    return imm_trans_table_idx(&dp->trans_table, src_idx, dst_idx);
}

enum imm_rc imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx,
                                imm_float lprob)
{
    if (imm_unlikely(imm_lprob_is_nan(lprob)))
        return error(IMM_NAN_PROBABILITY);

    trans_table_change(&dp->trans_table, trans_idx, lprob);
    return IMM_OK;
}

imm_float imm_dp_emis_score(struct imm_dp const *dp, unsigned state_id,
                            struct imm_seq const *seq)
{
    struct eseq eseq;
    imm_eseq_init(&eseq, dp->code);
    imm_float score = IMM_LPROB_NAN;
    if (imm_eseq_setup(&eseq, seq)) goto cleanup;

    unsigned state_idx = imm_state_table_idx(&dp->state_table, state_id);
    unsigned min = state_table_span(&dp->state_table, state_idx).min;

    unsigned seq_code = eseq_get(&eseq, 0, imm_seq_size(seq), min);
    score = emis_score(&dp->emis, state_idx, seq_code);

cleanup:
    imm_eseq_del(&eseq);
    return score;
}

imm_float imm_dp_trans_score(struct imm_dp const *dp, unsigned src,
                             unsigned dst)
{
    unsigned src_idx = imm_state_table_idx(&dp->state_table, src);
    unsigned dst_idx = imm_state_table_idx(&dp->state_table, dst);

    if (src_idx == UINT_MAX || dst_idx == UINT_MAX) return IMM_LPROB_NAN;

    for (unsigned i = 0; i < dp->trans_table.ntrans; ++i)
    {
        if (trans_table_source_state(&dp->trans_table, dst_idx, i) == src_idx)
            return trans_table_score(&dp->trans_table, dst_idx, i);
    }
    return IMM_LPROB_NAN;
}

void imm_dp_write_dot(struct imm_dp const *dp, FILE *restrict fd,
                      imm_state_name *name)
{
    fprintf(fd, "digraph hmm {\n");
    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        for (unsigned t = 0; t < trans_table_ntrans(&dp->trans_table, dst); ++t)
        {
            unsigned src = trans_table_source_state(&dp->trans_table, dst, t);

            char src_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                                  '\0', '\0', '\0', '\0'};
            char dst_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                                  '\0', '\0', '\0', '\0'};

            (*name)(state_table_id(&dp->state_table, src), src_name);
            (*name)(state_table_id(&dp->state_table, dst), dst_name);
            fprintf(fd, "%s -> %s [label=%.4f];\n", src_name, dst_name,
                    trans_table_score(&dp->trans_table, dst, t));
        }
    }
    fprintf(fd, "}\n");
}

static struct final_score best_trans_score(struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned dst, unsigned row,
                                           uint16_t *best_trans,
                                           uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = IMM_STATE_NULL_IDX;
    unsigned prev_seqlen = IMM_STATE_NULL_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {
        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_span(&dp->state_table, src).min;

        if (imm_unlikely(row < min_seq)) continue;
        // if (imm_unlikely(row < min_seq) || (min_seq == 0 && src > dst))
        //     continue;

        unsigned max_seq = state_table_span(&dp->state_table, src).max;
        max_seq = (unsigned)MIN(max_seq, row);
        for (unsigned len = min_seq; len <= max_seq; ++len)
        {

            imm_float v0 = matrix_get_score(matrix, row - len, src, len);
            imm_float v1 = trans_table_score(&dp->trans_table, dst, i);
            imm_float v = v0 + v1;

            if (v > score)
            {
                score = v;
                prev_state = src;
                prev_seqlen = len;
                *best_trans = (uint16_t)i;
                *best_len = (uint8_t)(len - min_seq);
            }
        }
    }

    return (struct final_score){score, prev_state, prev_seqlen};
}

static struct final_score
best_trans_score_first_row(struct imm_dp const *dp, struct matrix const *matrix,
                           unsigned dst, uint16_t *best_trans,
                           uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = IMM_STATE_NULL_IDX;
    unsigned prev_seq_len = IMM_STATE_NULL_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {

        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_span(&dp->state_table, src).min;

        if (min_seq > 0 || src > dst) continue;

        imm_float v0 = matrix_get_score(matrix, 0, src, 0);
        imm_float v1 = trans_table_score(&dp->trans_table, dst, i);
        imm_float v = v0 + v1;

        if (v > score)
        {
            score = v;
            prev_state = src;
            prev_seq_len = 0;
            *best_trans = (uint16_t)i;
            *best_len = 0;
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

static struct final_score final_score(struct imm_dp const *dp,
                                      struct imm_task *task)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state_idx;

    unsigned final_state = IMM_STATE_NULL_IDX;
    unsigned final_seq_len = IMM_STATE_NULL_SEQLEN;

    unsigned length = eseq_len(&task->eseq);
    unsigned max_seq = state_table_span(&dp->state_table, end_state).max;

    for (unsigned len = (unsigned)MIN(max_seq, length);; --len)
    {

        imm_float s =
            matrix_get_score(&task->matrix, length - len, end_state, len);
        if (s > score)
        {
            score = s;
            final_state = end_state;
            final_seq_len = len;
        }

        if (state_table_span(&dp->state_table, end_state).min == len) break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == IMM_STATE_NULL_IDX)
        fscore.score = (imm_float)imm_lprob_nan();

    return fscore;
}

static enum imm_rc viterbi(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_prod *prod)
{
    unsigned len = eseq_len(&task->eseq);

    struct unsafe_pair unsafe_pair = {0};
    unsigned num_unsafe_states = find_unsafe_states(dp, &unsafe_pair);
    struct unsafe_pair *upair = num_unsafe_states > 0 ? &unsafe_pair : NULL;
    assert(num_unsafe_states <= 1);

    if (len >= 1 + IMM_STATE_MAX_SEQLEN)
    {
        viterbi_first_row_safe(dp, task);
        _viterbi_safe(dp, task, 1, len - IMM_STATE_MAX_SEQLEN, upair);
        _viterbi(dp, task, len - IMM_STATE_MAX_SEQLEN + 1, len, len, upair);
    }
    else
    {
        viterbi_first_row(dp, task, len);
        _viterbi(dp, task, 1, len, len, upair);
    }

    // printf("B=0,X=1,E=0\n");
    // imm_matrix_dump(&task->matrix, stdout);

    struct final_score const fscore = final_score(dp, task);
    prod->loglik = fscore.score;
    return viterbi_path(dp, task, &prod->path, fscore.state, fscore.seq_len);
}

static void viterbi_first_row(struct imm_dp const *dp, struct imm_task *task,
                              unsigned remain)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {

        uint16_t trans = 0;
        uint8_t len = 0;
        struct final_score tscore =
            best_trans_score_first_row(dp, &task->matrix, i, &trans, &len);
        if (tscore.state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&task->path, 0, i, trans);
            path_set_seqlen(&task->path, 0, i, len);
            assert(path_trans(&task->path, 0, i) == trans);
            assert(path_seqlen(&task->path, 0, i) == len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            assert(!path_valid(&task->path, 0, i));
        }

        unsigned min_len = state_table_span(&dp->state_table, i).min;
        unsigned max_len =
            (unsigned)MIN(state_table_span(&dp->state_table, i).max, remain);

        if (dp->state_table.start.state == i)
            tscore.score = MAX(dp->state_table.start.lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_first_row_safe(struct imm_dp const *dp,
                                   struct imm_task *task)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {

        uint16_t trans = 0;
        uint8_t len = 0;
        struct final_score tscore =
            best_trans_score_first_row(dp, &task->matrix, i, &trans, &len);
        if (tscore.state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&task->path, 0, i, trans);
            path_set_seqlen(&task->path, 0, i, len);
            assert(path_trans(&task->path, 0, i) == trans);
            assert(path_seqlen(&task->path, 0, i) == len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            assert(!path_valid(&task->path, 0, i));
        }

        unsigned min_len = state_table_span(&dp->state_table, i).min;
        unsigned max_len = state_table_span(&dp->state_table, i).max;

        if (dp->state_table.start.state == i)
            tscore.score = MAX(dp->state_table.start.lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
                                struct imm_path *path, unsigned end_state,
                                unsigned end_seq_len)
{
    unsigned row = eseq_len(&task->eseq);
    unsigned state = end_state;
    unsigned seqlen = end_seq_len;
    bool valid = seqlen != IMM_STATE_NULL_SEQLEN;

    while (valid)
    {
        unsigned id = dp->state_table.ids[state];
        struct imm_step step = imm_step(id, seqlen);
        enum imm_rc rc = imm_path_add(path, step);
        if (rc) return rc;
        row -= seqlen;

        valid = path_valid(&task->path, row, state);
        if (valid)
        {
            unsigned trans = path_trans(&task->path, row, state);
            unsigned len = path_seqlen(&task->path, row, state);
            state = trans_table_source_state(&dp->trans_table, state, trans);
            seqlen = len + state_table_span(&dp->state_table, state).min;
        }
    }
    imm_path_reverse(path);
    return IMM_OK;
}

static inline void _viti(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const r, unsigned i, unsigned remain)
{
    uint16_t trans = 0;
    uint8_t len = 0;
    struct final_score tscore =
        best_trans_score(dp, &task->matrix, i, r, &trans, &len);
    if (tscore.state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&task->path, r, i, trans);
        path_set_seqlen(&task->path, r, i, len);
        assert(path_trans(&task->path, r, i) == trans);
        assert(path_seqlen(&task->path, r, i) == len);
    }
    else
    {
        path_invalidate(&task->path, r, i);
        assert(!path_valid(&task->path, r, i));
    }

    unsigned min_len = state_table_span(&dp->state_table, i).min;
    unsigned max_len =
        (unsigned)MIN(state_table_span(&dp->state_table, i).max, remain);

    set_score(dp, task, tscore.score, min_len, max_len, r, i);
}

static void _viterbi(struct imm_dp const *dp, struct imm_task *task,
                     unsigned const start_row, unsigned const stop_row,
                     unsigned seqlen, struct unsafe_pair *upair)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if ((r > 0 && r < seqlen) && upair)
        // if (r > 0 && upair)
        {
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
            _viti(dp, task, r, upair->src, stop_row - r);
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti(dp, task, r, i, stop_row - r);
        }
    }
}

static inline void _viti_safe(struct imm_dp const *dp, struct imm_task *task,
                              unsigned const r, unsigned i)
{
    uint16_t trans = 0;
    uint8_t len = 0;
    struct final_score tscore =
        best_trans_score(dp, &task->matrix, i, r, &trans, &len);
    if (tscore.state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&task->path, r, i, trans);
        path_set_seqlen(&task->path, r, i, len);
        assert(path_trans(&task->path, r, i) == trans);
        assert(path_seqlen(&task->path, r, i) == len);
    }
    else
    {
        path_invalidate(&task->path, r, i);
        assert(!path_valid(&task->path, r, i));
    }

    unsigned min_len = state_table_span(&dp->state_table, i).min;
    unsigned max_len = state_table_span(&dp->state_table, i).max;

    set_score(dp, task, tscore.score, min_len, max_len, r, i);
}

static void _viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                          unsigned const start_row, unsigned const stop_row,
                          struct unsafe_pair *upair)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if (r > 0 && upair)
        {
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
            _viti_safe(dp, task, r, upair->src);
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe(dp, task, r, i);
        }
    }
}

void imm_dp_dump_state_table(struct imm_dp const *dp)
{
    imm_state_table_dump(&dp->state_table);
}

void imm_dp_dump_path(struct imm_dp const *dp, struct imm_task const *task,
                      struct imm_prod const *prod, imm_state_name *callb)
{
    char name[IMM_STATE_NAME_SIZE] = {0};
    unsigned begin = 0;
    for (unsigned i = 0; i < imm_path_nsteps(&prod->path); ++i)
    {
        struct imm_step const *step = imm_path_step(&prod->path, i);
        unsigned idx = imm_state_table_idx(&dp->state_table, step->state_id);

        unsigned min = state_table_span(&dp->state_table, idx).min;
        unsigned seq_code = eseq_get(&task->eseq, begin, step->seqlen, min);

        imm_float score = emis_score(&dp->emis, idx, seq_code);
        struct imm_seq subseq = imm_subseq(task->seq, begin, step->seqlen);
        (*callb)(step->state_id, name);
        printf("<%s,%.*s,%.4f>\n", name, subseq.size, subseq.str, score);
        begin += step->seqlen;
    }
}
