#include "aye.h"
#include "imm_path.h"

static void normal_path(void)
{
  uint16_t state_ids[] = {42, 19};
  uint8_t sizes[] = {3, 5};

  struct imm_path path = imm_path();

  aye(imm_path_add(&path, imm_step(state_ids[0], sizes[0], 0)) == 0);
  aye(imm_path_add(&path, imm_step(state_ids[1], sizes[1], 0)) == 0);

  aye(imm_path_step(&path, 0)->state_id == state_ids[0]);
  aye(imm_path_step(&path, 1)->state_id == state_ids[1]);

  aye(imm_path_step(&path, 0)->seqsize == sizes[0]);
  aye(imm_path_step(&path, 1)->seqsize == sizes[1]);

  imm_path_cleanup(&path);
}

static void long_path(void)
{
  struct imm_path path = imm_path();

  for (uint16_t i = 0; i < 1 << 4; ++i)
    aye(imm_path_add(&path, imm_step(i, i % 3, 0)) == 0);

  for (uint16_t i = 0; i < 1 << 4; ++i)
  {
    aye(imm_path_step(&path, i)->state_id == i);
    aye(imm_path_step(&path, i)->seqsize == i % 3);
  }

  imm_path_reset(&path);

  for (uint16_t i = 0; i < 1 << 4; ++i)
    aye(imm_path_add(&path, imm_step(i, i % 3, 0)) == 0);

  for (uint16_t i = 0; i < 1 << 4; ++i)
  {
    aye(imm_path_step(&path, i)->state_id == i);
    aye(imm_path_step(&path, i)->seqsize == i % 3);
  }

  imm_path_cleanup(&path);
}

static void cut1_path(void)
{
  uint16_t state_ids[] = {42, 19, 1};
  uint8_t sizes[] = {3, 5, 2};

  struct imm_path path = imm_path();

  aye(imm_path_add(&path, imm_step(state_ids[0], sizes[0], 0)) == 0);
  aye(imm_path_add(&path, imm_step(state_ids[1], sizes[1], 0)) == 0);
  aye(imm_path_add(&path, imm_step(state_ids[2], sizes[2], 0)) == 0);

  imm_path_cut(&path, 1, 2);
  aye(imm_path_step(&path, 0)->state_id == state_ids[1]);
  aye(imm_path_step(&path, 1)->state_id == state_ids[2]);

  aye(imm_path_step(&path, 0)->seqsize == sizes[1]);
  aye(imm_path_step(&path, 1)->seqsize == sizes[2]);

  imm_path_cleanup(&path);
}

static void cut2_path(void)
{
  uint16_t state_ids[] = {42, 19, 1};
  uint8_t sizes[] = {3, 5, 2};

  struct imm_path path = imm_path();

  aye(imm_path_add(&path, imm_step(state_ids[0], sizes[0], 0)) == 0);
  aye(imm_path_add(&path, imm_step(state_ids[1], sizes[1], 0)) == 0);
  aye(imm_path_add(&path, imm_step(state_ids[2], sizes[2], 0)) == 0);

  imm_path_reverse(&path);

  imm_path_cut(&path, 1, 2);
  aye(imm_path_step(&path, 0)->state_id == state_ids[1]);
  aye(imm_path_step(&path, 1)->state_id == state_ids[0]);

  aye(imm_path_step(&path, 0)->seqsize == sizes[1]);
  aye(imm_path_step(&path, 1)->seqsize == sizes[0]);

  imm_path_cleanup(&path);
}

int main(void)
{
  aye_begin();
  normal_path();
  long_path();
  cut1_path();
  cut2_path();
  return aye_end();
}
