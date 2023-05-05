#include <stdio.h>
#include <stdint.h>

#include "../../lib/wave_sock.h"
#include "../../include/1609_3/wme.h"

int main(){

    enum action act = add;
    uint8_t dest_mac_addr[6] = {255, 255, 255, 255, 255, 255}; // brodcast address
    enum wsa_type wsatype = unsecured;
    uint32_t psid = 0xC00305;
    uint8_t psc[32] = "accident alerts";
    uint8_t psc_len = 15;
    enum time_slot chan_access = alternatingTimeslot1Only;
    bool ip_service = false;
    uint8_t ipv6_addr[16] = "";
    uint16_t service_port = 0;
    int8_t rcpi_threshold = -13;
    uint8_t wsa_count_threshold = 8;
    uint8_t wsa_count_thd_interval = 5;

    int8_t st = app_provider_service_req(act, dest_mac_addr, wsatype, psid, psc, psc_len, chan_access, ip_service, ipv6_addr, service_port, 
        rcpi_threshold, wsa_count_threshold, wsa_count_thd_interval);

    if (st<0){
        printf("something went wrong...\n");
        exit(1);
    }
    return 0;
}

