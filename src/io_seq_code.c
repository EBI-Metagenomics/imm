#include "io_seq_code.h"
#include "cast.h"
#include "seq_code.h"
#include <inttypes.h>
#include <limits.h>

struct io_seq_code_chunk
{
    uint32_t  chunk_size;
    uint8_t   min_seq;
    uint8_t   max_seq;
    uint32_t* offset;
    uint32_t* stride;
    uint32_t  size;
};

int io_seq_code_write(FILE* stream, struct seq_code const* seq_code)
{
    struct io_seq_code_chunk chunk = {.chunk_size = 0,
                                      .min_seq = seq_code->min_seq,
                                      .max_seq = seq_code->max_seq,
                                      .offset = seq_code->offset,
                                      .stride = seq_code->stride,
                                      .size = seq_code->size};

    chunk.chunk_size =
        (sizeof(chunk.chunk_size) + sizeof(chunk.min_seq) + sizeof(chunk.max_seq) +
         sizeof(*chunk.offset) * seq_code_offset_size(seq_code) +
         sizeof(*chunk.stride) * seq_code_stride_size(seq_code) + sizeof(chunk.size));

    if (fwrite(&chunk.chunk_size, sizeof(chunk.chunk_size), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.offset, sizeof(*chunk.offset) * seq_code_offset_size(seq_code), 1,
               stream) < 1)
        return 1;

    if (fwrite(chunk.stride, sizeof(*chunk.stride) * seq_code_stride_size(seq_code), 1,
               stream) < 1)
        return 1;

    if (fwrite(&chunk.size, sizeof(chunk.size), 1, stream) < 1)
        return 1;

    return 0;
}
