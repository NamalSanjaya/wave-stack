#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/1609_3/wme.h"
#include "../../include/pdu_buf.h"
#include "../../include/1609_3/wsmp_encode.h"
#include "../../include/1609_3/wsmp_decode.h"
#include "../include/test_wsmp.h"

ProviderServiceRequestTable *exec_test_provider_tb();
ProviderChannelInfoTable *exec_test_channelInfo_tb();
void exec_test_wsa();
void test_provider_service_req();

ProviderServiceRequestTable *exec_test_provider_tb(){
    // printf("\n----- Provider Service Request Table Info -----\n");
    ProviderServiceRequestTable *db = create_wme_provider_tb();

    // data set 1
    enum wsa_type  wsatype = unsecured;
    uint32_t psid = 0xC00305;
    uint8_t psc[32] = "weather information nice";
    enum channel_access  chan_access = alternatingTimeslot1Only;
    bool best_available = true;
    uint32_t op_class = 10;
    uint8_t  ps_chan_no = 168;
    uint8_t  wsa_chan_no = 178;
    uint8_t  repeat_rate = 10; 
    bool ip_service = false; 
    uint8_t ipv6_address[16] = ""; 
    uint8_t provider_mac_addr[6] = {161, 35, 69, 103, 137, 203};   // In decimal notation
    uint16_t service_port = 0; 
    int8_t  rcpi_threshold = -20;
    uint8_t  wsa_count_threshold = 23; 
    uint8_t  wsa_count_threshold_intrv = 5; 
    enum service_status  serv_status = satisfied;

    // data set 2
    enum wsa_type  wsatype2 = secured;
    uint32_t psid2 = 0xC00307;
    uint8_t psc2[32] = "traffic information within 1km";
    enum channel_access  chan_access2 = alternatingTimeslot1Only;
    bool best_available2 = false;
    uint32_t op_class2 = 10;
    uint8_t  ps_chan_no2 = 184;
    uint8_t  wsa_chan_no2 = 178;
    uint8_t  repeat_rate2 = 10; 
    bool ip_service2 = true; 
    uint8_t ipv6_address2[16] = {32, 1, 13, 184, 133, 163, 0, 0, 0, 0, 138, 46, 3, 112, 115, 52};   // 2001:0db8:85a3:0000:0000:8a2e:0370:7334
    uint8_t provider_mac_addr2[6] = {103, 56, 17, 98, 234, 12} ;   
    uint16_t service_port2 = 8000; 
    int8_t  rcpi_threshold2 = -8;
    uint8_t  wsa_count_threshold2 = 54; 
    uint8_t  wsa_count_threshold_intrv2 = 12; 
    enum service_status  serv_status2 = partiallySatisfied;

    wme_prvtb_add(wsatype, psid, psc, chan_access, best_available, op_class, ps_chan_no, wsa_chan_no, repeat_rate, ip_service, ipv6_address, provider_mac_addr,
    service_port, rcpi_threshold, wsa_count_threshold, wsa_count_threshold_intrv, serv_status, db);

    wme_prvtb_add(wsatype2, psid2, psc2, chan_access2, best_available2, op_class2, ps_chan_no2, wsa_chan_no2, repeat_rate2, ip_service2, ipv6_address2, provider_mac_addr2,
    service_port2, rcpi_threshold2, wsa_count_threshold2, wsa_count_threshold_intrv2, serv_status2, db);
    // debugging purpose 
    // ProviderServiceRequestTableEntry *entry = get_wme_prvtb(0, db);
    // ProviderServiceRequestTableEntry *entry2 = get_wme_prvtb(1, db);
    // show_wme_prvtb_entry(*entry);
    // printf("\n--------------------------------------------------------\n");
    // show_wme_prvtb_entry(*entry2);
    // free(db);
    return db;
}

