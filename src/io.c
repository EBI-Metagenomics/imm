#include "io.h"
#include "support.h"

void write_array_u32_flt(uint32_t *size, imm_float const *data, FILE *stream)
{
    xfwrite(size, sizeof(*size), 1, stream);
    xfwrite(data, sizeof(*data), *size, stream);
}

void write_array_u32_u32(uint32_t *size, unsigned const *data, FILE *stream)
{
    xfwrite(size, sizeof(*size), 1, stream);
    for (unsigned i = 0; i < *size; ++i)
    {
        uint32_t val = data[i];
        xfwrite(&val, sizeof(val), 1, stream);
    }
}

imm_float *read_array_u32_flt(imm_float *data, FILE *stream)
{
    uint32_t size = 0;
    fread(&size, sizeof(size), 1, stream);
    data = xrealloc(data, sizeof(*data) * size);
    fread(data, sizeof(*data), size, stream);
    return data;
}

unsigned *read_array_u32_u32(unsigned *data, FILE *stream)
{
    uint32_t size = 0;
    fread(&size, sizeof(size), 1, stream);
    data = xrealloc(data, sizeof(*data) * size);
    for (unsigned i = 0; i < size; ++i)
    {
        uint32_t val = 0;
        fread(&val, sizeof(val), 1, stream);
        data[i] = val;
    }
    return data;
}

uint16_t *read_array_u16_flt_size(uint16_t *data, unsigned size, FILE *stream)
{
    data = xrealloc(data, sizeof(*data) * size);
    fread(data, sizeof(*data), size, stream);
    return data;
}
