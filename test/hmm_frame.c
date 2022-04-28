#include "hope.h"
#include "imm/imm.h"

void test_hmm_frame_state_0eps(void);
void test_hmm_frame_state_len1(void);
void test_hmm_frame_state_len2(void);
void test_hmm_frame_state_len3(void);
void test_hmm_frame_state_len4(void);
void test_hmm_frame_state_len5(void);

struct imm_nuclt const *nuclt;
struct imm_abc const *abc;
static struct imm_code code;
struct imm_nuclt_lprob nucltp;
struct imm_codon_lprob codonp;
struct imm_codon_marg codonm;

int main(void)
{
    nuclt = imm_super(&imm_dna_iupac);
    abc = imm_super(nuclt);
    imm_code_init(&code, abc);

    nucltp = imm_nuclt_lprob(nuclt, IMM_ARR(imm_log(0.25), imm_log(0.25),
                                            imm_log(0.5), imm_lprob_zero()));

    codonp = imm_codon_lprob(nuclt);
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), imm_log(0.8));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), imm_log(0.1));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "CCC"), imm_log(0.1));

    codonm = imm_codon_marg(&codonp);

    test_hmm_frame_state_0eps();
    test_hmm_frame_state_len1();
    test_hmm_frame_state_len2();
    test_hmm_frame_state_len3();
    test_hmm_frame_state_len4();
    test_hmm_frame_state_len5();
    return hope_status();
}

void test_hmm_frame_state_0eps(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.0f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 3));
    struct imm_seq seq = imm_seq(IMM_STR("ATT"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -2.3025850930);
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -0.2231435513142097);
    seq = imm_seq(IMM_STR("AT"), abc);
    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &seq, &path)));

    imm_del(&path);
}

void test_hmm_frame_state_len1(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 1));
    struct imm_seq seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -6.0198639951);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 1));
    seq = imm_seq(IMM_STR("C"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -7.1184762838);

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    seq = imm_seq(IMM_STR("A"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -6.0198639951);

    imm_hmm_reset_dp(&hmm, imm_super(&state), &dp);
    imm_task_reset(task, &dp);
    seq = imm_seq(IMM_STR("C"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -7.1184762838);

    imm_del(task);
    imm_del(&path);
    imm_del(&prod);
    imm_del(&dp);
}

void test_hmm_frame_state_len2(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    struct imm_seq seq = imm_seq(IMM_STR("AA"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -8.9102357365);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    seq = imm_seq(IMM_STR("TG"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -3.2434246877);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    seq = imm_seq(IMM_STR("CC"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -4.2250228758);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    seq = imm_seq(IMM_STR("TT"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -5.3267168311);

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    seq = imm_seq(IMM_STR("AA"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -8.9102357365);

    imm_hmm_reset_dp(&hmm, imm_super(&state), &dp);
    imm_task_reset(task, &dp);
    seq = imm_seq(IMM_STR("TG"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -3.2434246877);

    imm_hmm_reset_dp(&hmm, imm_super(&state), &dp);
    imm_task_reset(task, &dp);
    seq = imm_seq(IMM_STR("CC"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -4.2250228758);

    imm_hmm_reset_dp(&hmm, imm_super(&state), &dp);
    imm_task_reset(task, &dp);
    seq = imm_seq(IMM_STR("TT"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -5.3267168311);

    imm_del(task);
    imm_del(&dp);
    imm_del(&prod);
    imm_del(&path);
}

void test_hmm_frame_state_len3(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 3));
    struct imm_seq seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -7.0123444607);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 3));
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -0.6397933781);

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    seq = imm_seq(IMM_STR("ATC"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -7.0123444607);

    imm_hmm_reset_dp(&hmm, imm_super(&state), &dp);
    imm_task_reset(task, &dp);
    seq = imm_seq(IMM_STR("ATG"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -0.6397933781);

    imm_del(task);
    imm_del(&dp);
    imm_del(&prod);
    imm_del(&path);
}

void test_hmm_frame_state_len4(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 4));
    struct imm_seq seq = imm_seq(IMM_STR("ATCC"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -11.9829290512);

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    seq = imm_seq(IMM_STR("ATCC"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -11.9829290512);

    imm_del(task);
    imm_del(&dp);
    imm_del(&prod);
    imm_del(&path);
}

void test_hmm_frame_state_len5(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 5));
    struct imm_seq seq = imm_seq(IMM_STR("ACGTA"), abc);
    COND(imm_lprob_is_zero(imm_hmm_loglik(&hmm, &seq, &path)));

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 5));
    seq = imm_seq(IMM_STR("ACTAG"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -10.1142085574);

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    seq = imm_seq(IMM_STR("ACGTA"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    COND(imm_lprob_is_nan(prod.loglik));

    imm_hmm_reset_dp(&hmm, imm_super(&state), &dp);
    imm_task_reset(task, &dp);
    seq = imm_seq(IMM_STR("ACTAG"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -10.1142085574);

    imm_del(task);
    imm_del(&dp);
    imm_del(&prod);
    imm_del(&path);
}
