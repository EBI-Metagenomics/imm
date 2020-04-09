#include "io_abc.h"
#include "imm/abc.h"
#include "imm/report.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

struct io_abc_chunk
{
    uint16_t    chunk_size;
    uint16_t    symbols_size;
    char const* symbols;
    char        any_symbol;
};

int io_abc_write(FILE* stream, struct imm_abc const* abc)
{
    struct io_abc_chunk chunk = {.chunk_size = 0,
                                 .symbols_size = (uint16_t)(strlen(abc->symbols) + 1),
                                 .symbols = abc->symbols};

    chunk.chunk_size = (sizeof(chunk.chunk_size) + sizeof(chunk.symbols_size) +
                        chunk.symbols_size + sizeof(chunk.any_symbol));

    if (fwrite(&chunk.chunk_size, sizeof(chunk.chunk_size), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.symbols_size, sizeof(chunk.symbols_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.symbols, chunk.symbols_size, 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1)
        return 1;

    return 0;
}
