#include "hope.h"
#include "imm/imm.h"
#include <string.h>

enum state_id
{
    S_ID = 0,
    N_ID = 1,
    B_ID = 2,
    M1_ID = 3,
    M2_ID = 4,
    M3_ID = 5,
    J_ID = 6,
    E_ID = 7,
    C_ID = 8,
    T_ID = 9,
};

#define ONEQ imm_log(0.25)
#define FOUR_QUARTERS IMM_ARR(ONEQ, ONEQ, ONEQ, ONEQ)

#define NULL_ATG imm_log(0.8 / 1.3)
#define NULL_ATT imm_log(0.1 / 1.3)
#define NULL_GTC imm_log(0.4 / 1.3)

#define M1_CCC imm_log(0.8 / 1.3)
#define M1_CAT imm_log(0.1 / 1.3)
#define M1_CTC imm_log(0.4 / 1.3)

#define M2_TTT imm_log(0.8 / 1.3)
#define M2_TAT imm_log(0.1 / 1.3)
#define M2_TGT imm_log(0.4 / 1.3)

#define M3_GGG imm_log(0.8 / 1.3)
#define M3_GAG imm_log(0.1 / 1.3)
#define M3_GTG imm_log(0.4 / 1.3)

#define EPSILON ((imm_float)(0.01))
// #define TPROB -1.5
#define TPROB 0

static unsigned state_name(unsigned id, char *name);

int main(void)
{
    struct imm_dna const *dna = &imm_dna_iupac;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);

    struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(nuclt, FOUR_QUARTERS);

    struct imm_codon_lprob codonp = {0};

    codonp = imm_codon_lprob(nuclt);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), NULL_ATG);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), NULL_ATT);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "GTC"), NULL_GTC);
    struct imm_codon_marg nullm = imm_codon_marg(&codonp);

    codonp = imm_codon_lprob(nuclt);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "CCC"), M1_CCC);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "CAT"), M1_CAT);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "CTC"), M1_CTC);
    struct imm_codon_marg m1m = imm_codon_marg(&codonp);

    codonp = imm_codon_lprob(nuclt);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "TTT"), M2_TTT);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "TAT"), M2_TAT);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "TGT"), M2_TGT);
    struct imm_codon_marg m2m = imm_codon_marg(&codonp);

    codonp = imm_codon_lprob(nuclt);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "GGG"), M3_GGG);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "GAG"), M3_GAG);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "GTG"), M3_GTG);
    struct imm_codon_marg m3m = imm_codon_marg(&codonp);

    struct imm_mute_state S = {0};
    struct imm_frame_state N = {0};
    struct imm_mute_state B = {0};

    struct imm_frame_state M1 = {0};
    struct imm_frame_state M2 = {0};
    struct imm_frame_state M3 = {0};

    struct imm_frame_state J = {0};

    struct imm_mute_state E = {0};
    struct imm_frame_state C = {0};
    struct imm_mute_state T = {0};

    imm_mute_state_init(&S, S_ID, abc);
    imm_frame_state_init(&N, N_ID, &nucltp, &nullm, EPSILON, imm_span(1, 5));
    imm_mute_state_init(&B, B_ID, abc);

    imm_frame_state_init(&M1, M1_ID, &nucltp, &m1m, EPSILON, imm_span(1, 5));
    imm_frame_state_init(&M2, M2_ID, &nucltp, &m2m, EPSILON, imm_span(1, 5));
    imm_frame_state_init(&M3, M3_ID, &nucltp, &m3m, EPSILON, imm_span(1, 5));

    imm_frame_state_init(&J, J_ID, &nucltp, &nullm, EPSILON, imm_span(1, 5));

    imm_mute_state_init(&E, E_ID, abc);
    imm_frame_state_init(&C, C_ID, &nucltp, &nullm, EPSILON, imm_span(1, 5));
    imm_mute_state_init(&T, T_ID, abc);

    static struct imm_code code = {0};
    imm_code_init(&code, abc);
    struct imm_hmm hmm = {0};
    imm_hmm_init(&hmm, &code);

    imm_hmm_add_state(&hmm, imm_super(&S));
    imm_hmm_add_state(&hmm, imm_super(&N));
    imm_hmm_add_state(&hmm, imm_super(&B));

    imm_hmm_add_state(&hmm, imm_super(&M1));
    imm_hmm_add_state(&hmm, imm_super(&M2));
    imm_hmm_add_state(&hmm, imm_super(&M3));

    imm_hmm_add_state(&hmm, imm_super(&J));

    imm_hmm_add_state(&hmm, imm_super(&E));
    imm_hmm_add_state(&hmm, imm_super(&C));
    imm_hmm_add_state(&hmm, imm_super(&T));

    imm_hmm_set_trans(&hmm, imm_super(&S), imm_super(&N), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&N), imm_super(&N), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&N), imm_super(&B), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&S), imm_super(&B), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M1), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M2), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M3), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&M2), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&M2), imm_super(&M3), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&E), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&M2), imm_super(&E), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&M3), imm_super(&E), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&B), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&J), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&J), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&B), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&C), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&C), imm_super(&C), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&C), imm_super(&T), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&T), TPROB);

    imm_hmm_set_start(&hmm, imm_super(&S), TPROB);

    FILE *fp = fopen(TMPDIR "/hmm.dot", "w");
    notnull(fp);
    imm_hmm_write_dot(&hmm, fp, &state_name);
    fclose(fp);

    struct imm_dp dp = {0};
    imm_hmm_init_dp(&hmm, imm_super(&T), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(IMM_STR("CCCTTTGGG"), abc);
    imm_task_setup(task, &seq);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    eq(imm_path_nsteps(&prod.path), 7);
    close(imm_hmm_loglik(&hmm, &seq, &prod.path), -1.57676188818);
    close(prod.loglik, -1.57676188818);
    imm_dp_dump_path(&dp, task, &prod, &state_name);

    fp = fopen(TMPDIR "/dp.dot", "w");
    notnull(fp);
    imm_dp_write_dot(&dp, fp, &state_name);
    fclose(fp);

    // imm_dp_dump_impl_details(&dp, stdout, &state_name);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);

    return hope_status();
}

static unsigned state_name(unsigned id, char *name)
{
    char *x = name;
    if (id == S_ID) *(x++) = 'S';
    if (id == N_ID) *(x++) = 'N';
    if (id == B_ID) *(x++) = 'B';

    if (id == M1_ID) *(x++) = 'M';
    if (id == M1_ID) *(x++) = '1';

    if (id == M2_ID) *(x++) = 'M';
    if (id == M2_ID) *(x++) = '2';

    if (id == M3_ID) *(x++) = 'M';
    if (id == M3_ID) *(x++) = '3';

    if (id == J_ID) *(x++) = 'J';

    if (id == E_ID) *(x++) = 'E';
    if (id == C_ID) *(x++) = 'C';
    if (id == T_ID) *(x++) = 'T';

    *x = '\0';
    return (unsigned)strlen(name);
}
