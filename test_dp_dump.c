#include "imm/imm.h"
#include "vendor/minctest.h"

static void dp_dump_ex1(void)
{
  imm_ex1_init(10);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  eq(imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp), 0);

  FILE *fp = fopen("dp_ex1.dump", "wb");
  imm_dp_dump(&dp, &imm_ex1_state_name, fp);
  fclose(fp);

  imm_dp_del(&dp);
  remove("dp_ex1.dump");
}

static void dp_dump_ex1_path(void)
{
  imm_ex1_init(10);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  eq(imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp), 0);

  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();
  struct imm_seq seq = imm_seq(imm_str("BMMMEJBMMME"), &m->abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -41.845375);

  imm_path_dump(&prod.path, &imm_ex1_state_name, &seq, stdout);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

int main(void)
{
  lrun("dp_dump_ex1", dp_dump_ex1);
  lrun("dp_dump_ex1_path", dp_dump_ex1_path);
  return lfails != 0;
}
