#include <stdio.h>
#include <libwave_sock.h>
#include <stddef.h>

#define SCKFILE "/home/suhail/workspace2/fyp/wave-stack/test/bin/sckfile"
#define WAVE_SCKFILE "/home/suhail/workspace2/fyp/wave-stack/test/bin/wave_sckfile"


#define MUL_FACTOR 1e7

void callback(local_resp_t *resp);
void callback_old(local_resp_t *resp);
void print_bin8_temp(uint8_t value);
void print_binx_temp(uint8_t *buf, size_t size);

int main(){
    int sock_fd = wave_sock_open(WAVE_SCKFILE);
    if(sock_fd == -1) return 1;
    wave_sock_listen(sock_fd, callback);
}

// void callback(local_resp_t *resp){
//     print_binx_temp(resp->buf, resp->data_size);
//     printf("data-size: %ld...\n", resp->data_size);
// }

void callback(local_resp_t *resp){
    print_binx_temp(resp->buf, resp->data_size);
    printf("data-size: %ld...\n", resp->data_size);
}

void callback_old(local_resp_t *resp){

    uint32_t lat = 0;
    uint32_t longt = 0;

    // Extract the first 64 bits
    for (int i = 0; i < 4; i++) {
        lat |= ( (uint32_t) *((resp->buf)+i) << (24 - (8 * i)));
    }

    for (int i = 4; i < 8; i++) {
        longt |= ( (uint32_t) *((resp->buf)+i) << (24 - (8 * (i - 4))));
    }

    double lat_2d = lat/MUL_FACTOR;
    double longt_2d = longt/MUL_FACTOR;


    printf("lat: %.7f, longt: %.7f\n", lat_2d, longt_2d);
    printf("data-size: %ld...\n", resp->data_size);
}

// print binary format of 1-byte value
void print_bin8_temp(uint8_t value){
    for (int i = 7; i >= 0; i--)
        printf("%d", (value & (1 << i)) >> i );
    printf(",");
}

// print binary format for a given size.
void print_binx_temp(uint8_t *buf, size_t size){
    for (size_t i = 0; i < size; i++){
        print_bin8_temp(*(buf+i));
    }
    printf("\n");
}