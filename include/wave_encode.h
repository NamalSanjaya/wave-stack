#ifndef _WAVE_ENCODE_H
#define _WAVE_ENCODE_H

#include <stdio.h>
#include <stdint.h>
#include "./pdu_buf.h"

void store_uint8(wave_pdu *pdu, const uint8_t v, int *err);
void store_uint16(wave_pdu *pdu, const uint16_t v, int *err);
void store_uint8_n(wave_pdu *pdu, size_t n, const uint8_t *v, int *err);
void retrieve_uint8(wave_pdu *pdu, uint8_t *ret, int *err);
void retrieve_uint16(wave_pdu *pdu, uint16_t *ret, int *err);
void retrieve_uint8_n(wave_pdu *pdu, size_t n, uint8_t *ret, int *err);

#endif  /* _WAVE_ENCODE_H */
