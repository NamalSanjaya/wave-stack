#include <stdio.h>
#include <stdint.h>
#include "../include/wave_encode.h"

void wave_store_uint8(uint8_t *buf, size_t *i, const uint8_t v, size_t mxlen, int *err){
    if (*err) {
	    return;
    } if (*i >= mxlen) {
	    *err = 2;
	    return;
    }

    buf[*i] = v;
    (*i)++;
}

void wave_store_uint16(uint8_t *buf, size_t *i, const uint16_t v, size_t mxlen, int *err){
    wave_store_uint8(buf, i, v >> 8, mxlen, err);
    wave_store_uint8(buf, i, v, mxlen, err);
}

void wave_store_uint8_n(uint8_t *buf, size_t *i, size_t n, const uint8_t *v, size_t mxlen, int *err){
    for (int j = 0; j < n; j++)
	    wave_store_uint8(buf, i, v[j], mxlen, err);
}
