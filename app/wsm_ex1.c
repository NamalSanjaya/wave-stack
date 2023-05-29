#include <stdio.h>
#include <libwave_sock.h>
#include <stddef.h>

#define SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/sckfile"
#define WAVE_SCKFILE "/home/sdrns/workspace/fyp_work/layer_development/wave_stack/test/bin/wave_sckfile"

void callback(local_resp_t *resp);
void print_bin8_temp(uint8_t value);
void print_binx_temp(uint8_t *buf, size_t size);

int main(){
    int sock_fd = wave_sock_open(WAVE_SCKFILE);
    if(sock_fd == -1) return 1;
    wave_sock_listen(sock_fd, callback);
}

void callback(local_resp_t *resp){

    int32_t lat = 0;
    int32_t longt = 0;

    // Extract the first 32 bits
    for (int i = 0; i < 4; i++) {
        lat |= ( (int32_t) (resp->buf)+i << (24 - (8 * i)));
    }

    for (int i = 4; i < 8; i++) {
        longt |= ( (int32_t) (resp->buf)+i << (24 - (8 * (i - 4))));
    }
    printf("lat: %d, longt: %d\n", lat, longt);
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