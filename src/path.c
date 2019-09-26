#include "path.h"
#include "nhmm.h"
#include "utlist.h"
#include <stdlib.h>

#define NHMM_API_EXPORTS

struct nhmm_path
{
    int state_id;
    size_t seq_len;
    struct nhmm_path *next;
    struct nhmm_path *prev;
};

NHMM_API void nhmm_path_create(struct nhmm_path **path) { *path = NULL; }

NHMM_API void nhmm_path_add(struct nhmm_path **path, int state_id, size_t seq_len)
{
    struct nhmm_path *elem = malloc(sizeof(struct nhmm_path));
    elem->state_id = state_id;
    elem->seq_len = seq_len;
    DL_APPEND(*path, elem);
}

NHMM_API void nhmm_path_destroy(struct nhmm_path **path)
{
    struct nhmm_path *elem, *tmp;
    DL_FOREACH_SAFE(*path, elem, tmp)
    {
        elem->state_id = NHMM_INVALID_STATE_ID;
        elem->seq_len = 0;
        DL_DELETE(*path, elem);
        free(elem);
    }
}

const struct nhmm_path *path_next_item(const struct nhmm_path *item)
{
    return item->next;
}

int path_get_state_id(const struct nhmm_path *item) { return item->state_id; }

size_t path_get_seq_len(const struct nhmm_path *item) { return item->seq_len; }
