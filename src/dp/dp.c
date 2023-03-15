#include "dp/dp.h"
#include "dp/emis.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "dp/viterbi.h"
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

void imm_dp_write_dot(struct imm_dp const *dp, FILE *restrict fp,
                      imm_state_name *name)
{
    fprintf(fp, "digraph hmm {\n");
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
            fprintf(fp, "%s -> %s [label=%.4f];\n", src_name, dst_name,
                    trans_table_score(&dp->trans_table, dst, t));
        }
    }
    fprintf(fp, "}\n");
}

IMM_API void imm_dp_dump_impl_details(struct imm_dp const *dp,
                                      FILE *restrict fp, imm_state_name *callb)
{
    if (dp->state_table.nstates == 0) return;
    printf("Symbology: \n");
    printf("  ^ starting-state\n");
    printf("  ~ mute-state with dst < src\n");
    printf("\n");

    char name[IMM_STATE_NAME_SIZE] = {0};

    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        (*callb)(state_table_id(&dp->state_table, dst), name);
        if (dp->state_table.start.state == dst)
            fputc('^', fp);
        else
            fputc(' ', fp);

        printf("%02u:%3s: ", dst, name);
        for (unsigned t = 0; t < trans_table_ntrans(&dp->trans_table, dst); ++t)
        {
            if (t > 0) fputc(',', fp);
            unsigned src = trans_table_source_state(&dp->trans_table, dst, t);
            struct span span = state_table_span(&dp->state_table, src);
            if (span.min == 0 && dst < src) fputc('~', fp);
            (*callb)(state_table_id(&dp->state_table, src), name);
            printf("%s", name);
        }
        fputc('\n', fp);
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
