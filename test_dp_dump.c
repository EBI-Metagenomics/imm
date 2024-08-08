#include "aye.h"
#include "imm_dp.h"
#include "imm_eseq.h"
#include "imm_ex1.h"
#include "imm_prod.h"
#include "imm_task.h"
#include "near.h"

static void dp_dump_ex1(void)
{
  imm_ex1_init(10);
  struct imm_dp dp;
  aye(imm_hmm_init_dp(imm_ex1.hmm, &dp) == 0);

  FILE *fp = fopen("dp_ex1.dump", "wb");
  imm_dp_set_state_name(&dp, &imm_ex1_state_name);
  imm_dp_dump(&dp, fp);
  fclose(fp);

  imm_dp_cleanup(&dp);
  remove("dp_ex1.dump");
  imm_ex1_cleanup();
}

static void dp_dump_ex1_path(void)
{
  imm_ex1_init(10);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  aye(imm_hmm_init_dp(imm_ex1.hmm, &dp) == 0);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();
  struct imm_seq seq = imm_seq_unsafe(imm_str("BMMMEJBMMME"), &m->abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -41.845375f);

  imm_path_dump(&prod.path, &imm_ex1_state_name, &seq, fopen("/dev/null", "w"));

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_ex1_cleanup();
}

int main(void)
{
  aye_begin();
  dp_dump_ex1();
  dp_dump_ex1_path();
  return aye_end();
}
