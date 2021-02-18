#include "free.h"
#include "imm/imm.h"
#include "minmax.h"
#include <limits.h>
#include <stdbool.h>

struct imm_window
{
    struct imm_seq const* seq;
    uint16_t              length;
    uint16_t              offset;
    bool                  end;
};

struct imm_window* imm_window_create(struct imm_seq const* seq, uint16_t length)
{
    length = (uint16_t)MIN((uint_fast32_t)length, (uint_fast32_t)imm_seq_length(seq));

    struct imm_window* window = malloc(sizeof(*window));
    window->seq = seq;
    window->length = length;
    window->offset = 0;
    window->end = false;

    return window;
}

void imm_window_destroy(struct imm_window const* window) { free_c(window); }

struct imm_subseq imm_window_get(struct imm_window const* window, uint16_t index)
{
    uint16_t const offset = MAX(window->length / 2, (uint16_t)1) * index;
    uint16_t const length =
        (uint16_t)MIN((uint_fast32_t)window->length, (uint_fast32_t)imm_seq_length(window->seq) - offset);

    IMM_SUBSEQ(subseq, window->seq, offset, length);

    return subseq;
}

uint16_t imm_window_size(struct imm_window const* window)
{
    uint16_t size = 0;
    uint16_t offset = 0;

    uint16_t length = 0;
    uint16_t poffset = 0;
    do {
        poffset = offset;
        offset += MAX(window->length / 2, (uint16_t)1);
        length = (uint16_t)MIN((uint_fast32_t)window->length, (uint_fast32_t)imm_seq_length(window->seq) - poffset);
        ++size;

    } while (imm_seq_length(window->seq) > poffset + length);

    return size;
}
