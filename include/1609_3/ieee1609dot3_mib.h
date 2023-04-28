#ifndef _IEEE1609DOT3_MIB_H_
#define _IEEE1609DOT3_MIB_H_

#include <stdint.h>
#include <stdbool.h>

#include "wme.h"

#define WSATYPE                      2
#define PROVIDERSERVICEIDENTIFIER    3
#define PROVIDERSERVICECONTEXT       4
#define PROVIDERCHANNELACCESS        5
#define PROVIDERBESTAVAILABLE        6
#define PROVIDEROPERATINGCLASS       7
#define PROVIDERSERVICECHANNELNUMBER 8
#define PROVIDERWSACHANNELNUMBER     9
#define PROVIDERREPEATRATE           10
#define PROVIDERIPSERVICE            11
#define PROVIDERIPV6ADDRESS          12
#define PROVIDERMACADDRESS           13
#define PROVIDERSERVICEPORT          14
#define PROVIDERRCPITHRESHOLD        15
#define PROVIDERWSACOUNTTHRESHOLD    16
#define PROVIDERWSACOUNTTHRESHOLDINTERVAL 17
#define PROVIDERSERVICESTATUS        18

typedef struct ProviderServiceRequestTableEntry {
    uint32_t  ProviderServiceRequestTableIndex;
    enum wsa_type  WsaType;
    uint32_t  ProviderServiceIdentifier;
    uint8_t   ProviderServiceContext[32];
    enum channel_access  ProviderChannelAccess;
    bool ProviderBestAvailable;
    uint32_t ProviderOperatingClass;
    uint8_t  ProviderServiceChannelNumber;
    uint8_t  ProviderWsaChannelNumber;
    uint8_t  ProviderRepeatRate;
    bool ProviderIpService;
    uint8_t ProviderIpv6Address[15];
    uint8_t ProviderMacAddress[11];    
    uint32_t ProviderServicePort;
    uint8_t  ProviderRcpiThreshold;
    uint8_t  ProviderWsaCountThreshold;
    uint8_t  ProviderWsaCountThresholdInterval;
    enum service_status  ProviderServiceStatus;
} ProviderServiceRequestTableEntry;

typedef struct ProviderServiceRequestTable {
    uint8_t oid[32];
    ProviderServiceRequestTableEntry table[32];
    size_t size;
} ProviderServiceRequestTable;

ProviderServiceRequestTableEntry *wme_prvtb_get_by_psid(uint32_t psid, ProviderServiceRequestTable *self);
void wme_prvtb_add(enum wsa_type  wsatype, uint32_t psid, uint8_t *psc, enum channel_access  chan_access, bool best_available, uint32_t op_class, 
    uint8_t  ps_chan_no, uint8_t  wsa_chan_no, uint8_t  repeat_rate, bool ip_service, uint8_t *ipv6_address, uint8_t *provider_mac_addr,    
    uint32_t service_port, uint8_t  rcpi_threshold, uint8_t  wsa_count_threshold, uint8_t  wsa_count_threshold_intrv, enum service_status  serv_status,
    ProviderServiceRequestTable *self);
ProviderServiceRequestTable *create_provider_tb();
void show_tb(size_t indx, ProviderServiceRequestTable *self);
void show_tb_entry(ProviderServiceRequestTableEntry entry);

#endif /* _IEEE1609DOT3_MIB_H_ */
