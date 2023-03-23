#include "imm/dp.h"
#include "best_trans.h"
#include "dp/ranges.h"
#include "elapsed/elapsed.h"
#include "error.h"
#include "final_score.h"
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
#include "viterbi_nopath.h"
#include "viterbi_path.h"
#include <assert.h>
#include <limits.h>

static enum imm_rc call_viterbi(struct imm_viterbi const *,
                                struct imm_prod *prod);
static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
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

enum imm_rc imm_dp_reset(struct imm_dp *dp, struct imm_dp_args const *args)
{
    enum imm_rc rc = IMM_OK;

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

enum imm_rc imm_dp_viterbi(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_prod *prod)
{
    imm_prod_reset(prod);
    if (!task->seq) return error(IMM_NOT_SET_SEQ);

    if (dp->code->abc != imm_seq_abc(task->seq))
        return error(IMM_DIFFERENT_ABC);

    unsigned end_state = dp->state_table.end_state_idx;
    unsigned min = imm_state_table_span(&dp->state_table, end_state).min;
    if (imm_seq_size(task->seq) < min) return error(IMM_SEQ_TOO_SHORT);

    struct elapsed elapsed = ELAPSED_INIT;
    if (elapsed_start(&elapsed)) return error(IMM_ELAPSED_LIB_FAILED);

    unsigned unsafe_state = {0};
    unsigned num_unsafe_states = find_unsafe_states(dp, &unsafe_state);
    (void)num_unsafe_states;
    assert(num_unsafe_states <= 1);
    struct imm_viterbi viterbi = {dp, task, unsafe_state};
    enum imm_rc rc = call_viterbi(&viterbi, prod);

    if (elapsed_stop(&elapsed)) return error(IMM_ELAPSED_LIB_FAILED);

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

enum imm_rc imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx,
                                imm_float lprob)
{
    if (imm_unlikely(imm_lprob_is_nan(lprob)))
        return error(IMM_NAN_PROBABILITY);

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

static enum imm_rc call_viterbi(struct imm_viterbi const *x,
                                struct imm_prod *prod)
{
    unsigned len = imm_eseq_len(&x->task->eseq);

    struct ranges rg = {0};
    dp_ranges_find(&rg, len);

    if (!imm_range_empty(rg.safe))
    {
        if (x->task->save_path)
        {
            viterbi_path_safe_row0(x);
            viterbi_path_safe_future(x, &rg.safe_future);
            viterbi_path_safe(x, &rg.safe);
            viterbi_path_safe_past(x, &rg.safe_past);
        }
        else
        {
            viterbi_nopath_safe_row0(x);
            viterbi_nopath_safe_future(x, &rg.safe_future);
            viterbi_nopath_safe(x, &rg.safe);
            viterbi_nopath_safe_past(x, &rg.safe_past);
        }
    }
    else if (len >= 1 + IMM_STATE_MAX_SEQLEN)
    {
        struct imm_range f = imm_range_init(1, len - IMM_STATE_MAX_SEQLEN + 1);
        struct imm_range t = imm_range_init(f.b, len + 1);

        if (x->task->save_path)
        {
            viterbi_path_safe_row0(x);
            viterbi_path_safe_future(x, &f);
            viterbi_path_unsafe(x, &t, len);
        }
        else
        {
            viterbi_nopath_safe_row0(x);
            viterbi_nopath_safe_future(x, &f);
            viterbi_nopath_unsafe(x, &t, len);
        }
    }
    else
    {
        struct imm_range range = imm_range_init(1, len + 1);
        if (x->task->save_path)
        {
            viterbi_path_row0(x, len);
            viterbi_path_unsafe(x, &range, len);
        }
        else
        {
            viterbi_nopath_row0(x);
            viterbi_nopath_unsafe(x, &range, len);
        }
    }

    struct final_score const fscore = final_score(x->dp, x->task);
    prod->loglik = fscore.score;
    if (x->task->save_path)
        return viterbi_path(x->dp, x->task, &prod->path, fscore.state,
                            fscore.seq_len);
    return IMM_OK;
}

static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
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
        enum imm_rc rc = imm_path_add(path, step);
        if (rc) return rc;
        row -= seqlen;

        valid = path_valid(&task->path, row, state);
        if (valid)
        {
            unsigned trans = path_trans(&task->path, row, state);
            unsigned len = path_seqlen(&task->path, row, state);
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
