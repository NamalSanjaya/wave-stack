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
    uint8_t  ProviderServiceChannelNumber;  // channel at which the actual data is sent
    uint8_t  ProviderWsaChannelNumber;      // channel at which WSA is sent
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

typedef struct ProviderChannelInfoTableEntry{
    uint8_t ProviderChannelInfoTableIndex;
	uint8_t ProviderChannelInfoOperatingClass;
	uint8_t ProviderChannelInfoChannelNumber;
	bool ProviderChannelInfoAdaptable;
	uint8_t ProviderChannelInfoDataRate;
	int8_t ProviderChannelInfoTransmitPowerLevel;
    
	uint8_t ProviderChannelInfoEdcaBkCWmin;
	uint32_t ProviderChannelInfoEdcaBkCWmax;
	uint8_t ProviderChannelInfoEdcaBkAifsn;
	uint32_t ProviderChannelInfoEdcaBkTxopLimit;
	bool ProviderChannelInfoEdcaBkMandatory;

	uint8_t ProviderChannelInfoEdcaBeCWmin;
	uint32_t ProviderChannelInfoEdcaBeCWmax;
	uint8_t ProviderChannelInfoEdcaBeAifsn;
	uint32_t ProviderChannelInfoEdcaBeTxopLimit;
	bool ProviderChannelInfoEdcaBeMandatory;

	uint8_t ProviderChannelInfoEdcaViCWmin;
	uint32_t ProviderChannelInfoEdcaViCWmax;
	uint8_t ProviderChannelInfoEdcaViAifsn;
	uint32_t ProviderChannelInfoEdcaViTxopLimit;
	bool ProviderChannelInfoEdcaViMandatory;

	uint8_t ProviderChannelInfoEdcaVoCWmin;
	uint32_t ProviderChannelInfoEdcaVoCWmax;
	uint8_t ProviderChannelInfoEdcaVoAifsn;
	uint32_t ProviderChannelInfoEdcaVoTxopLimit;
	bool ProviderChannelInfoEdcaVoMandatory;
} ProviderChannelInfoTableEntry;

typedef struct ProviderChannelInfoTable {
    uint8_t oid[32];
    ProviderChannelInfoTableEntry table[32];
    size_t size;
} ProviderChannelInfoTable;

ProviderServiceRequestTableEntry *get_wme_prvtb(size_t index, ProviderServiceRequestTable *self);
ProviderServiceRequestTableEntry *wme_prvtb_get_by_psid(uint32_t psid, ProviderServiceRequestTable *self);
void wme_prvtb_add(enum wsa_type  wsatype, uint32_t psid, uint8_t *psc, enum channel_access  chan_access, bool best_available, uint32_t op_class, 
    uint8_t  ps_chan_no, uint8_t  wsa_chan_no, uint8_t  repeat_rate, bool ip_service, uint8_t *ipv6_address, uint8_t *provider_mac_addr,    
    uint32_t service_port, uint8_t  rcpi_threshold, uint8_t  wsa_count_threshold, uint8_t  wsa_count_threshold_intrv, enum service_status  serv_status,
    ProviderServiceRequestTable *self);
ProviderServiceRequestTable *create_wme_provider_tb();
bool is_wme_prvtb_empty(ProviderServiceRequestTable *self);
uint16_t wme_prvtb_psc_len(uint8_t *psc);
void show_wme_prvtb(size_t indx, ProviderServiceRequestTable *self);
void show_wme_prvtb_entry(ProviderServiceRequestTableEntry entry);

ProviderChannelInfoTable *create_wme_prv_chan_tb();
void add_wme_prv_chan_tb(uint8_t op_class, uint8_t chan_no, bool is_adaptable, uint8_t data_rate, int8_t tx_pwr_level,
    uint8_t edcaBe_CWmin, uint32_t edcaBe_CWmax, uint8_t edcaBe_Aifsn, uint32_t edcaBe_TxopLimit, bool edcaBe_Mandatory,
    uint8_t edcaBk_CWmin, uint32_t edcaBk_CWmax, uint8_t edcaBk_Aifsn, uint32_t edcaBk_TxopLimit, bool edcaBk_Mandatory,
    uint8_t edcaVi_CWmin, uint32_t edcaVi_CWmax, uint8_t edcaVi_Aifsn, uint32_t edcaVi_TxopLimit, bool edcaVi_Mandatory,
    uint8_t edcaVo_CWmin, uint32_t edcaVo_CWmax, uint8_t edcaVo_Aifsn, uint32_t edcaVo_TxopLimit, bool edcaVo_Mandatory, 
    ProviderChannelInfoTable *self);

ProviderChannelInfoTableEntry *get_wme_prv_chan_entry(size_t index, ProviderChannelInfoTable *self);
void show_wme_chan_info(size_t indx, ProviderChannelInfoTable *self);
void show_wme_chan_info_entry(ProviderChannelInfoTableEntry *entry);

#endif /* _IEEE1609DOT3_MIB_H_ */
