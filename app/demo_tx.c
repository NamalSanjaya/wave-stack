#include <stdio.h>
#include <unistd.h> 
#include <stdint.h>
#include <stdlib.h>

// header file to communicate with wave stack
#include <libwave_sock.h>

#define SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/sckfile"

#define MUL_FACTOR 1e7

int app_send_wsm(double lat, double longt);
void store8(uint8_t *buf,uint8_t *i, uint8_t data);
void store32(uint8_t *buf,uint8_t *i, uint32_t data);

int main(){
    double tx_fix_lat = 6.7967400;
    double tx_fix_longt = 79.9012834;
    while(1){
        app_send_wsm(tx_fix_lat, tx_fix_longt);
        usleep(80000);
    }
}

// Communicate with stack
int app_send_wsm(double lat, double longt){
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

    uint32_t loc2d_lat = (uint32_t) (lat*MUL_FACTOR);
    uint32_t loc2d_longt = (uint32_t) (longt*MUL_FACTOR);

    uint8_t *data = (uint8_t *) calloc(len, sizeof(uint8_t));

    uint8_t indx = 0;
    store32(data, &indx, loc2d_lat);
    store32(data, &indx, loc2d_longt);

    uint8_t peer_mac_addr[6] =  {255, 255, 255, 255, 255, 255}; 
    uint32_t psid = 0xC00305;

    int8_t st = app_wsm_waveshortmsg_req(chan_id, timeslot,  data_rate, tx_power, channel_load, info_elem_indicator, prority, 
        wsm_expire_time, len, data, peer_mac_addr, psid, SCKFILE);

    if (st<0){
        printf("failed to communicate with wave socket.\n");
        return -1;
    }
    // printf("GPS data send to wave stack.\n");
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
