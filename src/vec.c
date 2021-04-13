#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>
#include <string.h>

struct imm_veci
{
    int*   data;
    size_t length;
    size_t capacity;
};

struct imm_vecp
{
    void const** data;
    size_t       length;
    size_t       capacity;
};

void imm_veci_append(struct imm_veci* vec, int item)
{
    if (vec->length == vec->capacity) {
        vec->capacity = vec->capacity * 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(*vec->data));
    }
    vec->data[vec->length] = item;
    vec->length++;
}

struct imm_veci* imm_veci_create(void)
{
    struct imm_veci* vec = xmalloc(sizeof(*vec));
    vec->data = xmalloc(sizeof(*vec->data));
    vec->length = 0;
    vec->capacity = 1;
    return vec;
}

void imm_veci_destroy(struct imm_veci const* vec)
{
    free(vec->data);
    free((struct imm_veci*)vec);
}

int imm_veci_get(struct imm_veci const* vec, size_t i) { return vec->data[i]; }

size_t imm_veci_size(struct imm_veci const* vec) { return vec->length; }

void imm_vecp_append(struct imm_vecp* vec, void const* item)
{
    if (vec->length == vec->capacity) {
        vec->capacity = vec->capacity * 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(*vec->data));
    }
    vec->data[vec->length] = item;
    vec->length++;
}

struct imm_vecp* imm_vecp_create(void)
{
    struct imm_vecp* vec = xmalloc(sizeof(*vec));
    vec->data = xmalloc(sizeof(*vec->data));
    vec->length = 0;
    vec->capacity = 1;
    return vec;
}

void imm_vecp_destroy(struct imm_vecp const* vec)
{
    free(vec->data);
    free((struct imm_vecp*)vec);
}

void const* imm_vecp_get(struct imm_vecp const* vec, size_t i) { return vec->data[i]; }

size_t imm_vecp_size(struct imm_vecp const* vec) { return vec->length; }
