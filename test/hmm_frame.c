#include "hope/hope.h"
#include "imm/dna.h"
#include "imm/imm.h"

void test_hmm_frame_state_0eps(void);
void test_hmm_frame_state_len1(void);
/* void test_hmm_frame_state_len2(void); */
/* void test_hmm_frame_state_len3(void); */
/* void test_hmm_frame_state_len4(void); */
/* void test_hmm_frame_state_len5(void); */

/* imm_float single_viterbi(struct imm_hmm const* hmm, struct imm_seq const*
 * seq, struct imm_state const* end_state, */
/*                          struct imm_path* path); */

int main(void)
{
    test_hmm_frame_state_0eps();
    test_hmm_frame_state_len1();
    /* test_hmm_frame_state_len2(); */
    /* test_hmm_frame_state_len3(); */
    /* test_hmm_frame_state_len4(); */
    /* test_hmm_frame_state_len5(); */
    return hope_status();
}

void test_hmm_frame_state_0eps(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp =
        imm_nuclt_lprob_init(nuclt, IMM_FARR(imm_log(0.25), imm_log(0.25),
                                             imm_log(0.5), imm_lprob_zero()));

    struct imm_codon_lprob codonp = imm_codon_lprob_init(nuclt);

    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, 'A', 'T', 'G'), imm_log(0.8));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, 'A', 'T', 'T'), imm_log(0.1));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, 'C', 'C', 'C'), imm_log(0.1));

    struct imm_codon_marg codonm = imm_codon_marg_init(&codonp);

    struct imm_hmm *hmm = imm_hmm_new(imm_super(nuclt));

    struct imm_frame_state *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.0);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(1.0));

    struct imm_path path = imm_path_init();
    imm_path_add(&path, IMM_STEP(imm_super(state)->id, 3));
    struct imm_seq seq = imm_seq_init(IMM_STR("ATT"), abc);
    CLOSE(imm_hmm_loglik(hmm, &seq, &path), -2.3025850930);
    seq = imm_seq_init(IMM_STR("ATG"), abc);
    CLOSE(imm_hmm_loglik(hmm, &seq, &path), -0.2231435513142097);
    seq = imm_seq_init(IMM_STR("AT"), abc);
    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &seq, &path)));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state);
}

void test_hmm_frame_state_len1(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);
    struct imm_abc const *abc = imm_super(nuclt);
    struct imm_nuclt_lprob nucltp =
        imm_nuclt_lprob_init(nuclt, IMM_FARR(imm_log(0.25), imm_log(0.25),
                                             imm_log(0.5), imm_lprob_zero()));

    struct imm_codon_lprob codonp = imm_codon_lprob_init(nuclt);

    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, 'A', 'T', 'G'), imm_log(0.8));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, 'A', 'T', 'T'), imm_log(0.1));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, 'C', 'C', 'C'), imm_log(0.1));

    struct imm_codon_marg codonm = imm_codon_marg_init(&codonp);

    struct imm_hmm *hmm = imm_hmm_new(imm_super(nuclt));

    struct imm_frame_state *state =
        imm_frame_state_new(0, &nucltp, &codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(1.0));

    struct imm_path path = imm_path_init();
    imm_path_add(&path, IMM_STEP(imm_super(state)->id, 1));
    struct imm_seq seq = imm_seq_init(IMM_STR("A"), abc);
    CLOSE(imm_hmm_loglik(hmm, &seq, &path), -6.0198640216);

    path = imm_path_init();
    imm_path_add(&path, IMM_STEP(imm_super(state)->id, 1));
    seq = imm_seq_init(IMM_STR("C"), abc);
    CLOSE(imm_hmm_loglik(hmm, &seq, &path), -7.118476310297789);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result_init();

    seq = imm_seq_init(IMM_STR("A"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -6.0198640216);

    dp = imm_hmm_reset_dp(hmm, imm_super(state), dp);
    imm_task_reset(task, dp);
    seq = imm_seq_init(IMM_STR("C"), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -7.1184763103);

    imm_deinit(&path);
    imm_del(dp);
    imm_del(hmm);
    imm_del(state);
}

#if 0
void test_hmm_frame_state_len2(void)
{
    struct imm_nuclt const*   base = imm_base_abc_create("ACGT", 'X');
    struct imm_abc const*        abc = imm_base_abc_super(base);
    imm_float const              imm_lprob_zero() = imm_lprob_zero();
    struct imm_nuclt_lprob const* nucltp = imm_base_lprob_create(base, imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero());

    struct imm_codon_lprob* codonp = imm_codon_lprob_create(base);
    struct imm_codon*       codon = imm_codon_create(base);
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.8));
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    COND(imm_codon_set(&codon, IMM_TRIPLET('C', 'C', 'C')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    struct imm_codon_marg const* codonm = imm_codon_marg_create(codonp);
    imm_codon_lprob_destroy(codonp);

    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_frame_state const* state = imm_frame_state_new("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 2));
    struct imm_seq const* seq = imm_seq_init("AA", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -8.910235779525845);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 2));
    seq = imm_seq_init("TG", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -3.2434246977896133);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 2));
    seq = imm_seq_init("CC", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -4.225022885864217);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 2));
    seq = imm_seq_init("TT", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -5.326716841069734);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("AA", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -8.910235779525845);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -8.910235779525845);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("TG", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -3.2434246977896133);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -3.2434246977896133);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("CC", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -4.225022885864217);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -4.225022885864217);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("TT", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -5.326716841069734);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -5.326716841069734);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    imm_base_abc_destroy(base);
    imm_codon_destroy(codon);
    imm_hmm_destroy(hmm);
    imm_frame_state_destroy(state);
    imm_codon_marg_destroy(codonm);
    imm_base_lprob_destroy(nucltp);
}

