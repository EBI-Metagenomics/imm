#include "imm/dp.h"
#include "elapsed/elapsed.h"
#include "error.h"
#include "imm/dp.h"
#include "imm/dp_args.h"
#include "imm/emis.h"
#include "imm/lprob.h"
#include "imm/matrix.h"
#include "imm/prod.h"
#include "imm/state.h"
#include "imm/state_table.h"
#include "imm/subseq.h"
#include "imm/trans.h"
#include "imm/trans_table.h"
#include "imm/viterbi.h"
#include "minmax.h"
#include "span.h"
#include "task.h"
#include "viterbi_generic.h"
#include <assert.h>
#include <limits.h>

static int call_viterbi(struct imm_viterbi const *, struct imm_prod *prod);
static int viterbi_path(struct imm_dp const *dp, struct imm_task const *task,
                        struct imm_path *path, unsigned end_state,
                        unsigned end_seq_len);

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

int imm_dp_reset(struct imm_dp *dp, struct imm_dp_args const *args)
{
    int rc = IMM_OK;

    if ((rc = imm_emis_reset(&dp->emis, dp->code, args->states, args->nstates)))
        return rc;

    if ((rc = imm_trans_table_reset(&dp->trans_table, args))) return rc;

    if ((rc = imm_state_table_reset(&dp->state_table, args))) return rc;

    return rc;
}

unsigned find_unsafe_states(struct imm_dp const *dp, unsigned *state)
{
    unsigned n = 0;
    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        for (unsigned t = 0; t < imm_trans_table_ntrans(&dp->trans_table, dst);
             ++t)
        {
            unsigned src =
                imm_trans_table_source_state(&dp->trans_table, dst, t);
            struct span span = imm_state_table_span(&dp->state_table, src);
            if (span.min == 0 && dst < src)
            {
                n++;
                *state = src;
            }
        }
    }
    return n;
}

int imm_dp_viterbi(struct imm_dp const *dp, struct imm_task *task,
                   struct imm_prod *prod)
{
    imm_prod_reset(prod);
    if (!task->seq) return IMM_NOT_SET_SEQ;

    if (dp->code->abc != imm_seq_abc(task->seq)) return IMM_DIFFERENT_ABC;

    unsigned end_state = dp->state_table.end_state_idx;
    unsigned min = imm_state_table_span(&dp->state_table, end_state).min;
    if (imm_seq_size(task->seq) < min) return IMM_SEQ_TOO_SHORT;

    struct elapsed elapsed = ELAPSED_INIT;
    if (elapsed_start(&elapsed)) return IMM_ELAPSED_LIB_FAILED;

    unsigned unsafe_state = {0};
    unsigned num_unsafe_states = find_unsafe_states(dp, &unsafe_state);
    (void)num_unsafe_states;
    assert(num_unsafe_states <= 1);
    struct imm_viterbi viterbi = {dp, task, imm_seq_size(task->seq),
                                  unsafe_state};
    int rc = call_viterbi(&viterbi, prod);

    if (elapsed_stop(&elapsed)) return IMM_ELAPSED_LIB_FAILED;

    prod->mseconds = elapsed_milliseconds(&elapsed);

    return rc;
}

unsigned imm_dp_nstates(struct imm_dp const *dp)
{
    return dp->state_table.nstates;
}

unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx, unsigned dst_idx)
{
    return imm_trans_table_idx(&dp->trans_table, src_idx, dst_idx);
}

int imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx, imm_float lprob)
{
    if (imm_unlikely(imm_lprob_is_nan(lprob))) return IMM_NAN_PROBABILITY;

    imm_trans_table_change(&dp->trans_table, trans_idx, lprob);
    return IMM_OK;
}

imm_float imm_dp_emis_score(struct imm_dp const *dp, unsigned state_id,
                            struct imm_seq const *seq)
{
    struct imm_eseq eseq = {0};
    imm_eseq_init(&eseq, dp->code);
    imm_float score = IMM_LPROB_NAN;
    if (imm_eseq_setup(&eseq, seq)) goto cleanup;

    unsigned state_idx = imm_state_table_idx(&dp->state_table, state_id);
    unsigned min = imm_state_table_span(&dp->state_table, state_idx).min;

