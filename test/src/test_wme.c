#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/1609_3/wme.h"
#include "../../include/1609_3/ieee1609dot3_mib.h"

void exec_test_provider_tb();

void exec_test_provider_tb(){
    printf("\n----- Provider Service Request Table Info -----\n");
    ProviderServiceRequestTable *db = create_wme_provider_tb();

    // data set 1
    enum wsa_type  wsatype = unsecured;
    uint32_t psid = 0xC00305;
    uint8_t psc[32] = "weather information nice";
    enum channel_access  chan_access = continuous;
    bool best_available = true;
    uint32_t op_class = 14;
    uint8_t  ps_chan_no = 178;
    uint8_t  wsa_chan_no = 174;
    uint8_t  repeat_rate = 10; 
    bool ip_service = false; 
    uint8_t ipv6_address[15] = ""; 
    uint8_t provider_mac_addr[11] = "ff:ff:ff:ff" ;   
    uint32_t service_port = 0; 
    uint8_t  rcpi_threshold = 120;
    uint8_t  wsa_count_threshold = 23; 
    uint8_t  wsa_count_threshold_intrv = 5; 
    enum service_status  serv_status = satisfied;

    // data set 2
    enum wsa_type  wsatype2 = secured;
    uint32_t psid2 = 0xC00307;
    uint8_t psc2[32] = "traffic information within 1km";
    enum channel_access  chan_access2 = alternatingTimeslot0Only;
    bool best_available2 = false;
    uint32_t op_class2 = 16;
    uint8_t  ps_chan_no2 = 172;
    uint8_t  wsa_chan_no2 = 188;
    uint8_t  repeat_rate2 = 14; 
    bool ip_service2 = true; 
    uint8_t ipv6_address2[15] = "192.188.101.129"; 
    uint8_t provider_mac_addr2[11] = "3f:12:af:bc" ;   
    uint32_t service_port2 = 8000; 
    uint8_t  rcpi_threshold2 = 178;
    uint8_t  wsa_count_threshold2 = 54; 
    uint8_t  wsa_count_threshold_intrv2 = 12; 
    enum service_status  serv_status2 = partiallySatisfied;

    wme_prvtb_add(wsatype, psid, psc, chan_access, best_available, op_class, ps_chan_no, wsa_chan_no, repeat_rate, ip_service, ipv6_address, provider_mac_addr,
    service_port, rcpi_threshold, wsa_count_threshold, wsa_count_threshold_intrv, serv_status, db);

    wme_prvtb_add(wsatype2, psid2, psc2, chan_access2, best_available2, op_class2, ps_chan_no2, wsa_chan_no2, repeat_rate2, ip_service2, ipv6_address2, provider_mac_addr2,
    service_port2, rcpi_threshold2, wsa_count_threshold2, wsa_count_threshold_intrv2, serv_status2, db);

    ProviderServiceRequestTableEntry *entry = get_wme_prvtb(0, db);
    ProviderServiceRequestTableEntry *entry2 = get_wme_prvtb(1, db);
    show_wme_prvtb_entry(*entry);
    printf("\n--------------------------------------------------------\n");
    show_wme_prvtb_entry(*entry2);
    free(db);
}

int main(){
    exec_test_provider_tb();
}
