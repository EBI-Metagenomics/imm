#include "imm/imm.h"
#include <string.h>

/* Log-probabilities */
#define ZERO IMM_LPROB_ZERO
#define ONE ((imm_float)0.)

/* State IDs */
#define M_ID ((imm_state_id_t)(0U << 12))
#define S_ID ((imm_state_id_t)(1U << 12))
#define N_ID ((imm_state_id_t)(2U << 12))
#define B_ID ((imm_state_id_t)(3U << 12))
#define E_ID ((imm_state_id_t)(4U << 12))
#define C_ID ((imm_state_id_t)(5U << 12))
#define T_ID ((imm_state_id_t)(6U << 12))
#define J_ID ((imm_state_id_t)(7U << 12))

char const imm_msv_example1_seq1[] =
    "NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMC";

static imm_float N_LPROBS[] = {ONE, ZERO, ZERO, ZERO};
static imm_float M_LPROBS[] = {ZERO, ONE, ZERO, ZERO};
static imm_float C_LPROBS[] = {ZERO, ZERO, ONE, ZERO};
static imm_float J_LPROBS[] = {ZERO, ZERO, ZERO, ONE};

struct imm_msv_example1 imm_msv_example1;

#define SET_TRANS(hmm, a, b, v)                                                \
    do                                                                         \
    {                                                                          \
        imm_hmm_set_trans(hmm, imm_super(a), imm_super(b), v);                 \
    } while (0);

static void initialise_states(struct imm_msv_example1 *);
static void add_states(struct imm_msv_example1 *);
static void set_transitions(struct imm_msv_example1 *);

void imm_msv_example1_init(unsigned core_size)
{
    assert(core_size > 0);
    assert(core_size <= IMM_MSV_EXAMPLE1_SIZE);
    struct imm_msv_example1 *x = &imm_msv_example1;
    imm_abc_init(&x->abc, imm_str(IMM_MSV_EXAMPLE1_NUCLT_SYMBOLS),
                 IMM_MSV_EXAMPLE1_NUCLT_ANY_SYMBOL);
    imm_code_init(&x->code, &x->abc);
    imm_hmm_init(&x->hmm, &x->code);
    x->core_size = core_size;

    initialise_states(&imm_msv_example1);
    add_states(&imm_msv_example1);
    set_transitions(&imm_msv_example1);
}

unsigned imm_msv_example1_state_name(unsigned id, char *name)
{
    if ((id & (15U << 12)) == M_ID) sprintf(name, "M%u", (id & (0xFFFF >> 4)));

    if ((id & (15U << 12)) == S_ID) strcpy(name, "S");
    if ((id & (15U << 12)) == N_ID) strcpy(name, "N");
    if ((id & (15U << 12)) == B_ID) strcpy(name, "B");

    if ((id & (15U << 12)) == E_ID) strcpy(name, "E");
    if ((id & (15U << 12)) == C_ID) strcpy(name, "C");
    if ((id & (15U << 12)) == T_ID) strcpy(name, "T");

    if ((id & (15U << 12)) == J_ID) strcpy(name, "J");

    return (unsigned)strlen(name);
}

static void initialise_states(struct imm_msv_example1 *x)
{
    imm_mute_state_init(&x->S, S_ID, &x->abc);
    imm_normal_state_init(&x->N, N_ID, &x->abc, N_LPROBS);
    imm_mute_state_init(&x->B, B_ID, &x->abc);

    for (unsigned k = 0; k < x->core_size; ++k)
        imm_normal_state_init(&x->M[k], M_ID | k, &x->abc, M_LPROBS);

    imm_mute_state_init(&x->E, E_ID, &x->abc);
    imm_normal_state_init(&x->C, C_ID, &x->abc, C_LPROBS);
    imm_mute_state_init(&x->T, T_ID, &x->abc);

    imm_normal_state_init(&x->J, J_ID, &x->abc, J_LPROBS);
}

static void add_states(struct imm_msv_example1 *x)
{
    imm_hmm_add_state(&x->hmm, imm_super(&x->S));
    imm_hmm_add_state(&x->hmm, imm_super(&x->N));
    imm_hmm_add_state(&x->hmm, imm_super(&x->B));

    for (unsigned k = 0; k < x->core_size; ++k)
        imm_hmm_add_state(&x->hmm, imm_super(&x->M[k]));

    imm_hmm_add_state(&x->hmm, imm_super(&x->E));
    imm_hmm_add_state(&x->hmm, imm_super(&x->C));
    imm_hmm_add_state(&x->hmm, imm_super(&x->T));

    imm_hmm_add_state(&x->hmm, imm_super(&x->J));
}

static void set_transitions(struct imm_msv_example1 *x)
{
    imm_hmm_set_start(&x->hmm, imm_super(&x->S), ONE);

    SET_TRANS(&x->hmm, &x->S, &x->N, imm_log(0.1));
    SET_TRANS(&x->hmm, &x->N, &x->N, imm_log(0.2));
    SET_TRANS(&x->hmm, &x->N, &x->B, imm_log(0.3));
    SET_TRANS(&x->hmm, &x->S, &x->B, imm_log(0.4));

    for (unsigned k = 0; k < x->core_size; ++k)
        SET_TRANS(&x->hmm, &x->B, &x->M[k], imm_log(0.11));

    for (unsigned k = 1; k < x->core_size; ++k)
        SET_TRANS(&x->hmm, &x->M[k - 1], &x->M[k], imm_log(0.12));

    SET_TRANS(&x->hmm, &x->M[x->core_size - 1], &x->E, imm_log(0.13));

    SET_TRANS(&x->hmm, &x->E, &x->C, imm_log(0.1));
    SET_TRANS(&x->hmm, &x->C, &x->C, imm_log(0.2));
    SET_TRANS(&x->hmm, &x->C, &x->T, imm_log(0.3));
    SET_TRANS(&x->hmm, &x->E, &x->T, imm_log(0.4));

    SET_TRANS(&x->hmm, &x->E, &x->J, imm_log(0.5));
    SET_TRANS(&x->hmm, &x->J, &x->J, imm_log(0.6));
    SET_TRANS(&x->hmm, &x->J, &x->B, imm_log(0.7));
    SET_TRANS(&x->hmm, &x->E, &x->B, imm_log(0.8));
}
