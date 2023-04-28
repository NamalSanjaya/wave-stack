#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/1609_3/wme.h"
#include "../../include/1609_3/ieee1609dot3_mib.h"

int main(){
    ProviderServiceRequestTable *db = create_provider_tb();

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
    uint8_t ipv6_address[15] = "255.255.101.129"; 
    uint8_t provider_mac_addr[11] = "ff:ff:ff:ff" ;   
    uint32_t service_port = 8080; 
    uint8_t  rcpi_threshold = 120;
    uint8_t  wsa_count_threshold = 23; 
    uint8_t  wsa_count_threshold_intrv = 5; 
    enum service_status  serv_status = satisfied;

    wme_prvtb_add(wsatype, psid, psc, chan_access, best_available, op_class, ps_chan_no, wsa_chan_no, repeat_rate, ip_service, ipv6_address, provider_mac_addr,
    service_port, rcpi_threshold, wsa_count_threshold, wsa_count_threshold_intrv, serv_status, db);

    ProviderServiceRequestTableEntry *entry = wme_prvtb_get_by_psid(psid, db);
    show_tb_entry(*entry);
    free(entry);
}
