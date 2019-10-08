#ifndef IMM_PATH_H
#define IMM_PATH_H

struct imm_path;

const struct imm_path *path_next_item(const struct imm_path *item);
int path_item_state_id(const struct imm_path *item);
int path_item_seq_len(const struct imm_path *item);
int path_seq_len(const struct imm_path *path);

#endif