void test_hmm_frame_state_len3(void)
{
    struct imm_nuclt const*   base = imm_base_abc_create("ACGT", 'X');
    struct imm_abc const*        abc = imm_base_abc_super(base);
    imm_float const              imm_lprob_zero() = imm_lprob_zero();
    struct imm_nuclt_lprob const* nucltp = imm_base_lprob_create(base, imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero());

    struct imm_codon_lprob* codonp = imm_codon_lprob_create(base);
    struct imm_codon*       codon = imm_codon_create(base);
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.8));
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    COND(imm_codon_set(&codon, IMM_TRIPLET('C', 'C', 'C')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    struct imm_codon_marg const* codonm = imm_codon_marg_create(codonp);
    imm_codon_lprob_destroy(codonp);

    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_frame_state const* state = imm_frame_state_new("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 3));
    struct imm_seq const* seq = imm_seq_init("ATC", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -7.012344487235739);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 3));
    seq = imm_seq_init("ATG", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -0.639793371602465);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("ATC", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -7.012344487235739);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -7.012344487235739);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("ATG", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -0.639793371602465);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -0.639793371602465);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    imm_base_abc_destroy(base);
    imm_codon_destroy(codon);
    imm_hmm_destroy(hmm);
    imm_frame_state_destroy(state);
    imm_codon_marg_destroy(codonm);
    imm_base_lprob_destroy(nucltp);
}

void test_hmm_frame_state_len4(void)
{
    struct imm_nuclt const*   base = imm_base_abc_create("ACGT", 'X');
    struct imm_abc const*        abc = imm_base_abc_super(base);
    imm_float const              imm_lprob_zero() = imm_lprob_zero();
    struct imm_nuclt_lprob const* nucltp = imm_base_lprob_create(base, imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero());

    struct imm_codon_lprob* codonp = imm_codon_lprob_create(base);
    struct imm_codon*       codon = imm_codon_create(base);
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.8));
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    COND(imm_codon_set(&codon, IMM_TRIPLET('C', 'C', 'C')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    struct imm_codon_marg const* codonm = imm_codon_marg_create(codonp);
    imm_codon_lprob_destroy(codonp);

    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_frame_state const* state = imm_frame_state_new("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 4));
    struct imm_seq const* seq = imm_seq_init("ATCC", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -11.982929094215963);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("ATCC", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -11.982929094215963);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -11.982929094215963);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    imm_base_abc_destroy(base);
    imm_codon_destroy(codon);
    imm_hmm_destroy(hmm);
    imm_frame_state_destroy(state);
    imm_codon_marg_destroy(codonm);
    imm_base_lprob_destroy(nucltp);
}

void test_hmm_frame_state_len5(void)
{
    struct imm_nuclt const*   base = imm_base_abc_create("ACGT", 'X');
    struct imm_abc const*        abc = imm_base_abc_super(base);
    imm_float const              imm_lprob_zero() = imm_lprob_zero();
    struct imm_nuclt_lprob const* nucltp = imm_base_lprob_create(base, imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero());

    struct imm_codon_lprob* codonp = imm_codon_lprob_create(base);
    struct imm_codon*       codon = imm_codon_create(base);
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.8));
    COND(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    COND(imm_codon_set(&codon, IMM_TRIPLET('C', 'C', 'C')) == 0);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));
    struct imm_codon_marg const* codonm = imm_codon_marg_create(codonp);
    imm_codon_lprob_destroy(codonp);

    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_frame_state const* state = imm_frame_state_new("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 5));
    struct imm_seq const* seq = imm_seq_init("ACGTA", abc);
    COND(imm_lprob_is_zero(imm_hmm_loglik(hmm, seq, path)));
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    imm_path_append(path, imm_step_create(imm_super(state), 5));
    seq = imm_seq_init("ACTAG", abc);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -10.11420858385178);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("ACGTA", abc);
    COND(!imm_lprob_is_valid(single_viterbi(hmm, seq, imm_super(state), path)));
    COND(!imm_lprob_is_valid(imm_hmm_loglik(hmm, seq, path)));
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_init();
    seq = imm_seq_init("ACTAG", abc);
    CLOSE(single_viterbi(hmm, seq, imm_super(state), path), -10.11420858385178);
    CLOSE(imm_hmm_loglik(hmm, seq, path), -10.11420858385178);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    imm_base_abc_destroy(base);
    imm_codon_destroy(codon);
    imm_hmm_destroy(hmm);
    imm_frame_state_destroy(state);
    imm_codon_marg_destroy(codonm);
    imm_base_lprob_destroy(nucltp);
}

imm_float single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq, struct imm_state const* end_state,
                         struct imm_path* path)
{
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, end_state);
    struct imm_dp_task*  task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq);
    struct imm_result const* r = imm_dp_viterbi(dp, task);
    imm_dp_task_destroy(task);
    if (r == NULL)
        return imm_lprob_invalid();

    struct imm_path const* src = imm_result_path(r);
    for (struct imm_step const* step = imm_path_first(src); step; step = imm_path_next(src, step))
        imm_path_append(path, imm_step_create(imm_step_state(step), imm_step_seq_len(step)));

    imm_float loglik = imm_hmm_loglik(hmm, seq, path);
    imm_result_destroy(r);
    imm_dp_destroy(dp);

    return loglik;
}
#endif
