#include "cass/cass.h"
#include "helper.h"
#include "imm/imm.h"
#include "model1.h"

static void check(struct model1 model, struct imm_results const* results, uint16_t i,
                  imm_float desired_score, char const* desired_str);

int main(void)
{
    char const* str = get_model1_str1();
    cass_cond(strlen(str) == 2000);

    struct model1        model = create_model1();
    struct imm_dp const* dp = imm_hmm_create_dp(model.hmm, imm_normal_state_super(model.end));

    struct imm_seq const* seq = imm_seq_create(str, model.abc);
    struct imm_dp_task*   task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq, 50);
    struct imm_results const* results = imm_dp_viterbi(dp, task);

    cass_cond(imm_results_size(results) == 79);

    imm_float desired = -1778.8892020572;
    imm_float invalid = imm_lprob_invalid();

    check(model, results, 0, desired, "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM");
    check(model, results, 1, invalid, "MIMMMMMMMMMIIMIMIMIMIMIIMIIIMIMIMIMMMMMMIMMIMIMIMI");
    check(model, results, 2, invalid, "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ");
    check(model, results, 3, invalid, "IMIMMIMIMIMIMIMMMMIMMIMEJBMIIMIIMMIMMMIMEJBMIIMIIM");
    check(model, results, 4, desired, "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM");
    check(model, results, 8, desired, "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM");

    imm_seq_destroy(seq);
    imm_dp_task_destroy(task);

    imm_results_destroy(results);
    imm_dp_destroy(dp);
    destroy_model1(model);

    return cass_status();
}

static void check(struct model1 model, struct imm_results const* results, uint16_t i,
                  imm_float desired_score, char const* desired_str)
{
    struct imm_result const* result = imm_results_get(results, i);
    struct imm_subseq        subseq = imm_result_subseq(result);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    imm_float                score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
    cass_close(score, desired_score);
    cass_cond(strncmp(imm_seq_string(s), desired_str, imm_seq_length(s)) == 0);
}
