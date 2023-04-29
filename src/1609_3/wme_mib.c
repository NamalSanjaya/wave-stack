#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../include/1609_3/wme.h"

// get by Provider Table Index
ProviderServiceRequestTableEntry *get_wme_prvtb(size_t index, ProviderServiceRequestTable *self){
    if(self->size <= index){
        return NULL;
    }
    return (self->table) + index;
}

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

// Add a Service metadata to ProviderServiceRequest Table
void wme_prvtb_add(enum wsa_type  wsatype, uint32_t psid, uint8_t *psc, enum channel_access  chan_access, bool best_available, uint32_t op_class, 
    uint8_t  ps_chan_no, uint8_t  wsa_chan_no, uint8_t  repeat_rate, bool ip_service, uint8_t *ipv6_address, uint8_t *provider_mac_addr,    
    uint16_t service_port, int8_t  rcpi_threshold, uint8_t  wsa_count_threshold, uint8_t  wsa_count_threshold_intrv, enum service_status  serv_status,
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
    memcpy(entry->ProviderIpv6Address, ipv6_address, 16);
    memcpy(entry->ProviderMacAddress, provider_mac_addr, 6);
    entry->ProviderServicePort = service_port;
    entry->ProviderRcpiThreshold = rcpi_threshold;
    entry->ProviderWsaCountThreshold = wsa_count_threshold;
    entry->ProviderWsaCountThresholdInterval = wsa_count_threshold_intrv;
    entry->ProviderServiceStatus = serv_status;

    memcpy((self->table) + (self->size), entry, sizeof(ProviderServiceRequestTableEntry));
    self->size++;

    free(entry);
}

