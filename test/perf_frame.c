#include "hope/hope.h"
#include "imm/imm.h"

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

char const seq_str[] = "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
                       "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
                       "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC";

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

struct model
{
    struct imm_dna const *dna;
    struct imm_hmm *hmm;
    struct imm_mute_state *start;
    struct imm_frame_state *b;
    struct imm_frame_state *j;
    struct imm_frame_state *m[NCORE_NODES];
    struct imm_frame_state *i[NCORE_NODES];
    struct imm_mute_state *d[NCORE_NODES];
    struct imm_frame_state *e;
    struct imm_mute_state *end;
    struct imm_nuclt_lprob nucltp;
    struct imm_codon_marg m_marg;
    struct imm_codon_marg i_marg;
    struct imm_codon_marg b_marg;
    struct imm_codon_marg e_marg;
    struct imm_codon_marg j_marg;
} model = {.dna = &imm_dna_default};

#define SET_TRANS(hmm, a, b, v)                                                \
    imm_hmm_set_trans(hmm, imm_super(a), imm_super(b), v)

void test_perf_viterbi(void)
{
    struct imm_dp *dp = imm_hmm_new_dp(model.hmm, imm_super(model.end));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    struct imm_nuclt const *nuclt = imm_super(model.dna);
    struct imm_abc const *abc = imm_super(nuclt);

    struct imm_seq seq = imm_seq(imm_str(seq_str), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -1622.8626708984);

    imm_del(task);
    imm_del(dp);
}

#define SET_CODONP(codonp, codon, v) imm_codon_lprob_set(codonp, codon, v)

static struct imm_codon_lprob create_codonp(struct imm_nuclt const *nuclt)
{
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'A', 'A'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'A', 'C'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'A', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'A', 'T'), imm_log(0.0023173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'C', 'A'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'C', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'C', 'T'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'G', 'C'), imm_log(0.0029114));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'G', 'G'), imm_log(0.0029003));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'G', 'T'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'T', 'A'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'T', 'G'), imm_log(0.0049178));
    SET_CODONP(&codonp, imm_codon(nuclt, 'A', 'T', 'T'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'A', 'A'), imm_log(0.0029478));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'A', 'C'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'A', 'G'), imm_log(0.0029123));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'A', 'T'), imm_log(0.0009133));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'C', 'A'), imm_log(0.0029179));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'C', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'C', 'T'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'G', 'C'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'G', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'G', 'T'), imm_log(0.0041183));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'T', 'A'), imm_log(0.0038173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'T', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'C', 'T', 'T'), imm_log(0.0029111));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'A', 'A'), imm_log(0.0019173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'A', 'C'), imm_log(0.0029103));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'A', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'A', 'T'), imm_log(0.0029103));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'C', 'A'), imm_log(0.0003138));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'C', 'G'), imm_log(0.0039173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'C', 'T'), imm_log(0.0029103));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'G', 'C'), imm_log(0.0019173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'G', 'G'), imm_log(0.0009173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'G', 'T'), imm_log(0.0029143));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'T', 'A'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'T', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'G', 'T', 'T'), imm_log(0.0029171));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'A', 'A'), imm_log(0.0099113));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'A', 'C'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'A', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'A', 'T'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'C', 'A'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'C', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'C', 'T'), imm_log(0.0020173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'G', 'C'), imm_log(0.0029193));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'G', 'G'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'G', 'T'), imm_log(0.0029173));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'T', 'A'), imm_log(0.0089193));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'T', 'G'), imm_log(0.0029138));
    SET_CODONP(&codonp, imm_codon(nuclt, 'T', 'T', 'T'), imm_log(0.0089183));
    return codonp;
}

static struct imm_codon_marg codonm(struct imm_codon codon, imm_float lprob)
{
    struct imm_codon_lprob codonp = create_codonp(codon.nuclt);
    imm_codon_lprob_set(&codonp, codon, lprob);
    return imm_codon_marg(&codonp);
}

void init_model(void)
{
    struct imm_nuclt const *nuclt = imm_super(model.dna);
    struct imm_abc const *abc = imm_super(nuclt);
    model.hmm = imm_hmm_new(abc);

    imm_float epsilon = (imm_float)0.01;
    model.nucltp =
        imm_nuclt_lprob(nuclt, IMM_ARR(imm_log(0.25), imm_log(0.25),
                                       imm_log(0.45), imm_log(0.05)));

    model.m_marg = codonm(imm_codon(nuclt, 'A', 'C', 'G'), imm_log(100.0));
    model.i_marg = codonm(imm_codon(nuclt, 'C', 'G', 'T'), imm_log(100.0));
    model.b_marg = codonm(imm_codon(nuclt, 'A', 'A', 'A'), imm_log(100.0));
    model.e_marg = codonm(imm_codon(nuclt, 'C', 'C', 'C'), imm_log(100.0));
    model.j_marg = codonm(imm_codon(nuclt, 'G', 'G', 'G'), imm_log(100.0));

    model.start = imm_mute_state_new(START, abc);
    imm_hmm_add_state(model.hmm, imm_super(model.start));
    imm_hmm_set_start(model.hmm, imm_super(model.start), imm_log(1.0));

    model.end = imm_mute_state_new(END, abc);
    imm_hmm_add_state(model.hmm, imm_super(model.end));

    model.b = imm_frame_state_new(B, &model.nucltp, &model.b_marg, epsilon);
    imm_hmm_add_state(model.hmm, imm_super(model.b));

    model.e = imm_frame_state_new(E, &model.nucltp, &model.e_marg, epsilon);
    imm_hmm_add_state(model.hmm, imm_super(model.e));

    model.j = imm_frame_state_new(J, &model.nucltp, &model.j_marg, epsilon);
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
        struct model *m = &model;
        m->m[k] = imm_frame_state_new(M | k, &m->nucltp, &m->m_marg, epsilon);
        m->i[k] = imm_frame_state_new(I | k, &m->nucltp, &m->i_marg, epsilon);
        m->d[k] = imm_mute_state_new(D | k, abc);

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
