#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/fmt_error.h"


// TODO: How to carry addtional data like err_code etc.
void fmt_error(char *level, char *reason) {
    fprintf(stderr, "%s : %s\n", level, reason);
}

void fmt_panic(char *reason) {
    fmt_error(WAVE_PANIC, reason);
    exit(EXIT_FAILURE);
}

void fmt_info(char *reason){
    fprintf(stdout, "%s : %s\n", WAVE_INFO, reason);
}