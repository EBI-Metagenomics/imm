#include "imm.h"
#include "src/imm/step.h"
#include "src/list.h"
#include <stdlib.h>

struct imm_path
{
    struct list_head steps;
};

struct imm_path* imm_path_create(void)
{
    struct imm_path* path = malloc(sizeof(struct imm_path));
    INIT_LIST_HEAD(&path->steps);
    return path;
}

void imm_path_destroy(struct imm_path* path)
{
    if (!path)
        return;

    struct list_head *entry = NULL, *tmp = NULL;
    list_for_each_safe(entry, tmp, &path->steps)
    {
        struct imm_step* step = list_entry(entry, struct imm_step, list);
        step->state = NULL;
        step->seq_len = -1;
        list_del(entry);
        free(step);
    }
    free(path);
}

int imm_path_add(struct imm_path* path, const struct imm_state* state, int seq_len)
{
    if (!path || !state) {
        imm_error("neither path nor state can be NULL");
        return 1;
    }
    if (seq_len < 0) {
        imm_error("seq_len cannot be negative");
        return 1;
    }
    if (seq_len < imm_state_min_seq(state) || imm_state_max_seq(state) < seq_len) {
        imm_error("seq_len outside the state's range");
        return 1;
    }
    struct imm_step* step = malloc(sizeof(struct imm_step));
    step->state = state;
    step->seq_len = seq_len;
    list_add_tail(&step->list, &path->steps);
    return 0;
}

void imm_path_prepend(struct imm_path* path, struct imm_state const* state, int seq_len)
{
    struct imm_step* step = malloc(sizeof(struct imm_step));
    step->state = state;
    step->seq_len = seq_len;
    list_add(&step->list, &path->steps);
}

struct imm_step const* imm_path_first(struct imm_path const* path)
{
    return list_first_entry_or_null(&path->steps, struct imm_step, list);
}

struct imm_step const* imm_path_next(struct imm_path const* path, struct imm_step const* step)
{
    const struct imm_step* next = list_next_entry(step, list);
    if (&path->steps != &next->list)
        return next;
    return NULL;
}
