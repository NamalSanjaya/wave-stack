#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

#include <libwave_sock.h>

#define SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/sckfile"

int app_send_wsa();
int app_send_wsm(int32_t lat, int32_t longt);
void store8(uint8_t *buf,uint8_t *i, uint8_t data);
void store32(uint8_t *buf,uint8_t *i, uint32_t data);
void print_bin8(uint8_t value);
void print_binx(uint8_t *buf, size_t size);

int app_send_wsa(){
    enum action act = add;
    uint8_t dest_mac_addr[6] = {255, 255, 255, 255, 255, 255}; // brodcast address
    enum wsa_type wsatype = unsecured;
    uint32_t psid = 0x8007;
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

int app_send_wsm(int32_t lat, int32_t longt){
    uint8_t chan_id = 172; 
    enum time_slot timeslot = time_slot1; 
    uint8_t data_rate = 67; 
    int8_t tx_power = -10; 
    uint8_t channel_load = 5; 
    uint8_t info_elem_indicator = 1; 
    uint8_t prority = 1; 
    uint64_t wsm_expire_time = 10; 

    // only for two 32-bits fields
    uint16_t len = 64; 

    int32_t loc2d_lat, loc2d_longt;
    loc2d_lat = lat;
    loc2d_longt = longt;

    uint8_t *data = (uint8_t *) calloc(len, sizeof(uint8_t));

    uint8_t indx = 0;
    store32(data, &indx, loc2d_lat);
    store32(data, &indx, loc2d_longt);

    uint8_t peer_mac_addr[6] =  {255, 255, 255, 255, 255, 255}; 
    uint32_t psid = 0xC00305;

    int8_t st = app_wsm_waveshortmsg_req(chan_id, timeslot,  data_rate, tx_power, channel_load, info_elem_indicator, prority, 
        wsm_expire_time, len, data, peer_mac_addr, psid, SCKFILE);

    // print_binx(data, len);
    if (st<0){
        printf("something went wrong...\n");
        return 1;
    }
    printf("--done--\n");
    return 0;
}

void store8(uint8_t *buf,uint8_t *i, uint8_t data){
    buf[*i] = data;
    (*i)++;
}

void store32(uint8_t *buf,uint8_t *i, uint32_t data){
    store8(buf, i, data >> 24);
    store8(buf, i, data >> 16);
    store8(buf, i, data >> 8);
    store8(buf, i, data);
}

// print binary format of 1-byte value
void print_bin8(uint8_t value){
    for (int i = 7; i >= 0; i--)
        printf("%d", (value & (1 << i)) >> i );
    printf(",");
}

// print binary format for a given size.
void print_binx(uint8_t *buf, size_t size){
    for (size_t i = 0; i < size; i++){
        print_bin8(*(buf+i));
    }
    printf("\n");
}

int main() {
    int min = -10;    // Minimum value of the range
    int max = 200;  // Maximum value of the range

    // Seed the random number generator with the current time
    srand(time(NULL));
    int count = 0;
    while (count < 105){
        // Generate a random number between min and max
        int randomNum = (rand() % (max - min + 1)) + min;

        int32_t lat = (int32_t) randomNum;
        int32_t longt = (int32_t) count;
        app_send_wsm(lat, longt);

        count++;
        usleep(25000);
    }
    printf("No.of packet sent: %d\n", count);
    return 0;
}
