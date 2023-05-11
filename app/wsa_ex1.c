#include <stdio.h>
#include <stdint.h>

#include <libwave_sock.h>

#define SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/sckfile"

int app_send_wsa();
int app_send_wsm();

int app_send_wsa(){
    enum action act = add;
    uint8_t dest_mac_addr[6] = {255, 255, 255, 255, 255, 255}; // brodcast address
    enum wsa_type wsatype = unsecured;
    uint32_t psid = 0xC00305;
    uint8_t psc[32] = "accident alerts";
    uint8_t psc_len = 15;
    enum channel_access chan_access = alternatingTimeslot1Only;
    bool ip_service = false;
    uint8_t ipv6_addr[16] = "";
    uint16_t service_port = 0;
    int8_t rcpi_threshold = -13;
    uint8_t wsa_count_threshold = 8;
    uint8_t wsa_count_thd_interval = 5;

    int8_t st = app_provider_service_req(act, dest_mac_addr, wsatype, psid, psc, psc_len, chan_access, ip_service, ipv6_addr, service_port, 
        rcpi_threshold, wsa_count_threshold, wsa_count_thd_interval, SCKFILE);

    if (st<0){
        printf("something went wrong...\n");
        return 1;
    }
    printf("--done--\n");
    return 0;
}

int app_send_wsm(){
    uint8_t chan_id = 172; 
    enum time_slot timeslot = time_slot1; 
    uint8_t data_rate = 67; 
    int8_t tx_power = -10; 
    uint8_t channel_load = 5; 
    uint8_t info_elem_indicator = 1; 
    uint8_t prority = 1; 
    uint64_t wsm_expire_time = 10; 
    uint16_t len = 11; 

    uint8_t data[1024] = "hello-world";
    uint8_t peer_mac_addr[6] =  {255, 255, 255, 255, 255, 255}; 
    uint32_t psid = 0xC00305;

    int8_t st = app_wsm_waveshortmsg_req(chan_id, timeslot,  data_rate, tx_power, channel_load, info_elem_indicator, prority, 
        wsm_expire_time, len, data, peer_mac_addr, psid, SCKFILE);

    if (st<0){
        printf("something went wrong...\n");
        return 1;
    }
    printf("--done--\n");
    return 0;
}

int main(){
    return app_send_wsm();
}
