#include "imm/window.h"
#include "free.h"
#include "imm/report.h"
#include "imm/subseq.h"
#include "min.h"
#include <limits.h>
#include <stdbool.h>

struct imm_window
{
    struct imm_seq const* seq;
    unsigned              length;
    unsigned              offset;
    bool                  end;
};

struct imm_window* imm_window_create(struct imm_seq const* seq, unsigned length)
{
    length = MIN(length, imm_seq_length(seq));

    struct imm_window* window = malloc(sizeof(struct imm_window));
    window->seq = seq;
    window->length = length;
    window->offset = 0;
    window->end = false;

    return window;
}

struct imm_subseq imm_window_next(struct imm_window* window)
{
    unsigned const offset = window->offset;
    unsigned const length = MIN(window->length, imm_seq_length(window->seq) - offset);

    IMM_SUBSEQ(subseq, window->seq, offset, length);

    if (window->end) {
        imm_error("there is no next windows");
        return subseq;
    }

    window->offset += MAX(window->length / 2, 1);
    if (imm_seq_length(window->seq) <= offset + length)
        window->end = true;

    return subseq;
}

bool imm_window_end(struct imm_window const* window) { return window->end; }

unsigned imm_window_size(struct imm_window const* window)
{
    unsigned size = 0;
    unsigned offset = 0;

    unsigned length = 0;
    unsigned poffset = 0;
    do {
        poffset = offset;
        offset += MAX(window->length / 2, 1);
        length = MIN(window->length, imm_seq_length(window->seq) - poffset);
        ++size;

    } while (imm_seq_length(window->seq) > poffset + length);

    return size;
}

void imm_window_destroy(struct imm_window const* window) { free_c(window); }
