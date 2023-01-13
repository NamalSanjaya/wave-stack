#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/1609_3/llc/llc_core.h"
#include "../../../include/wave_encode.h"

wave_llc_pdu_metadata *wave_init_llc_pdu_metadata(uint16_t ethertype){
    wave_llc_pdu_metadata *ptr = malloc(sizeof(wave_llc_pdu_metadata));
    if(ptr == NULL){
        fprintf(stderr, "unable to allocate memory to create LLC PDU metadata\n");
        exit(EXIT_FAILURE);
    }
    ptr->dsap = 0xAB;
    ptr->ssap = 0xAB;
    ptr->control=0x03;
    ptr->oui[0]= 0x00;
    ptr->oui[1]= 0x00;
    ptr->oui[2]= 0x00;
    ptr->ethertype = ethertype;

    char llc_sdu[] = "sample-hello-world";
    ptr->len  = strlen(llc_sdu);
    ptr->data = calloc(ptr->len, 1);
    memcpy(ptr->data, llc_sdu, ptr->len);

    return ptr;
}

void wave_print_llc_pdu_metadata(const wave_llc_pdu_metadata *self){
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

void wave_free_llc_pdu_metadata(wave_llc_pdu_metadata *self){
    free(self);
}

uint8_t *wave_llc_encode(const wave_llc_pdu_metadata *self, size_t *cnt, int *err){
    uint8_t buf[MSDU_MAXSIZE];
    uint8_t *ret;
    size_t i[1];

    *i = 0;
    *cnt = 0;
    *err = 0;

    if( WAVE_LLC_OUI != ((self->oui[0] << 16)| (self->oui[1] << 8)| self->oui[2])) {
        fprintf(stderr, "--error--");
        goto out;
    }
    if(self->ethertype != WAVE_LLC_ETHERTYPE_IP && self->ethertype != WAVE_LLC_ETHERTYPE_WAVE){
        fprintf(stderr ,"----error---");
        goto out;
    }
    wave_store_uint8(buf, i, WAVE_LLC_DSAP, MSDU_MAXSIZE, err);       /* DSAP field */
    wave_store_uint8(buf, i, WAVE_LLC_SSAP, MSDU_MAXSIZE, err);      /* SSAP field */
    wave_store_uint8(buf, i, WAVE_LLC_CONTROL, MSDU_MAXSIZE, err);   /* Control field */
    wave_store_uint8_n(buf, i, 3, self->oui, MSDU_MAXSIZE, err);     /* OUI field*/
    wave_store_uint16(buf, i, self->ethertype, MSDU_MAXSIZE, err);   /* ethertype field*/
    wave_store_uint8_n(buf, i, self->len, self->data, MSDU_MAXSIZE, err);   /* WSM Data field*/
out:
    ret = calloc(*i, 1);
    memcpy(ret, buf, *i);
    *cnt = *i;

    printf("\nencoding finished\n");
    return ret;
}