    unsigned seq_code = imm_eseq_get(&eseq, 0, imm_seq_size(seq), min);
    score = imm_emis_score(&dp->emis, state_idx, seq_code);

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
        if (imm_trans_table_source_state(&dp->trans_table, dst_idx, i) ==
            src_idx)
            return imm_trans_table_score(&dp->trans_table, dst_idx, i);
    }
    return IMM_LPROB_NAN;
}

void imm_dp_write_dot(struct imm_dp const *dp, FILE *restrict fd,
                      imm_state_name *name)
{
    fprintf(fd, "digraph hmm {\n");
    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        for (unsigned t = 0; t < imm_trans_table_ntrans(&dp->trans_table, dst);
             ++t)
        {
            unsigned src =
                imm_trans_table_source_state(&dp->trans_table, dst, t);

            char src_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                                  '\0', '\0', '\0', '\0'};
            char dst_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                                  '\0', '\0', '\0', '\0'};

            (*name)(imm_state_table_id(&dp->state_table, src), src_name);
            (*name)(imm_state_table_id(&dp->state_table, dst), dst_name);
            fprintf(fd, "%s -> %s [label=%.4f];\n", src_name, dst_name,
                    imm_trans_table_score(&dp->trans_table, dst, t));
        }
    }
    fprintf(fd, "}\n");
}

struct final_score
{
    imm_float score;
    unsigned state;
    unsigned seq_len;
};

static struct final_score final_score(struct imm_dp const *dp,
                                      struct imm_task *task)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state_idx;

    unsigned final_state = IMM_STATE_NULL_IDX;
    unsigned final_seq_len = IMM_STATE_NULL_SEQLEN;

    unsigned length = imm_eseq_len(&task->eseq);
    unsigned max_seq = imm_state_table_span(&dp->state_table, end_state).max;

    for (unsigned len = MIN(max_seq, length);; --len)
    {
        imm_float s = imm_matrix_get_score(
            &task->matrix, imm_cell_init(length - len, end_state, len));
        if (s > score)
        {
            score = s;
            final_state = end_state;
            final_seq_len = len;
        }

        if (imm_state_table_span(&dp->state_table, end_state).min == len) break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == IMM_STATE_NULL_IDX) fscore.score = imm_lprob_nan();

    return fscore;
}

static int call_viterbi(struct imm_viterbi const *x, struct imm_prod *prod)
{
    unsigned len = imm_eseq_len(&x->task->eseq);
    struct imm_range range = imm_range_init(0, len + 1);
    viterbi_generic(x, &range);
    struct final_score fs = final_score(x->dp, x->task);
    prod->loglik = fs.score;

    if (x->task->save_path)
        return viterbi_path(x->dp, x->task, &prod->path, fs.state, fs.seq_len);

    return 0;
}

static int viterbi_path(struct imm_dp const *dp, struct imm_task const *task,
                        struct imm_path *path, unsigned end_state,
                        unsigned end_seq_len)
{
    unsigned row = imm_eseq_len(&task->eseq);
    unsigned state = end_state;
    unsigned seqlen = end_seq_len;
    bool valid = seqlen != IMM_STATE_NULL_SEQLEN;

    while (valid)
    {
        unsigned id = dp->state_table.ids[state];
        struct imm_step step = imm_step(id, seqlen);
        int rc = imm_path_add(path, step);
        if (rc) return rc;
        row -= seqlen;

        valid = cpath_valid(&task->path, row, state);
        if (valid)
        {
            unsigned trans = cpath_trans(&task->path, row, state);
            unsigned len = cpath_seqlen(&task->path, row, state);
            state =
                imm_trans_table_source_state(&dp->trans_table, state, trans);
            seqlen = len + imm_state_table_span(&dp->state_table, state).min;
        }
    }
    imm_path_reverse(path);
    return IMM_OK;
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

        unsigned min = imm_state_table_span(&dp->state_table, idx).min;
        unsigned seq_code = imm_eseq_get(&task->eseq, begin, step->seqlen, min);

        imm_float score = imm_emis_score(&dp->emis, idx, seq_code);
        struct imm_seq subseq = imm_subseq(task->seq, begin, step->seqlen);
        (*callb)(step->state_id, name);
        printf("<%s,%.*s,%.4f>\n", name, subseq.size, subseq.str, score);
        begin += step->seqlen;
    }
}
