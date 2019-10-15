#include "src/imm/path.h"
#include "imm.h"
#include "src/linux/list.h"
#include <stdlib.h>

struct imm_step
{
    const struct imm_state *state;
    int seq_len;
    struct list_head list;
};

struct imm_path
{
    struct list_head steps;
};

struct imm_path *imm_path_create(void)
{
    struct imm_path *path = malloc(sizeof(struct imm_path));
    INIT_LIST_HEAD(&path->steps);
    return path;
}

void imm_path_add(struct imm_path *path, const struct imm_state *state, int seq_len)
{
    struct imm_step *step = malloc(sizeof(struct imm_step));
    step->state = state;
    step->seq_len = seq_len;
    list_add_tail(&step->list, &path->steps);
}

void imm_path_destroy(struct imm_path *path)
{
    struct list_head *entry = NULL, *tmp = NULL;
    list_for_each_safe(entry, tmp, &path->steps)
    {
        struct imm_step *step = list_entry(entry, struct imm_step, list);
        step->state = NULL;
        step->seq_len = -1;
        list_del(entry);
        free(step);
    }
}

const struct imm_step *path_first_step(const struct imm_path *path)
{
    return list_first_entry_or_null(&path->steps, struct imm_step, list);
}

const struct imm_step *path_next_step(const struct imm_path *path, const struct imm_step *step)
{
    const struct imm_step *next = list_next_entry(step, list);
    if (&path->steps != &next->list)
        return next;
    return NULL;
}

const struct imm_state *path_step_state(const struct imm_step *step) { return step->state; }

int path_step_seq_len(const struct imm_step *step) { return step->seq_len; }

int path_seq_len(const struct imm_path *path)
{
    const struct imm_step *step = path_first_step(path);

    int seq_len = 0;
    while (step) {
        seq_len += path_step_seq_len(step);
        step = path_next_step(path, step);
    }

    return seq_len;
}
