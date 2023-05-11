#ifndef _PDU_BUF_H
#define _PDU_BUF_H

#include <stdint.h>
#include <stddef.h>

/* data structure to represent pdu_buf. data is stored in a memory area called pdu_buf. */
typedef struct wave_pdu {
    uint8_t *head;
    uint8_t *current;
    uint8_t *end;
    size_t offset; // offset from lsb
} wave_pdu;

// To store WSA msgs
typedef struct PduTable{
    wave_pdu **wsa_store;
} PduTable;

wave_pdu *create_pdu_buf();
PduTable *create_pduTable();
void add_data_to_pbuf(wave_pdu *pdu, uint8_t *data, size_t szof_data, int *err);
void show_pdu(wave_pdu *self);
void free_pbuf(wave_pdu *pdu);
void read_from_pbuf(wave_pdu *pdu, size_t szof_data, uint8_t *ret, int *err);
wave_pdu *get_pdu(uint8_t indx, PduTable *pdu_tb);

#endif /* _PDU_BUF_H */
