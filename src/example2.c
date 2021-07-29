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
#define N ((imm_state_id_t)(8U << 11))

char const imm_example2_seq[] =
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
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC"
    "AAAACGCGTGTCACGACAACGCGTACGTTTCGACGAGTACGACGCCCGGG"
    "AAAACGCGTGTCGACGACGAACGCGTACGTTTACGACGAGTACGACGCCC";

struct imm_example2 imm_example2 = {.dna = &imm_dna_default};

#define SET_TRANS(hmm, a, b, v)                                                \
    imm_hmm_set_trans(&hmm, imm_super(&a), imm_super(&b), v)

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

void imm_example2_init(void)
{
    struct imm_example2 *m = &imm_example2;
    struct imm_nuclt const *nuclt = imm_super(m->dna);
    struct imm_abc const *abc = imm_super(nuclt);
    m->hmm = imm_hmm_init(abc);

    imm_float epsilon = (imm_float)0.01;
    m->nucltp = imm_nuclt_lprob(nuclt, IMM_ARR(imm_log(0.25), imm_log(0.25),
                                               imm_log(0.45), imm_log(0.05)));

    m->m_marg = codonm(imm_codon(nuclt, 'A', 'C', 'G'), imm_log(100.0));
    m->i_marg = codonm(imm_codon(nuclt, 'C', 'G', 'T'), imm_log(100.0));
    m->b_marg = codonm(imm_codon(nuclt, 'A', 'A', 'A'), imm_log(100.0));
    m->e_marg = codonm(imm_codon(nuclt, 'C', 'C', 'C'), imm_log(100.0));
    m->j_marg = codonm(imm_codon(nuclt, 'G', 'G', 'G'), imm_log(100.0));
    m->null.n_marg = codonm(imm_codon(nuclt, 'G', 'T', 'G'), imm_log(13.0));

    imm_mute_state_init(&m->start, START, abc);
    imm_hmm_add_state(&m->hmm, imm_super(&m->start));
    imm_hmm_set_start(&m->hmm, imm_super(&m->start), imm_log(1.0));

    imm_mute_state_init(&m->end, END, abc);
    imm_hmm_add_state(&m->hmm, imm_super(&m->end));

    imm_frame_state_init(&m->b, B, &m->nucltp, &m->b_marg, epsilon);
    imm_hmm_add_state(&m->hmm, imm_super(&m->b));

    imm_frame_state_init(&m->e, E, &m->nucltp, &m->e_marg, epsilon);
    imm_hmm_add_state(&m->hmm, imm_super(&m->e));

    imm_frame_state_init(&m->j, J, &m->nucltp, &m->j_marg, epsilon);
    imm_hmm_add_state(&m->hmm, imm_super(&m->j));

    SET_TRANS(m->hmm, m->start, m->b, imm_log(0.2));
    SET_TRANS(m->hmm, m->b, m->b, imm_log(0.2));
    SET_TRANS(m->hmm, m->e, m->e, imm_log(0.2));
    SET_TRANS(m->hmm, m->j, m->j, imm_log(0.2));
    SET_TRANS(m->hmm, m->e, m->j, imm_log(0.2));
    SET_TRANS(m->hmm, m->j, m->b, imm_log(0.2));
    SET_TRANS(m->hmm, m->e, m->end, imm_log(0.2));

    for (unsigned k = 0; k < IMM_EXAMPLE2_SIZE; ++k)
    {
        imm_frame_state_init(m->m + k, M | k, &m->nucltp, &m->m_marg, epsilon);
        imm_frame_state_init(m->i + k, I | k, &m->nucltp, &m->i_marg, epsilon);
        imm_mute_state_init(m->d + k, D | k, abc);

        imm_hmm_add_state(&m->hmm, imm_super(&m->m[k]));
        imm_hmm_add_state(&m->hmm, imm_super(&m->i[k]));
        imm_hmm_add_state(&m->hmm, imm_super(&m->d[k]));

        if (k == 0)
            SET_TRANS(m->hmm, m->b, m->m[0], imm_log(0.2));

        SET_TRANS(m->hmm, m->m[k], m->i[k], imm_log(0.2));
        SET_TRANS(m->hmm, m->i[k], m->i[k], imm_log(0.2));

        if (k > 0)
        {
            SET_TRANS(m->hmm, m->m[k - 1], m->m[k], imm_log(0.2));
            SET_TRANS(m->hmm, m->d[k - 1], m->m[k], imm_log(0.2));
            SET_TRANS(m->hmm, m->i[k - 1], m->m[k], imm_log(0.2));

            SET_TRANS(m->hmm, m->m[k - 1], m->d[k], imm_log(0.2));
            SET_TRANS(m->hmm, m->d[k - 1], m->d[k], imm_log(0.2));
        }

        if (k == IMM_EXAMPLE2_SIZE - 1)
        {
            SET_TRANS(m->hmm, m->m[k], m->e, imm_log(0.2));
            SET_TRANS(m->hmm, m->d[k], m->e, imm_log(0.2));
            SET_TRANS(m->hmm, m->i[k], m->e, imm_log(0.2));
        }
    }

    m->null.hmm = imm_hmm_init(abc);
    imm_frame_state_init(&m->null.n, N, &m->nucltp, &m->null.n_marg, epsilon);
    imm_hmm_add_state(&m->null.hmm, imm_super(&m->null.n));
    imm_hmm_set_start(&m->null.hmm, imm_super(&m->null.n), imm_log(1.0));
    SET_TRANS(m->null.hmm, m->null.n, m->null.n, imm_log(0.2));
}
