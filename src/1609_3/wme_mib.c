#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../include/1609_3/ieee1609dot3_mib.h"
#include "../../include/1609_3/wme.h"

// get by PSID
ProviderServiceRequestTableEntry *wme_prvtb_get_by_psid(uint32_t psid, ProviderServiceRequestTable *self){
    ProviderServiceRequestTableEntry *entry = calloc(1, sizeof(ProviderServiceRequestTableEntry));
    if (entry == NULL){
        return NULL;
    }
    for (size_t i = 0; i < self->size; i++){
        if((self->table)[i].ProviderServiceIdentifier == psid){
            memcpy(entry, (self->table)+i, sizeof(ProviderServiceRequestTableEntry));
            return entry;
        }
    }
    return NULL;
}

void wme_prvtb_add(enum wsa_type  wsatype, uint32_t psid, uint8_t *psc, enum channel_access  chan_access, bool best_available, uint32_t op_class, 
    uint8_t  ps_chan_no, uint8_t  wsa_chan_no, uint8_t  repeat_rate, bool ip_service, uint8_t *ipv6_address, uint8_t *provider_mac_addr,    
    uint32_t service_port, uint8_t  rcpi_threshold, uint8_t  wsa_count_threshold, uint8_t  wsa_count_threshold_intrv, enum service_status  serv_status,
    ProviderServiceRequestTable *self){
    ProviderServiceRequestTableEntry *entry = calloc(1, sizeof(ProviderServiceRequestTableEntry));
    if(entry == NULL){
        return;
    }
    entry->ProviderServiceRequestTableIndex = self->size;
    entry->WsaType = wsatype;
    entry->ProviderServiceIdentifier = psid;
    memcpy(entry->ProviderServiceContext, psc, 32);
    entry->ProviderChannelAccess = chan_access;
    entry->ProviderBestAvailable = best_available;
    entry->ProviderOperatingClass = op_class;
    entry->ProviderServiceChannelNumber = ps_chan_no;
    entry->ProviderWsaChannelNumber = wsa_chan_no;
    entry->ProviderRepeatRate = repeat_rate;
    entry->ProviderIpService = ip_service;
    memcpy(entry->ProviderIpv6Address, ipv6_address, 15);
    memcpy(entry->ProviderMacAddress, provider_mac_addr, 11);
    entry->ProviderServicePort = service_port;
    entry->ProviderRcpiThreshold = rcpi_threshold;
    entry->ProviderWsaCountThreshold = wsa_count_threshold;
    entry->ProviderWsaCountThresholdInterval = wsa_count_threshold_intrv;
    entry->ProviderServiceStatus = serv_status;

    memcpy((self->table) + (self->size), entry, sizeof(ProviderServiceRequestTableEntry));
    self->size++;

    free(entry);
}

ProviderServiceRequestTable *create_provider_tb(){
    ProviderServiceRequestTable *tb_obj = calloc(1, sizeof(ProviderServiceRequestTable));
    if(tb_obj == NULL){
        // should be panic
        return NULL;
    }
    uint8_t oid[32] = "1.3.111.2.1609.3.4.2.1.4" ; 
    memcpy(tb_obj->oid, oid, 32) ;
    tb_obj->size = 0;
    return tb_obj;
}

void show_tb(size_t indx, ProviderServiceRequestTable *self){
    ProviderServiceRequestTableEntry entry = self->table[indx];
    printf("size: %ld\n",  self->size );
    show_tb_entry(entry);
    
}

void show_tb_entry(ProviderServiceRequestTableEntry entry){
printf(".... Table Entry Info ....\n");
    printf("ProviderServiceRequestTableIndex: %d\n", entry.ProviderServiceRequestTableIndex);
    printf("WsaType: %d\n", entry.WsaType);
    printf("ProviderServiceIdentifier: %d\n", entry.ProviderServiceIdentifier);
    printf("ProviderServiceContext: ");
    for (size_t i = 0; i < 32; i++){
        if(entry.ProviderServiceContext[i] == 0){
            break;
        }
        printf("%d ", entry.ProviderServiceContext[i]);
    }
    printf("\nProviderChannelAccess: %d\n", entry.ProviderChannelAccess);
    if (entry.ProviderBestAvailable){
        printf("ProviderBestAvailable: %s\n", "true");
    } else {
        printf("ProviderBestAvailable: %s\n", "false");
    }

    printf("ProviderOperatingClass: %d\n", entry.ProviderOperatingClass);
    printf("ProviderServiceChannelNumber: %d\n", entry.ProviderServiceChannelNumber);
    printf("ProviderWsaChannelNumber: %d\n", entry.ProviderWsaChannelNumber);
    printf("ProviderRepeatRate: %d\n", entry.ProviderRepeatRate);

    if (entry.ProviderIpService){
        printf("ProviderIpService: %s\n", "true");
    } else {
        printf("ProviderIpService: %s\n", "false");
    }

    printf("ProviderIpv6Address: ");
    for (size_t i = 0; i < 15; i++){
        if(entry.ProviderIpv6Address[i] == 0){
            break;
        }
        printf("%d ", entry.ProviderIpv6Address[i]);
    }

    printf("\nProviderMacAddress: ");
    for (size_t i = 0; i < 11; i++){
        printf("%d ", entry.ProviderMacAddress[i]);
    }

    printf("\nProviderServicePort: %d\n", entry.ProviderServicePort);
    printf("ProviderRcpiThreshold: %d\n", entry.ProviderRcpiThreshold);
    printf("ProviderWsaCountThreshold: %d\n", entry.ProviderWsaCountThreshold);
    printf("ProviderWsaCountThresholdInterval: %d\n", entry.ProviderWsaCountThresholdInterval);
    printf("ProviderServiceStatus: %d\n", entry.ProviderServiceStatus);
    printf(".... End ....\n");
}