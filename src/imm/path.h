#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "src/imm/hide.h"

struct imm_path;
struct imm_state;
struct imm_step;

HIDE const struct imm_step *path_first_step(const struct imm_path *path);
HIDE const struct imm_step *path_next_step(const struct imm_path *path,
                                           const struct imm_step *step);
HIDE const struct imm_state *path_step_state(const struct imm_step *step);
HIDE int path_step_seq_len(const struct imm_step *step);
HIDE int path_seq_len(const struct imm_path *path);

#endif