ProviderServiceRequestTable *create_wme_provider_tb(){
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

bool is_wme_prvtb_empty(ProviderServiceRequestTable *self){
    return self->size == 0;
}

uint16_t wme_prvtb_psc_len(uint8_t *psc){
    uint16_t count = 0;
    for (size_t i = 0; i < 32; i++){
        if( *(psc+i) == 0){
            return count;
        }
        count++;
    }
    return count;
}

// for debugging purpose
void show_wme_prvtb(size_t indx, ProviderServiceRequestTable *self){
    ProviderServiceRequestTableEntry entry = self->table[indx];
    printf("size: %ld\n",  self->size );
    show_wme_prvtb_entry(entry);  
}

void show_wme_prvtb_entry(ProviderServiceRequestTableEntry entry){
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
    for (size_t i = 0; i < 16; i++){
        if(entry.ProviderIpv6Address[i] == 0){
            break;
        }
        printf("%d ", entry.ProviderIpv6Address[i]);
    }

    printf("\nProviderMacAddress: ");
    for (size_t i = 0; i < 6; i++){
        printf("%d ", entry.ProviderMacAddress[i]);
    }

    printf("\nProviderServicePort: %d\n", entry.ProviderServicePort);
    printf("ProviderRcpiThreshold: %d\n", entry.ProviderRcpiThreshold);
    printf("ProviderWsaCountThreshold: %d\n", entry.ProviderWsaCountThreshold);
    printf("ProviderWsaCountThresholdInterval: %d\n", entry.ProviderWsaCountThresholdInterval);
    printf("ProviderServiceStatus: %d\n", entry.ProviderServiceStatus);
    printf(".... End ....\n");
}

// methods for  ProviderChannelInfoTable

ProviderChannelInfoTable *create_wme_prv_chan_tb(){
    ProviderChannelInfoTable *tb_obj = calloc(1, sizeof(ProviderChannelInfoTable));
    if(tb_obj == NULL){
        // should be panic
        return NULL;
    }
    uint8_t oid[32] = "1.3.111.2.1609.3.4.2.1.3" ; 
    memcpy(tb_obj->oid, oid, 32) ;
    tb_obj->size = 0;
    return tb_obj;
}

void add_wme_prv_chan_tb(uint8_t op_class, uint8_t chan_no, bool is_adaptable, uint8_t data_rate, int8_t tx_pwr_level,
    uint8_t edcaBe_CWmin, uint32_t edcaBe_CWmax, uint8_t edcaBe_Aifsn, uint32_t edcaBe_TxopLimit, bool edcaBe_Mandatory,
    uint8_t edcaBk_CWmin, uint32_t edcaBk_CWmax, uint8_t edcaBk_Aifsn, uint32_t edcaBk_TxopLimit, bool edcaBk_Mandatory,
    uint8_t edcaVi_CWmin, uint32_t edcaVi_CWmax, uint8_t edcaVi_Aifsn, uint32_t edcaVi_TxopLimit, bool edcaVi_Mandatory,
    uint8_t edcaVo_CWmin, uint32_t edcaVo_CWmax, uint8_t edcaVo_Aifsn, uint32_t edcaVo_TxopLimit, bool edcaVo_Mandatory, 
    ProviderChannelInfoTable *self){
    ProviderChannelInfoTableEntry *entry = calloc(1, sizeof(ProviderChannelInfoTableEntry));
    if(entry == NULL){
        return;
    }
    entry->ProviderChannelInfoTableIndex = self->size;
    entry->ProviderChannelInfoOperatingClass = op_class;
    entry->ProviderChannelInfoChannelNumber = chan_no;
    entry->ProviderChannelInfoAdaptable = is_adaptable;
    entry->ProviderChannelInfoDataRate = data_rate;
    entry->ProviderChannelInfoTransmitPowerLevel = tx_pwr_level;

    entry->ProviderChannelInfoEdcaBkCWmin = edcaBk_CWmin;
	entry->ProviderChannelInfoEdcaBkCWmax = edcaBk_CWmax;
	entry->ProviderChannelInfoEdcaBkAifsn = edcaBk_Aifsn;
	entry->ProviderChannelInfoEdcaBkTxopLimit = edcaBk_TxopLimit;
    entry->ProviderChannelInfoEdcaBkMandatory = edcaBk_Mandatory;

    entry->ProviderChannelInfoEdcaBeCWmin = edcaBe_CWmin;
	entry->ProviderChannelInfoEdcaBeCWmax = edcaBe_CWmax;
	entry->ProviderChannelInfoEdcaBeAifsn = edcaBe_Aifsn;
	entry->ProviderChannelInfoEdcaBeTxopLimit = edcaBe_TxopLimit;
    entry->ProviderChannelInfoEdcaBeMandatory = edcaBe_Mandatory;

    entry->ProviderChannelInfoEdcaViCWmin = edcaVi_CWmin;
	entry->ProviderChannelInfoEdcaViCWmax = edcaVi_CWmax;
	entry->ProviderChannelInfoEdcaViAifsn = edcaVi_Aifsn;
	entry->ProviderChannelInfoEdcaViTxopLimit = edcaVi_TxopLimit;
    entry->ProviderChannelInfoEdcaViMandatory = edcaVi_Mandatory;

    entry->ProviderChannelInfoEdcaVoCWmin = edcaVo_CWmin;
	entry->ProviderChannelInfoEdcaVoCWmax = edcaVo_CWmax;
	entry->ProviderChannelInfoEdcaVoAifsn = edcaVo_Aifsn;
	entry->ProviderChannelInfoEdcaVoTxopLimit = edcaVo_TxopLimit;
    entry->ProviderChannelInfoEdcaVoMandatory = edcaVo_Mandatory;

    memcpy((self->table) + (self->size), entry, sizeof(ProviderChannelInfoTableEntry));
    self->size++;

    free(entry);
}

// get Channel Info Entry by index
ProviderChannelInfoTableEntry *get_wme_prv_chan_entry(size_t index, ProviderChannelInfoTable *self){
    if(self->size <= index){
        return NULL;
    }
    return (self->table) + index;
}

// Get the `ProviderChannelInfoTableIndex`
uint8_t get_chan_index(uint8_t chan_no, ProviderChannelInfoTable *self){
    for (uint8_t i = 0; i < self->size; i++){
        if((self->table)[i].ProviderChannelInfoChannelNumber == chan_no){
            return i+1;
        }
    }
    return 0;
}

// only for debugging purposes
void show_wme_chan_info(size_t indx, ProviderChannelInfoTable *self){
    ProviderChannelInfoTableEntry entry = self->table[indx];
    printf("size: %ld\n",  self->size );
    show_wme_chan_info_entry(&entry);  
}

void show_wme_chan_info_entry(ProviderChannelInfoTableEntry *entry){
    printf("\n----- channel Info Entry ------\n");
    printf("ProviderChannelInfoTableIndex: %d\n", entry->ProviderChannelInfoTableIndex);
    printf("ProviderChannelInfoOperatingClass: %d\n", entry->ProviderChannelInfoOperatingClass);
    printf("ProviderChannelInfoChannelNumber: %d\n", entry->ProviderChannelInfoChannelNumber);
    if(entry->ProviderChannelInfoAdaptable){
        printf("ProviderChannelInfoAdaptable: true\n");
    } else {
        printf("ProviderChannelInfoAdaptable: false\n");
    }
    printf("ProviderChannelInfoDataRate: %d\n", entry->ProviderChannelInfoDataRate);
    printf("ProviderChannelInfoTransmitPowerLevel : %d\n", entry->ProviderChannelInfoTransmitPowerLevel);
    
    printf("ProviderChannelInfoEdcaBkCWmin: %d\n", entry->ProviderChannelInfoEdcaBkCWmin);
    printf("ProviderChannelInfoEdcaBkCWmax: %d\n", entry->ProviderChannelInfoEdcaBkCWmax);
    printf("ProviderChannelInfoEdcaBkAifsn: %d\n", entry->ProviderChannelInfoEdcaBkAifsn);
    printf("ProviderChannelInfoEdcaBkTxopLimit: %d\n", entry->ProviderChannelInfoEdcaBkTxopLimit);

    if (entry->ProviderChannelInfoEdcaBkMandatory){
        printf("ProviderChannelInfoEdcaBkMandatory: true\n");
    } else {
        printf("ProviderChannelInfoEdcaBkMandatory: false\n");
    }

    printf("------ End --------\n");
}
