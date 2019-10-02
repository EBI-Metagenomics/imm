#ifndef IMM_PATH_H
#define IMM_PATH_H

#include <stddef.h>

struct imm_path;

const struct imm_path *path_next_item(const struct imm_path *item);
int path_get_state_id(const struct imm_path *item);
size_t path_get_seq_len(const struct imm_path *item);

#endif
