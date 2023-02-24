#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../include/fmt_error.h"

extern int errno;

// TODO: How to carry addtional data like err_code etc.
void fmt_error(char *level, char *reason) {
    fprintf(stderr, "%s : %s, %s\n", level, reason, strerror(errno));
}

void fmt_panic(char *reason) {
    fmt_error(WAVE_PANIC, reason);
    exit(EXIT_FAILURE);
}
