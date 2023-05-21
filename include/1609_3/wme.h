#ifndef _WME_H
#define _WME_H

#include <stdint.h>
#include <stdbool.h>

#include "../pdu_buf.h"

#define DEFAULT_CCH 178
// TODO: Remove this when we support multiple operating classess.
#define OPERATING_CLASS 14
#define MAXWSMREQS 32
#define WSMDATAMAXSIZE 2048  

// WME MIB tables - All sizes are in bytes
#define MACADDRSIZE 6 
#define PSIDSIZE 8
#define IPADDRSIZE 16
#define PSCSIZE 32

#define MAXAVAILABLESERVICES 4096

// To check the validity of channel number
#define IS_NOT_VALID_CHANNEL(elem) \
    ({ \
        uint8_t arr[7] = {172, 174, 176, 178, 180, 182, 184}; \
        bool not_found = true; \
        for (int i = 0; i < 7; i++) { \
            if (arr[i] == elem) { \
                not_found = false; \
                break; \
            } \
        } \
        not_found; \
})

/* Time Slot */
enum time_slot {
    time_slot0, time_slot1
};

enum action {
    add, delete, change
};

enum wsa_type {
    secured = 1, unsecured = 2, securedOrUnsecured = 3, any = 4
};

enum channel_access {
    continuous = 0, alternatingTimeslot0Only = 1, alternatingTimeslot1Only = 2
};

enum service_status{
    pending = 0, satisfied = 1, partiallySatisfied = 2
};

enum user_request_type {
    auto_access, no_SCH_access
};

enum wme_service_confirm {
    Accepted, RejectedInvalidParameter, RejectedUnspecified
};

// TODO: List need to be completed
enum security_result_code{
    success = 1, inconsistentInputParameters, spduParsingInvalidInput
};

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
    uint8_t ProviderIpv6Address[16];
    uint8_t ProviderMacAddress[6];    
    uint16_t ProviderServicePort;
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

typedef struct UserServiceRequestTableEntry {
    uint16_t UserServiceRequestTableIndex;
    enum user_request_type  UserServiceRequestType;
    uint8_t UserServiceRequestProviderServiceIdentifier[8];
    uint8_t UserServiceRequestProviderServiceContext[32];
    uint8_t UserServiceRequestPriority;
    enum wsa_type UserServiceRequestWsaTypes;
    uint8_t UserServiceRequestSourceMacAddress[6];
    uint8_t UserServiceRequestAdvertiserIdentifier[32];
    uint8_t UserServiceRequestOperatingClass;
    uint8_t UserServiceRequestChannelNumber;
    uint8_t UserServiceRequestLinkQuality; // data size for link quality need to be decided
    uint8_t UserServiceRequestImmediateAccess;
    enum service_status UserServiceStatus;
} UserServiceRequestTableEntry;

typedef struct UserServiceRequestTable {
    uint8_t oid[32];
    UserServiceRequestTableEntry table[4096];
    size_t size;
} UserServiceRequestTable;

