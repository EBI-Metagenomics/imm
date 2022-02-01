/*      CWPack/goodies - cwpack_file_context.h   */
/*
 The MIT License (MIT)

 Copyright (c) 2017 Claes Wihlborg

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef cwpack_file_context_h
#define cwpack_file_context_h

#include "cwpack.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    cw_pack_context pc;
    FILE *file;
} stream_pack_context;

static int flush_stream_pack_context(struct cw_pack_context *pc)
{
    stream_pack_context *spc = (stream_pack_context *)pc;
    unsigned long contains = (unsigned long)(pc->current - pc->start);
    if (contains)
    {
        unsigned long rc = fwrite(pc->start, contains, 1, spc->file);
        if (rc != 1)
        {
            pc->err_no = ferror(spc->file);
            return CWP_RC_ERROR_IN_HANDLER;
        }
    }
    return CWP_RC_OK;
}

static int handle_stream_pack_overflow(struct cw_pack_context *pc,
                                       unsigned long more)
{
    int rc = flush_stream_pack_context(pc);
    if (rc != CWP_RC_OK) return rc;

    unsigned long buffer_length = (unsigned long)(pc->end - pc->start);
    if (buffer_length < more)
    {
        while (buffer_length < more)
            buffer_length = 2 * buffer_length;

        void *new_buffer = malloc(buffer_length);
        if (!new_buffer) return CWP_RC_BUFFER_OVERFLOW;

        free(pc->start);
        pc->start = (uint8_t *)new_buffer;
        pc->end = pc->start + buffer_length;
    }
    pc->current = pc->start;
    return CWP_RC_OK;
}

static void init_stream_pack_context(stream_pack_context *spc,
                                     unsigned long initial_buffer_length,
                                     FILE *file)
{
    unsigned long buffer_length =
        (initial_buffer_length > 0 ? initial_buffer_length : 4096);

    spc->file = file;
    if (!file)
    {
        spc->pc.return_code = CWP_RC_ERROR_IN_HANDLER;
        return;
    }
    void *buffer = malloc(buffer_length);
    if (!buffer)
    {
        spc->pc.return_code = CWP_RC_MALLOC_ERROR;
        return;
    }

    cw_pack_context_init((cw_pack_context *)spc, buffer, buffer_length,
                         &handle_stream_pack_overflow);
    cw_pack_set_flush_handler((cw_pack_context *)spc,
                              &flush_stream_pack_context);
}

static void terminate_stream_pack_context(stream_pack_context *spc)
{
    cw_pack_context *pc = (cw_pack_context *)spc;
    cw_pack_flush(pc);

    if (pc->return_code != CWP_RC_MALLOC_ERROR) free(pc->start);
}

typedef struct
{
    cw_unpack_context uc;
    unsigned long buffer_length;
    FILE *file;
} stream_unpack_context;

static int handle_stream_unpack_underflow(struct cw_unpack_context *uc,
                                          unsigned long more)
{
    stream_unpack_context *suc = (stream_unpack_context *)uc;
    unsigned long remains = (unsigned long)(uc->end - uc->current);
    if (remains)
    {
        memmove(uc->start, uc->current, remains);
    }

    if (suc->buffer_length < more)
    {
        while (suc->buffer_length < more)
            suc->buffer_length = 2 * suc->buffer_length;

        void *new_buffer = realloc(uc->start, suc->buffer_length);
        if (!new_buffer) return CWP_RC_BUFFER_UNDERFLOW;

        uc->start = (uint8_t *)new_buffer;
    }
    uc->current = uc->start;
    uc->end = uc->start + remains;
    unsigned long l =
        fread(uc->end, 1, suc->buffer_length - remains, suc->file);
    if (!l)
    {
        if (feof(suc->file)) return CWP_RC_END_OF_INPUT;
        suc->uc.err_no = ferror(suc->file);
        return CWP_RC_ERROR_IN_HANDLER;
    }

    uc->end += l;

    return CWP_RC_OK;
}

static void init_stream_unpack_context(stream_unpack_context *suc,
                                       unsigned long initial_buffer_length,
                                       FILE *file)
{
    unsigned long buffer_length =
        (initial_buffer_length > 0 ? initial_buffer_length : 1024);
    void *buffer = malloc(buffer_length);
    if (!buffer)
    {
        suc->uc.return_code = CWP_RC_MALLOC_ERROR;
        return;
    }
    suc->file = file;
    suc->buffer_length = buffer_length;

    cw_unpack_context_init((cw_unpack_context *)suc, buffer, 0,
                           &handle_stream_unpack_underflow);
}

static void terminate_stream_unpack_context(stream_unpack_context *suc)
{
    if (suc->uc.return_code != CWP_RC_MALLOC_ERROR) free(suc->uc.start);
}

static stream_pack_context spc = {0};
static stream_unpack_context suc = {0};

cw_pack_context *pack_ctx = &spc.pc;
cw_unpack_context *unpack_ctx = &suc.uc;

static inline void new_pack_ctx(char const *filepath)
{
    init_stream_pack_context(&spc, 1, fopen(filepath, "wb"));
}

static inline void del_pack_ctx(void)
{
    terminate_stream_pack_context(&spc);
    if (spc.file) fclose(spc.file);
}

static inline void new_unpack_ctx(char const *filepath)
{
    init_stream_unpack_context(&suc, 1, fopen(filepath, "rb"));
}

static inline void del_unpack_ctx(void)
{
    terminate_stream_unpack_context(&suc);
    if (suc.file) fclose(spc.file);
}

#endif /* cwpack_file_context_h */
