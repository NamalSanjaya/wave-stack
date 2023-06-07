#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/1609_3/wave_llc.h"
#include "../../include/1609_3/wsmp.h" // circular dependency
#include "../../include/1609_3/wme.h"
#include "../../include/wave_encode.h"
#include "../../include/pdu_buf.h"
#include "../../include/fmt_error.h"
#include "../../include/network.h"
#include "../../include/1609_3/wsmp_decode.h"

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

llc_pdu_metadata *llc_decode(wave_pdu *pdu, int *err){
    llc_pdu_metadata *llc_metadata = init_llc_pdu_metadata(WAVE_LLC_ETHERTYPE_WAVE);
    if(llc_metadata == NULL) {
        //TODO: *err=?
        return NULL;
    }
    *err = 0;
    retrieve_uint8(pdu, &(llc_metadata->dsap), err);
    retrieve_uint8(pdu, &(llc_metadata->ssap), err);
    retrieve_uint8(pdu, &(llc_metadata->control), err);
    retrieve_uint8_n(pdu, 3, llc_metadata->oui, err);
    if(WAVE_LLC_OUI != ((llc_metadata->oui[0] << 16)| (llc_metadata->oui[1] << 8)| llc_metadata->oui[2])) {
        *err=WSMP_ENOSUPPORT;
        fmt_error(WAVE_WARN, "unsupported OUI field found during LLC PDU decoding.");
        return NULL;
    }
    retrieve_uint16(pdu, &(llc_metadata->ethertype), err);
    if(llc_metadata->ethertype != WAVE_LLC_ETHERTYPE_IP && llc_metadata->ethertype != WAVE_LLC_ETHERTYPE_WAVE){
        *err=WSMP_ENOSUPPORT;
        fmt_error(WAVE_WARN, "unsupported Ethertype field found during LLC PDU decoding.");
        return NULL;
    }
    return llc_metadata;
}

/* TODO: Need to decide the return type. we can input an error pointer and can check its states */
/* TODO: Power is signed integer. check it again */
/* Issued by WSMP layer to request that LSDU to be sent to Multi-Channel operation layer */
void dl_unitdatax_req(wave_pdu *pdu, uint8_t *src_addr, uint8_t *dest_addr, uint8_t prority, uint8_t chan_id, enum time_slot timeslot, uint8_t data_rate,
    uint8_t txpwr_level, uint8_t channel_load, uint64_t wsm_expire_time, int *err){
    /* most of parameters coming from WSMP layer. They have checked once. Only new parameters are src_addr, txpwr_level. */
    /* src_addr, dest_addr, prority, channel_Id etc won't use to encode at LLC. But there will be pass to MAC layer functions */
    *err = 0;
    uint16_t ethertype = WAVE_LLC_ETHERTYPE_WAVE; /* default to WSMP */
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
     fmt_info("Data encoding at LLC.");
    /* TODO: need to send pdu with relavent parameter to multi channel operational layer running in kernel space */
    // To verify wsm whole flow is working, we are configuring 1609.4 as userspace process which only use signal channal(no channel switching).
    // This is only for the testing purpose.
    // ma_unitdatax_req()
    write_tun(pdu);
    free_llc_pdu_metadata(llc_metadata); // put very end
}

// send payload to WSMP layer.
void dl_unitdata_ind(llc_pdu_metadata *llc_metadata, wave_pdu *pdu, UserAvailableServiceTable_t *uastb, int *err){
    *err=0;
    if(llc_metadata->ethertype == WAVE_LLC_ETHERTYPE_IP){
        // current tcp/ip is not supported. log a warning msg
        printf("tcp/ip is not supported...\n");
        return;
    }
    // TODO: call WSMP layer function to handover the data.
    // Decode wsm
    // WSM-WaveShortMessage.indication(...., wsm_payload,...)
    struct wsmp_wsm *wsm = wsmp_wsm_decode(pdu, err, WSMP_STRICT);
    // fmt_info("Succefully decode the data LLC");
    free_pbuf(pdu);
    wsm_waveshortmsg_ind(wsm, uastb);
}

// receive packets from multi-channel operational layer.
void dl_recv(wave_pdu *pdu, UserAvailableServiceTable_t *uastb, int *err){
    /** TODO:
     * 1. block and listen to receive data.
     * 2. decode and extract each field while validating and payload.
     * 3. take necessary action if a field has corrupted or has wrong data.
     * 4. call DL-UNITDATA.ind method to send payload to WSMP layer.
    */
    *err=0;
    // TODO: 1. block listening to receive data. update the pdu as necessary
    // to represent a frame received from multi-channel operation layer.
    llc_pdu_metadata *llc_metadata = llc_decode(pdu, err);
    if(*err){
        // log relvent error
        printf("unable to decode llc metadata \n");
        return;
    }
    dl_unitdata_ind(llc_metadata, pdu, uastb, err);
}
