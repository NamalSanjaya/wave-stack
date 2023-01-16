#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pdu_buf.h"
#include "../include/wave.h"
#include "../include/fmt_error.h"

void show_pdu(wave_pdu *self){
    printf("head: %p\n", self->head);
    printf("end : %p\n", self->end);
    printf("current: %p\n", self->current);
    printf("offset : %p, value: %ld\n", &(self->offset), self->offset );
}

/**
 * create pdu_buf and return data structure called wave_pdu to handle pdu_buf.
 * pdu.current - upto where data is filled.
 * pdu.offset - offset from pdu.current to pdu.end in octets.
 */
wave_pdu *create_pdu_buf(){
    uint8_t *pdu_buf =  calloc(MSDU_MAXSIZE, 1);
    if (pdu_buf == NULL) fmt_error(WAVE_ERROR, "could not allocate memory to create pdu-buf");

    wave_pdu* pdu = malloc(sizeof(wave_pdu));
    if (pdu == NULL) fmt_error(WAVE_ERROR, "could not allocate memory to create wave pdu");

    pdu->head = pdu_buf;
    pdu->end = pdu_buf + (size_t)MSDU_MAXSIZE;
    pdu->current = pdu->end;
    pdu->offset = 0;
    return pdu;
}

/**
 * add data with given size to pdu_buf from left to right. only support 
 * uint8_t data pointers. if pdu_buf size is not enough operation will cancel.(pdu_buf size won't increase automatically)
 */
void add_data_to_pbuf(wave_pdu *pdu, uint8_t *data, size_t szof_data){
    uint8_t *st = pdu->current - szof_data;
    if (st < pdu->head){
        /* pdu_buf size is not enough */
        fmt_error(WAVE_WARN, "pdu_buf size is exceeded");
        return;
    }
    memcpy(st, data, szof_data);
    pdu->current = st;
    pdu->offset += szof_data;
}

void free_pbuf(wave_pdu *pdu){
    free(pdu->head);
    free(pdu);
}
