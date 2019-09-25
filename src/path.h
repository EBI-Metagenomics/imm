#ifndef NHMM_PATH_H
#define NHMM_PATH_H

#include <stddef.h>

struct nhmm_path;

const struct nhmm_path *path_next_item(const struct nhmm_path *item);
int path_get_state_id(const struct nhmm_path *item);
size_t path_get_seq_len(const struct nhmm_path *item);

#endif