typedef struct UserAvailableServiceTableEntry{
    uint16_t UserAvailableServiceTableIndex;
    enum wsa_type UserAvailableWsaType;
    enum security_result_code UserAvailableSecurityResultCode;

    uint8_t UserAvailableGenerationTime[8];
    uint8_t UserAvailableLifetime[8];
    uint8_t UserAvailableEarliestNextCrlTime[8];
    uint8_t UserAvailableSourceMacAddress[MACADDRSIZE];
    uint32_t UserAvailableProviderServiceIdentifier;
    uint8_t UserAvailableProviderServiceContext[PSCSIZE];
    uint8_t UserAvailableIpv6Address[IPADDRSIZE];
    uint16_t UserAvailableServicePort;
    uint8_t UserAvailableProviderMacAddress[MACADDRSIZE];
    uint8_t UserAvailableRcpiThreshold;
    uint8_t UserAvailableRcpi;
    uint8_t UserAvailableWsaCountThreshold;
    uint8_t UserAvailableOperatingClass;
    uint8_t UserAvailableChannelNumber;
    bool UserAvailableAdaptable;
    uint8_t UserAvailableDataRate;
    int8_t UserAvailableTransmitPowerLevel;
    enum channel_access UserAvailableChannelAccess;
    uint8_t UserAvailableAdvertiserIdentifier[32];
    int32_t UserAvailableTxLatitude;
    int32_t UserAvailableTxLongitude;
    uint16_t UserAvailableTxElevation;   // This is differnet from what is in the 1609.3 Std
    uint8_t UserAvailableLinkQuality;
    enum service_status UserAvailableServiceStatus;

    uint8_t UserAvailableEdcaBkCWmin;
    uint16_t UserAvailableEdcaBkCWmax;
    uint8_t UserAvailableEdcaBkAifsn;
    uint16_t UserAvailableEdcaBkTxopLimit;
    bool UserAvailableEdcaBkMandatory;

    uint8_t UserAvailableEdcaBeCWmin;
    uint16_t UserAvailableEdcaBeCWmax;
    uint8_t UserAvailableEdcaBeAifsn;
    uint16_t UserAvailableEdcaBeTxopLimit;
    bool UserAvailableEdcaBeMandatory;

    uint8_t UserAvailableEdcaViCWmin;
    uint16_t UserAvailableEdcaViCWmax;
    uint8_t UserAvailableEdcaViAifsn;
    uint16_t UserAvailableEdcaViTxopLimit;
    bool UserAvailableEdcaViMandatory;

    uint8_t UserAvailableEdcaVoCWmin;
    uint16_t UserAvailableEdcaVoCWmax;
    uint8_t UserAvailableEdcaVoAifsn;
    uint16_t UserAvailableEdcaVoTxopLimit;
    bool UserAvailableEdcaVoMandatory;

} UserAvailableServiceTableEntry_t;

typedef struct UserAvailableServiceTable {
    uint8_t oid[32];
    UserAvailableServiceTableEntry_t table[MAXAVAILABLESERVICES];
    size_t size;
} UserAvailableServiceTable_t;

typedef struct WSM_Req {
    uint8_t chan_id; 
    enum time_slot timeslot; 
    uint8_t data_rate; 
    int8_t tx_power; 
    uint8_t channel_load; 
    uint8_t info_elem_indicator; 
    uint8_t prority; 
    uint64_t wsm_expire_time; 
    uint16_t len; 
    uint8_t data[WSMDATAMAXSIZE]; 
    uint8_t peer_mac_addr[6]; 
    uint32_t psid;
} WSM_Req_t;

typedef struct WSM_ReqTable{
    WSM_Req_t table[MAXWSMREQS];  // Able to store 32 WSM requests data
    size_t cur_index;      // next index to serve
    size_t filled_index;  // should init to -1;
    size_t size;          // how many WSM requests are at a time
} WSM_ReqTable_t;

typedef struct mib {
    PduTable *pdutb;
    ProviderServiceRequestTable *psrtb;
    ProviderChannelInfoTable *pcitb;
    UserServiceRequestTable *usrtb;
    UserAvailableServiceTable_t *uastb;
    WSM_ReqTable_t *wrtb;
} mib_t;

// from libwave_sock.h
typedef struct app_ProviderServiceReqEntry{
    uint8_t id;
    enum action act; 
    uint8_t dest_mac_addr[6]; 
    enum wsa_type wsatype; 
    uint32_t psid; 
    uint8_t psc[32];
    uint8_t psc_len; 
    enum channel_access chan_access; 
    bool ip_service; 
    uint8_t ipv6_addr[16]; 
    uint16_t service_port; 
    int8_t rcpi_threshold; 
    uint8_t wsa_count_threshold;
    uint8_t wsa_count_thd_interval;
} app_ProviderServiceReqEntry;

