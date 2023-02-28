#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/1609_3/wave_llc.h"
#include "../../include/1609_3/wsmp.h"
#include "../../include/wave_encode.h"
#include "../../include/pdu_buf.h"
#include "../../include/fmt_error.h"

llc_pdu_metadata *init_llc_pdu_metadata(uint16_t ethertype){
    llc_pdu_metadata *ptr = malloc(sizeof(llc_pdu_metadata));
    if(ptr == NULL) { 
        fmt_error(WAVE_ERROR,"unable to allocate memory to create LLC PDU metadata");
        return NULL;
    }
    ptr->dsap = 0xAB;
    ptr->ssap = 0xAB;
    ptr->control = 0x03;
    ptr->oui[0] = 0x00;
    ptr->oui[1] = 0x00;
    ptr->oui[2] = 0x00;
    ptr->ethertype = ethertype;
    return ptr;
}
/* debug function to print each field in llc frame */
void print_llc_pdu_metadata(const llc_pdu_metadata *self){
    printf("DSAP      : %X\n", self->dsap);
    printf("SSAP      : %X\n", self->ssap);
    printf("Control   : %x\n", self->control);
    printf("OUI-1     : %x\n", self->oui[0]);
    printf("OUI-2     : %x\n", self->oui[1]);
    printf("OUI-3     : %x\n", self->oui[2]);
    printf("EtherType : %X\n", self->ethertype);
    printf("\nllc pdu metadata printing finished.\n");
}

void free_llc_pdu_metadata(llc_pdu_metadata *self){
    free(self);
}

void llc_encode(const llc_pdu_metadata *self, wave_pdu *pdu, int *err){
    *err = 0;

    if(WAVE_LLC_OUI != ((self->oui[0] << 16)| (self->oui[1] << 8)| self->oui[2])) {
        *err=WSMP_ENOSUPPORT;
        fmt_error(WAVE_WARN, "unsupported OUI field given to encode LLC PDU.");
        return;
    }
    if(self->ethertype != WAVE_LLC_ETHERTYPE_IP && self->ethertype != WAVE_LLC_ETHERTYPE_WAVE){
        *err=WSMP_ENOSUPPORT;
        fmt_error(WAVE_WARN, "unsupported Ethertype field given to encode LLC PDU.");
        return;
    }
    // don't need to get data seperate as we use the pdu_buf to track the memory locations
    // store_uint8_n(pdu, self->len, self->data, err);   /* WSM Data field */
    store_uint16(pdu, self->ethertype, err);   /* ethertype field */
    store_uint8_n(pdu, 3, self->oui, err);     /* OUI field */
    store_uint8(pdu, WAVE_LLC_CONTROL, err);   /* Control field */
    store_uint8(pdu, WAVE_LLC_SSAP, err);      /* SSAP field */
    store_uint8(pdu, WAVE_LLC_DSAP, err);      /* DSAP field */
}

/* TODO: Need to decide the return type. we can input an error pointer and can check its states */
/* TODO: Power is signed integer. check it again */
/* Issued by WSMP layer to request that LSDU to be sent to Multi-Channel operation layer */
void dl_unitdatax_req(wave_pdu *pdu, uint8_t *src_addr, uint8_t *dest_addr, uint8_t prority, uint8_t chan_id, enum time_slot timeslot, uint8_t data_rate,
    uint8_t txpwr_level, uint8_t channel_load, uint64_t wsm_expire_time, int *err){
    /* most of parameters coming from WSMP layer. They have checked once. Only new parameters are src_addr, txpwr_level. */
    /* src_addr, dest_addr, prority, channel_Id etc won't use to encode at LLC. But there will be pass to MAC layer functions */
    *err = 0;
    uint16_t ethertype = 0x88DC; /* default to WSMP */
    llc_pdu_metadata *llc_metadata = init_llc_pdu_metadata(ethertype);
    if (llc_metadata == NULL){
        fmt_error(WAVE_WARN, "unable to send wsmp_wsm to MAC layer since failed to create LLC metadata");
        *err=WSMP_ENOMEM;
        return;
    }    
    llc_encode(llc_metadata, pdu, err);
    if(*err){
        fmt_error(WAVE_WARN, "unable to send wsmp_wsm to MAC layer since failed to encode LLC frame");
        // return;
    }
    /* TODO: need to send pdu with relavent parameter to multi channel operational layer running in kernel space */
    // To verify wsm whole flow is working, we are configuring 1609.4 as userspace process which only use signal channal(no channel switching).
    // This is only for the testing purpose.
    // ma_unitdatax_req()
    free_llc_pdu_metadata(llc_metadata); // put very end
}
