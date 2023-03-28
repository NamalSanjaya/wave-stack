#include <stdio.h>
#include <stdint.h>
#include "../include/wave_encode.h"
#include "../include/pdu_buf.h"
#include "../include/fmt_error.h"

void store_uint8(wave_pdu *pdu, const uint8_t v, int *err){
    if (*err) {
        fmt_error(WAVE_WARN, "failed to store data in pdu_buf");
	    return;
    }
    uint8_t temp = v;
    add_data_to_pbuf(pdu, &temp, 1, err);
}

void store_uint16(wave_pdu *pdu, const uint16_t v, int *err){
    store_uint8(pdu, v, err);
    store_uint8(pdu, v >> 8, err);
}

void store_uint8_n(wave_pdu *pdu, size_t n, const uint8_t *v, int *err){
    for (int j = n-1; j >= 0; j--)
	    store_uint8(pdu, v[j], err);
}

void retrieve_uint8(wave_pdu *pdu, uint8_t *ret, int *err){
    if (*err) {
        fmt_error(WAVE_WARN, "failed to retrieve data in pdu_buf");
	    return;
    }
    read_from_pbuf(pdu, 1, ret, err);
}

void retrieve_uint16(wave_pdu *pdu, uint16_t *ret, int *err){
    if (*err) {
        fmt_error(WAVE_WARN, "failed to retrieve data in pdu_buf");
	    return;
    }
    uint8_t temp[2];
    retrieve_uint8_n(pdu, 2, temp, err);
    *ret = (temp[0] << 8) | temp[1] ; 
}

void retrieve_uint8_n(wave_pdu *pdu, size_t n, uint8_t *ret, int *err){
    for(uint8_t j=0; j < n; j++)
        retrieve_uint8(pdu, ret+j, err);
}
