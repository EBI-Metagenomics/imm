#include "hope/hope.h"
#include "imm/imm.h"

void test_hmm_frame_state_0eps(void);
void test_hmm_frame_state_len1(void);
void test_hmm_frame_state_len2(void);
void test_hmm_frame_state_len3(void);
void test_hmm_frame_state_len4(void);
void test_hmm_frame_state_len5(void);

struct imm_nuclt const *nuclt;
struct imm_abc const *abc;
struct imm_nuclt_lprob nucltp;
struct imm_codon_lprob codonp;
struct imm_codon_marg codonm;

int main(void)
{
    nuclt = imm_super(&imm_dna_default);
    abc = imm_super(nuclt);

    nucltp = imm_nuclt_lprob(nuclt, IMM_ARR(imm_log(0.25), imm_log(0.25),
                                            imm_log(0.5), imm_lprob_zero()));

    codonp = imm_codon_lprob(nuclt);
    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'A', 'T', 'G'), imm_log(0.8));
    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'A', 'T', 'T'), imm_log(0.1));
    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'C', 'C', 'C'), imm_log(0.1));

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
    imm_hmm_init(&hmm, abc);

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
    imm_hmm_init(&hmm, abc);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 1));
    struct imm_seq seq = imm_seq(IMM_STR("A"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -6.0198640216);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 1));
    seq = imm_seq(IMM_STR("C"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -7.118476310297789);

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(&state));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    seq = imm_seq(IMM_STR("A"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -6.0198640216);

    imm_hmm_reset_dp(&hmm, imm_super(&state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq(IMM_STR("C"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -7.1184763103);

    imm_del(task);
    imm_del(&path);
    imm_del(&result);
    imm_del(dp);
}

void test_hmm_frame_state_len2(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, abc);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    struct imm_seq seq = imm_seq(IMM_STR("AA"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -8.910235779525845);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    seq = imm_seq(IMM_STR("TG"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -3.2434246977896133);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    seq = imm_seq(IMM_STR("CC"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -4.225022885864217);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 2));
    seq = imm_seq(IMM_STR("TT"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -5.326716841069734);

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(&state));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    seq = imm_seq(IMM_STR("AA"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -8.910235779525845);

    imm_hmm_reset_dp(&hmm, imm_super(&state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq(IMM_STR("TG"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -3.2434246977896133);

    imm_hmm_reset_dp(&hmm, imm_super(&state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq(IMM_STR("CC"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -4.225022885864217);

    imm_hmm_reset_dp(&hmm, imm_super(&state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq(IMM_STR("TT"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -5.326716841069734);

    imm_del(task);
    imm_del(dp);
    imm_del(&result);
    imm_del(&path);
}

void test_hmm_frame_state_len3(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, abc);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 3));
    struct imm_seq seq = imm_seq(IMM_STR("ATC"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -7.012344487235739);

    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_super(&state)->id, 3));
    seq = imm_seq(IMM_STR("ATG"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -0.639793371602465);

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(&state));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    seq = imm_seq(IMM_STR("ATC"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -7.012344487235739);

    imm_hmm_reset_dp(&hmm, imm_super(&state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq(IMM_STR("ATG"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -0.639793371602465);

    imm_del(task);
    imm_del(dp);
    imm_del(&result);
    imm_del(&path);
}

void test_hmm_frame_state_len4(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, abc);

    struct imm_frame_state state;
    imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_super(&state)->id, 4));
    struct imm_seq seq = imm_seq(IMM_STR("ATCC"), abc);
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -11.982929094215963);

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(&state));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    seq = imm_seq(IMM_STR("ATCC"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -11.982929094215963);

    imm_del(task);
    imm_del(dp);
    imm_del(&result);
    imm_del(&path);
}

void test_hmm_frame_state_len5(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, abc);

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
    CLOSE(imm_hmm_loglik(&hmm, &seq, &path), -10.11420858385178);

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(&state));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    seq = imm_seq(IMM_STR("ACGTA"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    COND(imm_lprob_is_nan(result.loglik));

    imm_hmm_reset_dp(&hmm, imm_super(&state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq(IMM_STR("ACTAG"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -10.11420858385178);

    imm_del(task);
    imm_del(dp);
    imm_del(&result);
    imm_del(&path);
}
