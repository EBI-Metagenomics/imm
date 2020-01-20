#include "imm/imm.h"
#include "step_p.h"
#include <stdlib.h>

struct imm_path
{
    struct list_head steps;
};

static struct imm_step* create_step(struct imm_state const* state, int seq_len);

struct imm_path* imm_path_create(void)
{
    struct imm_path* path = malloc(sizeof(struct imm_path));
    INIT_LIST_HEAD(&path->steps);
    return path;
}

void imm_path_destroy(struct imm_path const* path)
{
    if (!path) {
        imm_error("path should not be NULL");
        return;
    }

    struct list_head *entry = NULL, *tmp = NULL;
    list_for_each_safe(entry, tmp, &path->steps)
    {
        struct imm_step* step = list_entry(entry, struct imm_step, list);
        step->state = NULL;
        step->seq_len = -1;
        list_del(entry);
        free(step);
    }
    free((struct imm_path*)path);
}

int imm_path_append(struct imm_path* path, struct imm_state const* state, int seq_len)
{
    if (!path) {
        imm_error("path cannot be NULL");
        return 1;
    }
    struct imm_step* step = create_step(state, seq_len);
    if (!step)
        return 1;
    list_add_tail(&step->list, &path->steps);
    return 0;
}

int imm_path_prepend(struct imm_path* path, struct imm_state const* state, int seq_len)
{
    if (!path) {
        imm_error("path cannot be NULL");
        return 1;
    }
    struct imm_step* step = create_step(state, seq_len);
    if (!step)
        return 1;
    list_add(&step->list, &path->steps);
    return 0;
}

struct imm_step const* imm_path_first(struct imm_path const* path)
{
    return list_first_entry_or_null(&path->steps, struct imm_step, list);
}

struct imm_step const* imm_path_last(struct imm_path const* path)
{
    if (list_empty(&path->steps))
        return NULL;
    return list_last_entry(&path->steps, struct imm_step, list);
}

struct imm_step const* imm_path_next(struct imm_path const* path, struct imm_step const* step)
{
    struct imm_step const* next = list_next_entry(step, list);
    if (&path->steps != &next->list)
        return next;
    return NULL;
}

static struct imm_step* create_step(struct imm_state const* state, int seq_len)
{
    if (!state) {
        imm_error("state cannot be NULL");
        return NULL;
    }
    if (seq_len < 0) {
        imm_error("seq_len cannot be negative");
        return NULL;
    }
    if (seq_len < imm_state_min_seq(state) || imm_state_max_seq(state) < seq_len) {
        imm_error("seq_len outside the state's range");
        return NULL;
    }
    struct imm_step* step = malloc(sizeof(struct imm_step));
    step->state = state;
    step->seq_len = seq_len;
    return step;
}
