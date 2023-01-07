#ifndef _WAVE_ENCODE_H
#define _WAVE_ENCODE_H

#include <stdio.h>
#include <stdint.h>

void wave_store_uint8(uint8_t *buf, size_t *i, const uint8_t v, size_t mxlen, int *err);
void wave_store_uint16(uint8_t *buf, size_t *i, const uint16_t v, size_t mxlen, int *err);
void wave_store_uint8_n(uint8_t *buf, size_t *i, size_t n, const uint8_t *v, size_t mxlen, int *err);

#endif  /* _WAVE_ENCODE_H */
