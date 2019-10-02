#include "path.h"
#include "imm.h"
#include "utlist.h"
#include <stdlib.h>

#define IMM_API_EXPORTS

struct imm_path
{
    int state_id;
    size_t seq_len;
    struct imm_path *next;
    struct imm_path *prev;
};

IMM_API void imm_path_create(struct imm_path **path) { *path = NULL; }

IMM_API void imm_path_add(struct imm_path **path, int state_id, size_t seq_len)
{
    struct imm_path *elem = malloc(sizeof(struct imm_path));
    elem->state_id = state_id;
    elem->seq_len = seq_len;
    DL_APPEND(*path, elem);
}

IMM_API void imm_path_destroy(struct imm_path **path)
{
    struct imm_path *elem, *tmp;
    DL_FOREACH_SAFE(*path, elem, tmp)
    {
        elem->state_id = IMM_INVALID_STATE_ID;
        elem->seq_len = 0;
        DL_DELETE(*path, elem);
        free(elem);
    }
}

const struct imm_path *path_next_item(const struct imm_path *item)
{
    return item->next;
}

int path_get_state_id(const struct imm_path *item) { return item->state_id; }

size_t path_get_seq_len(const struct imm_path *item) { return item->seq_len; }