ProviderChannelInfoTable *exec_test_channelInfo_tb(){
    // data set-1
    uint8_t op_class = 10;
    uint8_t chan_no = 184;
    bool is_adaptable = true;
    uint8_t data_rate = 23;
    int8_t tx_power_level = 18;

    uint8_t be_cwmin = 2;
    uint32_t be_cwmax = 2784;
    uint8_t be_aifsn = 202;
    uint32_t be_txop_limit = 10001;
    bool be_mand = true;

    uint8_t bk_cwmin = 1;
    uint32_t bk_cwmax = 1340;
    uint8_t bk_aifsn = 201;
    uint32_t bk_txop_limit = 10002;
    bool bk_mand = false;

    uint8_t vi_cwmin = 3;
    uint32_t vi_cwmax = 1026;
    uint8_t vi_aifsn = 203;
    uint32_t vi_txop_limit = 10003;
    bool vi_mand = false;

    uint8_t vo_cwmin = 4;
    uint32_t vo_cwmax = 1587;
    uint8_t vo_aifsn = 204;
    uint32_t vo_txop_limit = 10004;
    bool vo_mand = true;

    // data set-2
    uint8_t op_class2 = 10;
    uint8_t chan_no2 = 168;
    bool is_adaptable2 = false;
    uint8_t data_rate2 = 35;
    int8_t tx_power_level2 = 48;

    ProviderChannelInfoTable *chan_info_tb = create_wme_prv_chan_tb();
    add_wme_prv_chan_tb(op_class, chan_no, is_adaptable, data_rate, tx_power_level, 
        be_cwmin, be_cwmax, be_aifsn, be_txop_limit, be_mand,
        bk_cwmin, bk_cwmax, bk_aifsn, bk_txop_limit, bk_mand,
        vi_cwmin, vi_cwmax, vi_aifsn, vi_txop_limit, vi_mand,
        vo_cwmin, vo_cwmax, vo_aifsn, vo_txop_limit, vo_mand, chan_info_tb);

    add_wme_prv_chan_tb(op_class2, chan_no2, is_adaptable2, data_rate2, tx_power_level2, 
        be_cwmin, be_cwmax, be_aifsn, be_txop_limit, be_mand,
        bk_cwmin, bk_cwmax, bk_aifsn, bk_txop_limit, bk_mand,
        vi_cwmin, vi_cwmax, vi_aifsn, vi_txop_limit, vi_mand,
        vo_cwmin, vo_cwmax, vo_aifsn, vo_txop_limit, vo_mand, chan_info_tb);
    // debugging purpose
    // ProviderChannelInfoTableEntry *entry = get_wme_prv_chan_entry(0, chan_info_tb);
    // ProviderChannelInfoTableEntry *entry2 = get_wme_prv_chan_entry(1, chan_info_tb);
    // show_wme_chan_info_entry(entry);
    // printf("-----------------------------------------------\n");
    // show_wme_chan_info_entry(entry2);

    // free(chan_info_tb);
    return chan_info_tb;
}

void exec_test_wsa(){
    ProviderServiceRequestTable *pr_tb = exec_test_provider_tb();
    ProviderChannelInfoTable *chan_info_tb = exec_test_channelInfo_tb();
    uint8_t wsa_id = 4;
    int err[1];

    wave_pdu *pdu = create_pdu_buf();
    struct wsmp_wsa *wsa_metadata = create_wsa_metadata(wsa_id, pr_tb, chan_info_tb);
    wsmp_wsa_encode(wsa_metadata, pdu, err, WSMP_STRICT);
    if(*err) {
        fprintf(stderr, "went wrong in wsa encoding %d", *err);
        exit(1);
    }
    // print_binx(pdu->current, pdu->offset);
    // print_wsa(wsa_metadata);
    struct wsmp_wsa *decoded_wsa = wsmp_wsa_decode(pdu, err, WSMP_STRICT);
    print_wsa(decoded_wsa);

    free(pr_tb);
    free(chan_info_tb);
    free(wsa_metadata);
    free_pbuf(pdu);
    free(decoded_wsa);
}

void test_provider_service_req(){
    uint16_t local_service_index = 1;
    enum action act = add;
    uint8_t dest_mac_addr[6] = {255, 255, 255, 255, 255, 255}; // brodcast address
    enum wsa_type wsatype = unsecured;
    uint32_t psid = 0xC00305;
    uint8_t psc[32] = "accident alerts";
    uint8_t sch_id = 0;
    uint8_t wsa_chan_id = 178;
    enum time_slot chan_access = alternatingTimeslot1Only;
    uint8_t repeat_rate = 12;
    bool ip_service = false;
    uint8_t ipv6_addr[16] = "";
    uint16_t service_port = 0;
    uint8_t provider_mac_addr[6] = {103, 56, 17, 98, 234, 12}; 
    int8_t rcpi_threshold = -13;
    uint8_t wsa_count_threshold = 8;
    uint8_t wsa_count_thd_interval = 5;
    uint8_t info_elements_indicator = 0;
    uint16_t sign_lifetime = 0;
    mib_t *mib_db = create_mib();

    wme_provider_service_req(local_service_index, act, dest_mac_addr, wsatype, psid, psc, sch_id, wsa_chan_id, chan_access, 
        repeat_rate, ip_service, ipv6_addr, service_port, provider_mac_addr, rcpi_threshold, wsa_count_threshold, wsa_count_thd_interval, 
        info_elements_indicator, sign_lifetime, mib_db);
    
    int err[1];
    wave_pdu *pdu = get_pdu(4, mib_db->pdutb);
    struct wsmp_wsa *decoded_wsa = wsmp_wsa_decode(pdu, err, WSMP_STRICT);
    print_wsa(decoded_wsa);

}

int main(){
    test_provider_service_req();
}
