#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pdu_buf.h"
#include "../include/fmt_error.h"
#include "../include/1609_3/wsmp.h"

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
    uint8_t *pdu_buf =  calloc(WSMP_MAXSIZE, 1);
    if (pdu_buf == NULL) fmt_error(WAVE_ERROR, "could not allocate memory to create pdu-buf");

    wave_pdu* pdu = malloc(sizeof(wave_pdu));
    if (pdu == NULL) fmt_error(WAVE_ERROR, "could not allocate memory to create wave pdu");

    pdu->head = pdu_buf;
    pdu->end = pdu_buf + (size_t)WSMP_MAXSIZE;
    pdu->current = pdu->end;
    pdu->offset = 0;
    return pdu;
}

/**
 * add data with given size to pdu_buf from left to right. only support 
 * uint8_t data pointers. if pdu_buf size is not enough operation will cancel.(pdu_buf size won't increase automatically)
 */
void add_data_to_pbuf(wave_pdu *pdu, uint8_t *data, size_t szof_data, int *err){
    uint8_t *st = pdu->current - szof_data;
    if (st < pdu->head){
        // pdu_buf size is not enough
        *err=WSMP_EPBUF;
        fmt_error(WAVE_WARN, "pdu_buf size is exceeded for a WSM data");
        return;
    }
    memcpy(st, data, szof_data);
    pdu->current = st;
    pdu->offset += szof_data;
}

void read_from_pbuf(wave_pdu *pdu, size_t szof_data, uint8_t *ret, int *err){
    if(pdu->current > pdu->end){
        // err = ?
        // log relavent error
        return;
    }
    memcpy(ret, pdu->current, szof_data);
    pdu->current += szof_data;
    pdu->offset -= szof_data;
}

void free_pbuf(wave_pdu *pdu){
    free(pdu->head);
    free(pdu);
}

PduTable *create_pduTable(){
    PduTable *pdu_tb =  calloc(1, sizeof(PduTable));
    if (pdu_tb == NULL) fmt_panic("unable to allocate memory to create PDU table");
    pdu_tb->wsa_store = calloc(16, sizeof(wave_pdu));
    if (pdu_tb->wsa_store  == NULL) fmt_panic("unable to allocate memory to create wave_pdu struct");
    return pdu_tb;
}

wave_pdu *get_pdu(uint8_t indx, PduTable *pdu_tb){
    return pdu_tb->wsa_store[indx];
}
