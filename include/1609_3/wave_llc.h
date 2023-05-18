#ifndef _WAVE_LLC_H
#define _WAVE_LLC_H

#include <stdint.h>
#include "../pdu_buf.h"

#include "wsmp.h"

/* TODO: WsmMaxLength and this variable is similary which is maintain under MIB */
#define MSDU_MAXSIZE 2304  /* in octets */

#define WAVE_LLC_DSAP 0xAA
#define WAVE_LLC_SSAP 0xAA
#define WAVE_LLC_CONTROL 0x03
#define WAVE_LLC_OUI 0x000000
#define WAVE_LLC_ETHERTYPE_IP 0x86DD
#define WAVE_LLC_ETHERTYPE_WAVE 0x88DC

/* This will be used to create LLC PDU(MSDU) which is inserted into data field of MA-UNITDATAX.request */
typedef struct llc_pdu_metadata {
    uint8_t dsap;
    uint8_t ssap;
    uint8_t control;
    uint8_t oui[3];
    uint16_t ethertype;
} llc_pdu_metadata;

llc_pdu_metadata *init_llc_pdu_metadata(uint16_t ethertype);
void print_llc_pdu_metadata(const llc_pdu_metadata *self);
void llc_encode(const llc_pdu_metadata *self, wave_pdu *pdu, int *err);
llc_pdu_metadata *llc_decode(wave_pdu *pdu, int *err);
void free_llc_pdu_metadata(llc_pdu_metadata *self);
void dl_unitdatax_req(wave_pdu *pdu, uint8_t *src_addr, uint8_t *dest_addr, uint8_t prority, uint8_t chan_id, 
    enum time_slot timeslot, uint8_t data_rate, uint8_t txpwr_level, uint8_t channel_load, uint64_t wsm_expire_time, int *err);
void dl_unitdata_ind(llc_pdu_metadata *llc_metadata, wave_pdu *pdu, int *err);
void dl_recv(wave_pdu *pdu, int *err);

#endif  /* _WAVE_LLC_H */
