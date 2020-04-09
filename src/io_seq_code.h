#ifndef IO_SEQ_CODE_H
#define IO_SEQ_CODE_H

#include <stdio.h>

struct seq_code;

int io_seq_code_write(FILE* stream, struct seq_code const* seq_code);

#endif
