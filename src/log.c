#include "log.h"
#include "imm/log.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void default_print(char const *msg, void *arg)
{
    fprintf(stderr, "%s\n", msg);
}

static void default_flush(void *arg) { fflush(stderr); }

static imm_log_error_t *log_error = default_print;
static imm_log_fatal_t *log_fatal = default_print;
static imm_log_flush_t *log_flush = default_flush;
static void *log_arg = NULL;

void imm_log_setup(imm_log_error_t *error, imm_log_fatal_t *fatal,
                   imm_log_flush_t *flush, void *arg)
{
    log_error = error;
    log_fatal = fatal;
    log_flush = flush;
    log_arg = arg;
}

enum imm_rc __log_error(enum imm_rc rc, char const *msg)
{
    log_error(msg, log_arg);
    return rc;
}
