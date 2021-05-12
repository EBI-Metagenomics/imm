#include "hope/hope.h"
#include "imm/imm.h"

/* Log-probabilities */
#define ZERO IMM_LPROB_ZERO
#define ONE ((imm_float)0.)

/* State IDs */
#define START ((imm_state_id_t)(0U << 11))
#define B ((imm_state_id_t)(1U << 11))
#define M ((imm_state_id_t)(2U << 11))
#define I ((imm_state_id_t)(3U << 11))
#define D ((imm_state_id_t)(4U << 11))
#define E ((imm_state_id_t)(5U << 11))
#define J ((imm_state_id_t)(6U << 11))
#define END ((imm_state_id_t)(7U << 11))

#define NCORE_NODES 1000U

char const seq_str[] = "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";

char const abc_str[] = "BMIEJ";

imm_float b_lprobs[] = {ONE, ZERO, ZERO, ZERO, ZERO};
imm_float m_lprobs[] = {ZERO, ONE, ZERO, ZERO, ZERO};
imm_float i_lprobs[] = {ZERO, ZERO, ONE, ZERO, ZERO};
imm_float e_lprobs[] = {ZERO, ZERO, ZERO, ONE, ZERO};
imm_float j_lprobs[] = {ZERO, ZERO, ZERO, ZERO, ONE};

struct model
{
    struct imm_abc abc;
    struct imm_hmm *hmm;
    struct imm_mute_state *start;
    struct imm_normal_state *b;
    struct imm_normal_state *j;
    struct imm_normal_state *m[NCORE_NODES];
    struct imm_normal_state *i[NCORE_NODES];
    struct imm_mute_state *d[NCORE_NODES];
    struct imm_normal_state *e;
    struct imm_mute_state *end;
} model;

void init_model(void);
void test_perf_viterbi(void);
void deinit_model(void);

int main(void)
{
    init_model();
    test_perf_viterbi();
    deinit_model();
    return hope_status();
}

void test_perf_viterbi(void)
{
    struct imm_dp *dp = imm_hmm_new_dp(model.hmm, imm_super(model.end));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    EQ(strlen(seq_str), 2000);
    struct imm_seq seq = imm_seq(imm_str(seq_str), &model.abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -65826.0106185297);

    imm_del(task);
    imm_deinit(&result);
    imm_del(dp);
}

#define SET_TRANS(hmm, a, b, v)                                                \
    imm_hmm_set_trans(hmm, imm_super(a), imm_super(b), v)

void init_model(void)
{
    imm_abc_init(&model.abc, imm_str(abc_str), '*');
    model.hmm = imm_hmm_new(&model.abc);

    model.start = imm_mute_state_new(START, &model.abc);
    imm_hmm_add_state(model.hmm, imm_super(model.start));
    imm_hmm_set_start(model.hmm, imm_super(model.start), imm_log(1.0));

    model.end = imm_mute_state_new(END, &model.abc);
    imm_hmm_add_state(model.hmm, imm_super(model.end));

    model.b = imm_normal_state_new(B, &model.abc, b_lprobs);
    imm_hmm_add_state(model.hmm, imm_super(model.b));

    model.e = imm_normal_state_new(E, &model.abc, e_lprobs);
    imm_hmm_add_state(model.hmm, imm_super(model.e));

    model.j = imm_normal_state_new(J, &model.abc, j_lprobs);
    imm_hmm_add_state(model.hmm, imm_super(model.j));

    SET_TRANS(model.hmm, model.start, model.b, imm_log(0.2));
    SET_TRANS(model.hmm, model.b, model.b, imm_log(0.2));
    SET_TRANS(model.hmm, model.e, model.e, imm_log(0.2));
    SET_TRANS(model.hmm, model.j, model.j, imm_log(0.2));
    SET_TRANS(model.hmm, model.e, model.j, imm_log(0.2));
    SET_TRANS(model.hmm, model.j, model.b, imm_log(0.2));
    SET_TRANS(model.hmm, model.e, model.end, imm_log(0.2));

    for (unsigned k = 0; k < NCORE_NODES; ++k)
    {
        model.m[k] = imm_normal_state_new(M | k, &model.abc, m_lprobs);
        model.i[k] = imm_normal_state_new(I | k, &model.abc, i_lprobs);
        model.d[k] = imm_mute_state_new(D | k, &model.abc);

        imm_hmm_add_state(model.hmm, imm_super(model.m[k]));
        imm_hmm_add_state(model.hmm, imm_super(model.i[k]));
        imm_hmm_add_state(model.hmm, imm_super(model.d[k]));

        if (k == 0)
            SET_TRANS(model.hmm, model.b, model.m[0], imm_log(0.2));

        SET_TRANS(model.hmm, model.m[k], model.i[k], imm_log(0.2));
        SET_TRANS(model.hmm, model.i[k], model.i[k], imm_log(0.2));

        if (k > 0)
        {
            SET_TRANS(model.hmm, model.m[k - 1], model.m[k], imm_log(0.2));
            SET_TRANS(model.hmm, model.d[k - 1], model.m[k], imm_log(0.2));
            SET_TRANS(model.hmm, model.i[k - 1], model.m[k], imm_log(0.2));

            SET_TRANS(model.hmm, model.m[k - 1], model.d[k], imm_log(0.2));
            SET_TRANS(model.hmm, model.d[k - 1], model.d[k], imm_log(0.2));
        }

        if (k == NCORE_NODES - 1)
        {
            SET_TRANS(model.hmm, model.m[k], model.e, imm_log(0.2));
            SET_TRANS(model.hmm, model.d[k], model.e, imm_log(0.2));
            SET_TRANS(model.hmm, model.i[k], model.e, imm_log(0.2));
        }
    }
}

void deinit_model(void)
{
    imm_del(model.hmm);
    imm_del(model.start);
    imm_del(model.b);
    imm_del(model.j);
    for (unsigned k = 0; k < NCORE_NODES; ++k)
    {
        imm_del(model.m[k]);
        imm_del(model.i[k]);
        imm_del(model.d[k]);
    }
    imm_del(model.e);
    imm_del(model.end);
}
