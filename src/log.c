#include "imm/log.h"
#include "log.h"
#include <stdio.h>

static void default_print(char const *msg, void *arg)
{
    (void)arg;
    fprintf(stderr, "%s\n", msg);
}

static imm_log_print_t *__log_print = default_print;
static void *__log_arg = NULL;

void imm_log_setup(imm_log_print_t *print, void *arg)
{
    __log_print = print;
    __log_arg = arg;
}

void log_print(char const *msg) { __log_print(msg, __log_arg); }
