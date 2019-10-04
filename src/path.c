#define IMM_API_EXPORTS

#include "path.h"
#include "imm.h"
#include "utlist.h"
#include <stdlib.h>

struct imm_path
{
    int state_id;
    int seq_len;
    struct imm_path *next;
    struct imm_path *prev;
};

IMM_API void imm_path_create(struct imm_path **path) { *path = NULL; }

IMM_API void imm_path_add(struct imm_path **path, int state_id, int seq_len)
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

int path_item_state_id(const struct imm_path *item) { return item->state_id; }

int path_item_seq_len(const struct imm_path *item) { return item->seq_len; }

int path_seq_len(const struct imm_path *path)
{
    const struct imm_path *item = path;

    int seq_len = 0;
    while (item) {
        seq_len += path_item_seq_len(item);
        item = path_next_item(item);
    }

    return seq_len;
}
