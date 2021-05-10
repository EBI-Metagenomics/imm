#include "hope/hope.h"
#include "imm/imm.h"

void test_hmm_frame_state_0eps(void);
/* void test_hmm_frame_state_len1(void); */
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
    /* test_hmm_frame_state_len1(); */
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
    struct imm_nuclt_lprob nucltp =
        imm_nuclt_lprob_init(nuclt, IMM_FARR(imm_log(0.25), imm_log(0.25),
                                             imm_log(0.5), imm_lprob_zero()));

    struct imm_codon_lprob codonp = imm_codon_lprob_init(nuclt);

    imm_codon_lprob_set(&codonp,
                        imm_codon_init(nuclt, IMM_TRIPLET('A', 'T', 'G')),
                        imm_log(0.8));
    imm_codon_lprob_set(&codonp,
                        imm_codon_init(nuclt, IMM_TRIPLET('A', 'T', 'T')),
                        imm_log(0.1));
    imm_codon_lprob_set(&codonp,
                        imm_codon_init(nuclt, IMM_TRIPLET('C', 'C', 'C')),
                        imm_log(0.1));

    struct imm_codon_marg codonm;
    imm_codon_marg_init(&codonm, &codonp);
    imm_codon_lprob_deinit(codonp);

    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_frame_state const *state =
        imm_frame_state_create("M", nucltp, codonm, 0.0);

    imm_hmm_add_state(hmm, imm_frame_state_super(state), imm_log(1.0));

    struct imm_path *path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 3));
    struct imm_seq const *seq = imm_seq_create("ATT", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -2.3025850930);
    imm_seq_destroy(seq);
    seq = imm_seq_create("ATG", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -0.2231435513142097);
    imm_seq_destroy(seq);
    seq = imm_seq_create("AT", abc);
    COND(!imm_lprob_is_valid(imm_hmm_loglikelihood(hmm, seq, path)));
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 2));
    seq = imm_seq_create("AT", abc);
    COND(imm_lprob_is_zero(imm_hmm_loglikelihood(hmm, seq, path)));
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    imm_base_abc_destroy(base);
    imm_codon_destroy(codon);
    imm_hmm_destroy(hmm);
    imm_frame_state_destroy(state);
    imm_codon_marg_destroy(codonm);
    imm_base_lprob_destroy(nucltp);
}

#if 0
void test_hmm_frame_state_len1(void)
{
    struct imm_base_abc const*   base = imm_base_abc_create("ACGT", 'X');
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

    struct imm_frame_state const* state = imm_frame_state_create("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_frame_state_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 1));
    struct imm_seq const* seq = imm_seq_create("A", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -6.0198640216);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 1));
    seq = imm_seq_create("C", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -7.118476310297789);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("A", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -6.0198640216);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -6.0198640216);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("C", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -7.1184763103);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -7.1184763103);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    imm_base_abc_destroy(base);
    imm_codon_destroy(codon);
    imm_hmm_destroy(hmm);
    imm_frame_state_destroy(state);
    imm_codon_marg_destroy(codonm);
    imm_base_lprob_destroy(nucltp);
}

void test_hmm_frame_state_len2(void)
{
    struct imm_base_abc const*   base = imm_base_abc_create("ACGT", 'X');
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

    struct imm_frame_state const* state = imm_frame_state_create("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_frame_state_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 2));
    struct imm_seq const* seq = imm_seq_create("AA", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -8.910235779525845);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 2));
    seq = imm_seq_create("TG", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -3.2434246977896133);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 2));
    seq = imm_seq_create("CC", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -4.225022885864217);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 2));
    seq = imm_seq_create("TT", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -5.326716841069734);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("AA", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -8.910235779525845);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -8.910235779525845);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("TG", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -3.2434246977896133);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -3.2434246977896133);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("CC", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -4.225022885864217);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -4.225022885864217);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("TT", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -5.326716841069734);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -5.326716841069734);
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
    struct imm_base_abc const*   base = imm_base_abc_create("ACGT", 'X');
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

    struct imm_frame_state const* state = imm_frame_state_create("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_frame_state_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 3));
    struct imm_seq const* seq = imm_seq_create("ATC", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -7.012344487235739);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 3));
    seq = imm_seq_create("ATG", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -0.639793371602465);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("ATC", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -7.012344487235739);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -7.012344487235739);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("ATG", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -0.639793371602465);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -0.639793371602465);
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
    struct imm_base_abc const*   base = imm_base_abc_create("ACGT", 'X');
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

    struct imm_frame_state const* state = imm_frame_state_create("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_frame_state_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 4));
    struct imm_seq const* seq = imm_seq_create("ATCC", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -11.982929094215963);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("ATCC", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -11.982929094215963);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -11.982929094215963);
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
    struct imm_base_abc const*   base = imm_base_abc_create("ACGT", 'X');
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

    struct imm_frame_state const* state = imm_frame_state_create("M", nucltp, codonm, (imm_float)0.1);

    imm_hmm_add_state(hmm, imm_frame_state_super(state), imm_log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 5));
    struct imm_seq const* seq = imm_seq_create("ACGTA", abc);
    COND(imm_lprob_is_zero(imm_hmm_loglikelihood(hmm, seq, path)));
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_frame_state_super(state), 5));
    seq = imm_seq_create("ACTAG", abc);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -10.11420858385178);
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("ACGTA", abc);
    COND(!imm_lprob_is_valid(single_viterbi(hmm, seq, imm_frame_state_super(state), path)));
    COND(!imm_lprob_is_valid(imm_hmm_loglikelihood(hmm, seq, path)));
    imm_seq_destroy(seq);
    imm_path_destroy(path);

    path = imm_path_create();
    seq = imm_seq_create("ACTAG", abc);
    cass_close(single_viterbi(hmm, seq, imm_frame_state_super(state), path), -10.11420858385178);
    cass_close(imm_hmm_loglikelihood(hmm, seq, path), -10.11420858385178);
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

    imm_float loglik = imm_hmm_loglikelihood(hmm, seq, path);
    imm_result_destroy(r);
    imm_dp_destroy(dp);

    return loglik;
}
#endif
