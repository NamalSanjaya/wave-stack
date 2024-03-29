#ifndef _WAVE_SOCK_H
#define _WAVE_SOCK_H

#include <stdint.h>
#include <stdbool.h>

// primitive identifiers
#define WSM_WaveShortMessage_request 1
#define WSM_WaveShortMessage_confirm 2
#define WSM_WaveShortMessage_indication 3
#define WME_ProviderService_request 11
#define WME_ProviderService_confirm 12
#define WME_UserService_request 13
#define WME_UserService_confirm 14
#define WME_WSMService_request 15
#define WME_WSMService_confirm 16
#define WME_ChannelService_request 17
#define WME_ChannelService_confirm 18
#define WME_TimingAdvertisementService_request 19
#define WME_TimingAdvertisementService_confirm 20
#define WME_Notification_indication 21
#define WME_Get_request 22
#define WME_Get_confirm 23
#define WME_Set_request 24
#define WME_Set_confirm 25
#define WME_AddressChange_request 26
#define WME_AddressChange_confirm 27
#define DL_UNITDATAX 31
#define DL_UNITDATA 32
#define MLMEX_DELETETXPROFILE 41
#define MLMEX_REGISTERTXPROFILE 42
#define MLMEX_TA 43
#define MLMEX_TAEND 44
#define MLMEX_CHSTART 45
#define MLMEX_CHEND 46
#define MLME_GET 47
#define MLME_SET 48 
#define MA_UNITDATA 51
#define MA_UNITDATAX 52

#define DATA_MAXSIZE 2304
#define WSMDATAMAXSIZE 2048       // TODO: Need to check the exact value

enum action {
    add, delete, change
};

enum wsa_type {
    secured = 1, unsecured = 2, securedOrUnsecured = 3, any = 4
};

enum channel_access {
    continuous = 0, alternatingTimeslot0Only = 1, alternatingTimeslot1Only = 2
};

enum time_slot {
    time_slot0, time_slot1
};

enum user_request_type {
    auto_access, no_SCH_access
};

//  for WME primitive
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

// for WSM primitive
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

int8_t _put_uint8(uint8_t *buf, size_t *i, const uint8_t v);
int8_t _put_uint8_n(uint8_t *buf, size_t *i, size_t n, const uint8_t *v);
int8_t _put_uint16(uint8_t *buf, size_t *i, const uint16_t v);
int8_t _put_uint32(uint8_t *buf, size_t *i, const uint32_t v);
void print_app_ProviderServiceReqEntry(app_ProviderServiceReqEntry *psre);

int send_data(uint8_t *src, size_t size, const char *sckfile);
int init_socket(const char *sckfile);

int8_t app_provider_service_req(enum action act, uint8_t *dest_mac_addr, enum wsa_type wsatype, uint32_t psid, 
    uint8_t *psc, uint8_t psc_len, enum channel_access chan_access, bool ip_service, uint8_t *ipv6_addr, uint16_t service_port, int8_t rcpi_threshold, 
    uint8_t wsa_count_threshold, uint8_t wsa_count_thd_interval, const char *sckfile);

int8_t app_wsm_waveshortmsg_req(uint8_t chan_id, enum time_slot timeslot, uint8_t data_rate, int8_t tx_power, uint8_t channel_load, uint8_t info_elem_indicator, 
    uint8_t prority, uint64_t wsm_expire_time, uint16_t len, uint8_t *data, uint8_t *peer_mac_addr, uint32_t psid, const char *sckfile);

#endif /* _WAVE_SOCK_H */
