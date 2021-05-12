#include "imm/log.h"
#include "imm/compiler.h"
#include "support.h"

static char const *__msg[] = {
    [IMM_SUCCESS] = "success",          [IMM_FAILURE] = "failure",
    [IMM_OUTOFMEM] = "out of memory",   [IMM_ILLEGALARG] = "illegal argument",
    [IMM_IOERROR] = "i/o error",        [IMM_PARSEERROR] = "parse error",
    [IMM_RUNTIMEERROR] "runtime error",
};

static void default_print(struct imm_log_event event);

static imm_log_callback log_callback = default_print;
enum imm_level log_level = IMM_WARN;

void imm_log_setup(imm_log_callback cb, enum imm_level level)
{
    log_callback = cb;
    log_level = level;
}

int __imm_log_impl(enum imm_level level, enum imm_code code, char const *file,
                   int line, char const *fmt, ...)
{
    if (level < log_level)
        return (int)code;

    struct imm_log_event e = {
        .fmt = fmt, .file = file, .line = line, .level = level, .code = code};
    va_start(e.va, fmt);
    log_callback(e);
    va_end(e.va);

    if (imm_unlikely(level == IMM_FATAL))
        exit(EXIT_FAILURE);

    return (int)code;
}

static void default_print(struct imm_log_event event)
{
    fprintf(stderr, "%s:%d:%s: ", event.file, event.line, __msg[event.code]);
    vfprintf(stderr, event.fmt, event.va);
    fputc('\n', stderr);
}

void __imm_bug(char const *file, int line, char const *cond)
{
    fprintf(stderr, "BUG: %s:%d: %s\n", file, line, cond);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