typedef struct app_UserServiceReqEntry{
    enum action act;
    enum user_request_type user_req_type;
    uint32_t psid; 
    enum wsa_type wsatype; 
    uint8_t psc[32];
    uint8_t  channel_id;
    uint8_t src_mac_addr[6]; 
    uint8_t advertiser_id[32];
    uint8_t link_quality;
    uint8_t immediate_access;
} app_UserServiceReqEntry_t;

// use in libWave_sock.h library
typedef struct app_WSM_Req {
    uint8_t chan_id; 
    enum time_slot timeslot; 
    uint8_t data_rate; 
    int8_t tx_power; 
    uint8_t channel_load; 
    uint8_t info_elem_indicator; 
    uint8_t prority; 
    uint64_t wsm_expire_time; 
    uint16_t len; 
    uint8_t data[WSMDATAMAXSIZE]; 
    uint8_t peer_mac_addr[6]; 
    uint32_t psid;
} app_WSM_Req_t;

typedef struct local_req{
    int id;
    app_ProviderServiceReqEntry psre;
    app_WSM_Req_t wsmr;
    app_UserServiceReqEntry_t usre;
    // Add other app_<reqEntryTypes>

} local_req_t;

mib_t *create_mib();

// WME MIB related methods
ProviderServiceRequestTableEntry *get_wme_prvtb(size_t index, ProviderServiceRequestTable *self);
ProviderServiceRequestTableEntry *wme_prvtb_get_by_psid(uint32_t psid, ProviderServiceRequestTable *self);
void wme_prvtb_add(enum wsa_type  wsatype, uint32_t psid, uint8_t *psc, enum channel_access  chan_access, bool best_available, uint32_t op_class, 
    uint8_t  ps_chan_no, uint8_t  wsa_chan_no, uint8_t  repeat_rate, bool ip_service, uint8_t *ipv6_address, uint8_t *provider_mac_addr,    
    uint16_t service_port, int8_t  rcpi_threshold, uint8_t  wsa_count_threshold, uint8_t  wsa_count_threshold_intrv, enum service_status  serv_status,
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
uint8_t get_chan_index(uint8_t chan_no, ProviderChannelInfoTable *self);
void show_wme_chan_info(size_t indx, ProviderChannelInfoTable *self);
void show_wme_chan_info_entry(ProviderChannelInfoTableEntry *entry);

UserServiceRequestTable *create_wme_user_serv_req_tb();
void add_wme_user_serv_req_tb(enum user_request_type  req_type, uint8_t *psid, uint8_t *psc, uint8_t priority, enum wsa_type wsatype,
    uint8_t *src_mac_addr, uint8_t *advert_id, uint8_t op_class, uint8_t channel_no, uint8_t link_quality, uint8_t immediate_access, 
    enum service_status serv_status, UserServiceRequestTable *self);

// methods for UserAvailableService Table
UserAvailableServiceTable_t *create_wme_available_service_tb();
void add_wme_available_service(enum wsa_type wsatype, enum security_result_code sec_result_code, uint8_t *gen_time, uint8_t *lifetime, enum service_status service_status,
    uint8_t *earliest_nxt_Crl_time, uint8_t *src_mac_addr, uint32_t psid, uint16_t psc_len, uint8_t *psc, uint8_t *ipv6_addr, uint16_t port, uint8_t *provider_mac_addr,
    uint8_t rcpi_threshold, uint8_t rcpi, uint8_t wsa_count_threshold, uint8_t op_class, uint8_t channel_number, bool adaptable, uint8_t data_rate,
    int8_t tx_pwr_level, enum channel_access channel_access, uint16_t advert_len, uint8_t *advert_id, int32_t tx_lat, int32_t tx_long, uint16_t tx_elev, uint8_t link_quality,
    uint8_t edcaBkCWmin, uint16_t edcaBkCWmax, uint8_t edcaBkAifsn, uint16_t edcaBkTxopLimit, bool edcaBkMandatory,
    uint8_t edcaBeCWmin, uint16_t edcaBeCWmax, uint8_t edcaBeAifsn, uint16_t edcaBeTxopLimit, bool edcaBeMandatory,
    uint8_t edcaViCWmin, uint16_t edcaViCWmax, uint8_t edcaViAifsn, uint16_t edcaViTxopLimit, bool edcaViMandatory,
    uint8_t edcaVoCWmin, uint16_t edcaVoCWmax, uint8_t edcaVoAifsn, uint16_t edcaVoTxopLimit, bool edcaVoMandatory, UserAvailableServiceTable_t *self);

// WME related methods
struct wsmp_iex *create_wsa_iex(uint8_t repeat_rate, bool use_loc2d,  uint32_t loc2d_latitude, uint32_t loc2d_longitude, bool use_loc3d,
    uint32_t loc3d_latitude, uint32_t loc3d_longitude, uint32_t loc3d_elevation, uint16_t advert_len, uint8_t *advert_id);

struct wsmp_iex *create_sii_iex(uint16_t psc_len, uint8_t *psc, uint8_t *ipaddr, uint16_t port, uint8_t *provider_mac, bool use_rcpi_thres,
    int8_t rcpi_thres, bool use_count_thres, uint8_t count_thres, bool use_count_thres_interv, uint8_t count_thres_interv);

struct wsmp_iex *create_cii_iex(bool use_edca, uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, uint8_t chan_access);

struct wsmp_sii *create_wsa_sii(uint32_t psid, uint8_t chan_index, uint16_t psc_len, uint8_t *psc, uint8_t *ipaddr, uint16_t port, uint8_t *provider_mac, 
    bool use_rcpi_thres, int8_t rcpi_thres, bool use_count_thres, uint8_t count_thres, bool use_count_thres_interv, uint8_t count_thres_interv);

struct wsmp_cii *create_wsa_cii(uint8_t op_class, uint8_t chan_no, int8_t tx_pow, uint8_t adapt, uint8_t data_rate, bool use_edac,
    uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, uint8_t chan_access);

struct wsmp_wsa *create_wsa_metadata(uint8_t wsa_id, ProviderServiceRequestTable *provider_serv_tb, ProviderChannelInfoTable *provider_chan_tb);

enum wme_service_confirm wme_provider_service_req(uint16_t local_service_index, enum action act, uint8_t *dest_mac_addr, enum wsa_type wsatype,
    uint32_t psid, uint8_t *psc, uint8_t sch_id, uint8_t wsa_chan_id, enum time_slot chan_access, uint8_t repeat_rate, 
    bool ip_service, uint8_t *ipv6_addr, uint16_t service_port, uint8_t *provider_mac_addr, int8_t rcpi_threshold, 
    uint8_t wsa_count_threshold, uint8_t wsa_count_thd_interval, uint8_t info_elements_indicator, uint16_t sign_lifetime, mib_t *mib_db);

uint8_t find_suitable_channel();

enum wme_service_confirm wme_user_service_req(uint8_t local_service_indx, enum action act, enum user_request_type user_req_type, uint8_t *psid, uint8_t *psc,
    enum wsa_type wsatype, uint8_t channel_id, uint8_t *src_mac_addr, uint8_t *advert_id, uint8_t link_quality, uint8_t immediate_access,
    UserServiceRequestTable *user_serv_tb);

WSM_ReqTable_t *create_wsm_req_tb();
void add_wsm_req_tb(uint8_t chan_id, enum time_slot timeslot, uint8_t data_rate, int8_t tx_power, uint8_t channel_load, uint8_t info_elem_indicator, 
    uint8_t prority, uint64_t wsm_expire_time, uint16_t len, uint8_t *data, uint8_t *peer_mac_addr, uint32_t psid, WSM_ReqTable_t *self);
WSM_Req_t get_nxt_wsm_req(WSM_ReqTable_t *self, int *err);

#endif /* _WME_H */
