#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/1609_3/wave_llc.h"
#include "../../include/wave_encode.h"
#include "../../include/pdu_buf.h"
#include "../../include/fmt_error.h"

llc_pdu_metadata *init_llc_pdu_metadata(uint16_t ethertype, uint8_t *data, uint16_t count){
    llc_pdu_metadata *ptr = malloc(sizeof(llc_pdu_metadata));
    if(ptr == NULL) fmt_panic("unable to allocate memory to create LLC PDU metadata\n");
    ptr->dsap = 0xAB;
    ptr->ssap = 0xAB;
    ptr->control = 0x03;
    ptr->oui[0] = 0x00;
    ptr->oui[1] = 0x00;
    ptr->oui[2] = 0x00;
    ptr->ethertype = ethertype;
    ptr->len  = count;
    ptr->data = calloc(ptr->len, 1);
    memcpy(ptr->data, data, count);
    return ptr;
}

void print_llc_pdu_metadata(const llc_pdu_metadata *self){
    printf("DSAP      : %X\n", self->dsap);
    printf("SSAP      : %X\n", self->ssap);
    printf("Control   : %x\n", self->control);
    printf("OUI-1     : %x\n", self->oui[0]);
    printf("OUI-2     : %x\n", self->oui[1]);
    printf("OUI-3     : %x\n", self->oui[2]);
    printf("EtherType : %X\n", self->ethertype);
    printf("len       : %d\n", self->len);
    printf("data      : ");
    for (size_t i = 0; i < self->len; i++){
        printf("%c", *(self->data + i) );
    }
    printf("\nllc pdu metadata printing finished\n");
}

void free_llc_pdu_metadata(llc_pdu_metadata *self){
    free(self);
}

void llc_encode(const llc_pdu_metadata *self, wave_pdu *pdu, int *err){
    *err = 0;

    if(WAVE_LLC_OUI != ((self->oui[0] << 16)| (self->oui[1] << 8)| self->oui[2])) {
        fmt_error(WAVE_WARN, "unsupported OUI field given to encode LLC PDU");
        return;
    }
    if(self->ethertype != WAVE_LLC_ETHERTYPE_IP && self->ethertype != WAVE_LLC_ETHERTYPE_WAVE){
        fmt_error(WAVE_WARN, "unsupported Ethertype field given to encode LLC PDU");
        return;
    }
    store_uint8_n(pdu, self->len, self->data, err);   /* WSM Data field */
    store_uint16(pdu, self->ethertype, err);   /* ethertype field */
    store_uint8_n(pdu, 3, self->oui, err);     /* OUI field */
    store_uint8(pdu, WAVE_LLC_CONTROL, err);   /* Control field */
    store_uint8(pdu, WAVE_LLC_SSAP, err);      /* SSAP field */
    store_uint8(pdu, WAVE_LLC_DSAP, err);      /* DSAP field */
}

/* Issued by WSMP layer to request that LSDU to be sent to Multi-Channel operation layer */
void dl_unitdatax_req(wave_pdu *pdu, char *src_addr, char *dest_addr, uint8_t prority, uint8_t chan_id, enum time_slot timeslot, uint8_t data_rate,
    uint8_t txpwr_level, uint8_t channel_load, uint64_t wsm_expire_time, uint8_t *data, uint16_t count){
    /* most of parameters coming from WSMP layer. They have checked once. Only new parameters are src_addr, txpwr_level. */
    /* WSMP Header + WSM data <= WSM Max Length */
    uint16_t ethertype = 0x88DC; /* default to WSMP */
    int myerr=0;
    int *err=&myerr;
    llc_pdu_metadata *llc_metadata = init_llc_pdu_metadata(ethertype, data, count);
    llc_encode(llc_metadata, pdu, err);

    /* TODO: need to send pdu with relavent parameter to multi channel operational layer running in kernel space */
    free_llc_pdu_metadata(llc_metadata); // put very end
}
