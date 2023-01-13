#ifndef _WAVE_LLC_H
#define _WAVE_LLC_H

#include <stdint.h>
#include <stddef.h>

#define MSDU_MAXSIZE 2304  /* in octets */

#define WAVE_LLC_DSAP 0xAA
#define WAVE_LLC_SSAP 0xAA
#define WAVE_LLC_CONTROL 0x03
#define WAVE_LLC_OUI 0x000000
#define WAVE_LLC_ETHERTYPE_IP 0x86DD
#define WAVE_LLC_ETHERTYPE_WAVE 0x88DC

/* This will be used to create LLC PDU(MSDU) which is inserted into data field of MA-UNITDATAX.request' */
typedef struct wave_llc_pdu_metadata {
    uint8_t dsap;
    uint8_t ssap;
    uint8_t control;
    uint8_t oui[3];
    uint16_t ethertype;
    uint16_t len;
    uint8_t *data;
} wave_llc_pdu_metadata;

wave_llc_pdu_metadata *wave_init_llc_pdu_metadata(uint16_t ethertype);
void wave_print_llc_pdu_metadata(const wave_llc_pdu_metadata *self);
uint8_t *wave_llc_encode(const wave_llc_pdu_metadata *self, size_t *cnt, int *err);
void wave_free_llc_pdu_metadata(wave_llc_pdu_metadata *self);

#endif  /* _WAVE_LLC_H */
