#include "imm/imm.h"
#include <string.h>

/* Log-probabilities */
#define ZERO IMM_LPROB_ZERO
#define ONE ((imm_float)0.)

/* State IDs */
#define START ((imm_state_id_t)(0U << 12))
#define B ((imm_state_id_t)(1U << 12))
#define M ((imm_state_id_t)(2U << 12))
#define I ((imm_state_id_t)(3U << 12))
#define D ((imm_state_id_t)(4U << 12))
#define E ((imm_state_id_t)(5U << 12))
#define J ((imm_state_id_t)(6U << 12))
#define END ((imm_state_id_t)(7U << 12))
#define N ((imm_state_id_t)(8U << 12))

unsigned imm_example1_state_name(unsigned id, char name[IMM_STATE_NAME_SIZE])
{
    if (id == START) strcpy(name, "START");
    if (id == B) strcpy(name, "B");
    if (id == M) strcpy(name, "M");
    if (id == I) strcpy(name, "I");
    if (id == D) strcpy(name, "D");
    if (id == E) strcpy(name, "E");
    if (id == J) strcpy(name, "J");
    if (id == END) strcpy(name, "END");
    if (id == N) strcpy(name, "N");
    return (unsigned)strlen(name);
}

char const imm_example1_seq[] =
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
    "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
    "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
    "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";

static imm_float b_lprobs[] = {ONE, ZERO, ZERO, ZERO, ZERO};
static imm_float m_lprobs[] = {ZERO, ONE, ZERO, ZERO, ZERO};
static imm_float i_lprobs[] = {ZERO, ZERO, ONE, ZERO, ZERO};
static imm_float e_lprobs[] = {ZERO, ZERO, ZERO, ONE, ZERO};
static imm_float j_lprobs[] = {ZERO, ZERO, ZERO, ZERO, ONE};
static imm_float n_lprobs[] = {-1e3f, -1e3f, -1e3f, -1e3f, -1e3f};

struct imm_example1 imm_example1;

#define SET_TRANS(hmm, a, b, v)                                                \
    imm_hmm_set_trans(hmm, imm_super(a), imm_super(b), v)

void imm_example1_init(unsigned core_size)
{
    assert(core_size > 0);
    assert(core_size <= IMM_EXAMPLE1_SIZE);
    struct imm_example1 *m = &imm_example1;
    imm_abc_init(&m->abc, imm_str(IMM_EXAMPLE1_NUCLT_SYMBOLS),
                 IMM_EXAMPLE1_NUCLT_ANY_SYMBOL);
    imm_code_init(&m->code, &m->abc);
    imm_hmm_init(&m->hmm, &m->code);
    m->core_size = core_size;

    imm_mute_state_init(&m->start, START, &m->abc);
    imm_hmm_add_state(&m->hmm, imm_super(&m->start));
    imm_hmm_set_start(&m->hmm, imm_super(&m->start), imm_log(1.0));

    imm_mute_state_init(&m->end, END, &m->abc);
    imm_hmm_add_state(&m->hmm, imm_super(&m->end));

    imm_normal_state_init(&m->b, B, &m->abc, b_lprobs);
    imm_hmm_add_state(&m->hmm, imm_super(&m->b));

    imm_normal_state_init(&m->e, E, &m->abc, e_lprobs);
    imm_hmm_add_state(&m->hmm, imm_super(&m->e));

    imm_normal_state_init(&m->j, J, &m->abc, j_lprobs);
    imm_hmm_add_state(&m->hmm, imm_super(&m->j));

    SET_TRANS(&m->hmm, &m->start, &m->b, imm_log(0.2));
    SET_TRANS(&m->hmm, &m->b, &m->b, imm_log(0.2));
    SET_TRANS(&m->hmm, &m->e, &m->e, imm_log(0.2));
    SET_TRANS(&m->hmm, &m->j, &m->j, imm_log(0.2));
    SET_TRANS(&m->hmm, &m->e, &m->j, imm_log(0.2));
    SET_TRANS(&m->hmm, &m->j, &m->b, imm_log(0.2));
    SET_TRANS(&m->hmm, &m->e, &m->end, imm_log(0.2));

    for (unsigned k = 0; k < core_size; ++k)
    {
        imm_normal_state_init(&m->m[k], M | k, &m->abc, m_lprobs);
        imm_normal_state_init(&m->i[k], I | k, &m->abc, i_lprobs);
        imm_mute_state_init(&m->d[k], D | k, &m->abc);

        imm_hmm_add_state(&m->hmm, imm_super(&m->m[k]));
        imm_hmm_add_state(&m->hmm, imm_super(&m->i[k]));
        imm_hmm_add_state(&m->hmm, imm_super(&m->d[k]));

        if (k == 0) SET_TRANS(&m->hmm, &m->b, &m->m[0], imm_log(0.2));

        SET_TRANS(&m->hmm, &m->m[k], &m->i[k], imm_log(0.2));
        SET_TRANS(&m->hmm, &m->i[k], &m->i[k], imm_log(0.2));

        if (k > 0)
        {
            SET_TRANS(&m->hmm, &m->m[k - 1], &m->m[k], imm_log(0.2));
            SET_TRANS(&m->hmm, &m->d[k - 1], &m->m[k], imm_log(0.2));
            SET_TRANS(&m->hmm, &m->i[k - 1], &m->m[k], imm_log(0.2));

            SET_TRANS(&m->hmm, &m->m[k - 1], &m->d[k], imm_log(0.2));
            SET_TRANS(&m->hmm, &m->d[k - 1], &m->d[k], imm_log(0.2));
        }

        if (k == core_size - 1)
        {
            SET_TRANS(&m->hmm, &m->m[k], &m->e, imm_log(0.2));
            SET_TRANS(&m->hmm, &m->d[k], &m->e, imm_log(0.2));
            SET_TRANS(&m->hmm, &m->i[k], &m->e, imm_log(0.2));
        }
    }

    imm_hmm_init(&m->null.hmm, &m->code);
    imm_normal_state_init(&m->null.n, N, &m->abc, n_lprobs);
    imm_hmm_add_state(&m->null.hmm, imm_super(&m->null.n));
    imm_hmm_set_start(&m->null.hmm, imm_super(&m->null.n), imm_log(1.0));
    SET_TRANS(&m->null.hmm, &m->null.n, &m->null.n, imm_log(0.2));
}
