#include "free.h"
#include "imm/imm.h"
#include "list.h"
#include "step.h"

struct imm_path
{
    struct list_head steps;
};

void imm_path_append(struct imm_path* path, struct imm_step* step) { list_add_tail(&step->list_entry, &path->steps); }

struct imm_path* imm_path_clone(struct imm_path const* path)
{
    struct imm_path* new_path = malloc(sizeof(*new_path));
    INIT_LIST_HEAD(&new_path->steps);

    struct list_head* entry = NULL;
    list_for_each(entry, &path->steps)
    {
        struct imm_step const* step = list_entry(entry, struct imm_step, list_entry);
        struct imm_step*       new_step = imm_step_clone(step);
        imm_path_append(new_path, new_step);
    }

    return new_path;
}

struct imm_path* imm_path_create(void)
{
    struct imm_path* path = malloc(sizeof(struct imm_path));
    INIT_LIST_HEAD(&path->steps);
    return path;
}

void imm_path_destroy(struct imm_path const* path)
{
    struct list_head *entry = NULL, *tmp = NULL;
    list_for_each_safe (entry, tmp, &path->steps) {
        struct imm_step const* step = list_entry(entry, struct imm_step, list_entry);
        list_del(entry);
        imm_step_destroy(step);
    }
    imm_path_free(path);
}

bool imm_path_empty(struct imm_path const* path) { return imm_path_first(path) == NULL; }

struct imm_step const* imm_path_first(struct imm_path const* path)
{
    return list_first_entry_or_null(&path->steps, struct imm_step, list_entry);
}

void imm_path_free(struct imm_path const* path) { free_c(path); }

struct imm_step const* imm_path_next(struct imm_path const* path, struct imm_step const* step)
{
    struct imm_step const* next = list_next_entry(step, list_entry);
    if (&path->steps != &next->list_entry)
        return next;
    return NULL;
}

void imm_path_prepend(struct imm_path* path, struct imm_step* step) { list_add(&step->list_entry, &path->steps); }
