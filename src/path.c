#include "free.h"
#include "imm/imm.h"
#include "list.h"

struct imm_step
{
    struct imm_state const* state;
    unsigned                seq_len;
    struct list_head        list_entry;
};

struct imm_state const* imm_step_state(struct imm_step const* step) { return step->state; }

unsigned imm_step_seq_len(struct imm_step const* step) { return step->seq_len; }

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

void imm_path_destroy(struct imm_path const* path)
{
    struct list_head *entry = NULL, *tmp = NULL;
    list_for_each_safe(entry, tmp, &path->steps)
    {
        struct imm_step const* step = list_entry(entry, struct imm_step, list_entry);
        list_del(entry);
        free_c(step);
    }
    free_c(path);
}

static struct imm_step* create_step(struct imm_state const* state, unsigned const seq_len);

int imm_path_append(struct imm_path* path, struct imm_state const* state, unsigned seq_len)
{
    struct imm_step* step = create_step(state, seq_len);
    if (!step)
        return 1;
    list_add_tail(&step->list_entry, &path->steps);
    return 0;
}

int imm_path_prepend(struct imm_path* path, struct imm_state const* state, unsigned seq_len)
{
    struct imm_step* step = create_step(state, seq_len);
    if (!step)
        return 1;
    list_add(&step->list_entry, &path->steps);
    return 0;
}

struct imm_step const* imm_path_first(struct imm_path const* path)
{
    return list_first_entry_or_null(&path->steps, struct imm_step, list_entry);
}

struct imm_step const* imm_path_last(struct imm_path const* path)
{
    if (list_empty(&path->steps))
        return NULL;
    return list_last_entry(&path->steps, struct imm_step, list_entry);
}

struct imm_step const* imm_path_next(struct imm_path const* path, struct imm_step const* step)
{
    struct imm_step const* next = list_next_entry(step, list_entry);
    if (&path->steps != &next->list_entry)
        return next;
    return NULL;
}

static struct imm_step* create_step(struct imm_state const* state, unsigned seq_len)
{
    if (seq_len < imm_state_min_seq(state) || imm_state_max_seq(state) < seq_len) {
        imm_error("seq_len outside the state's range");
        return NULL;
    }
    struct imm_step* step = malloc(sizeof(struct imm_step));
    step->state = state;
    step->seq_len = seq_len;
    return step;
}
