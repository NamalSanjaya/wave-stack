#include <pcap.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/network.h"
#include "../include/fmt_error.h"

uint8_t *get_mac_addr(char *device){
    struct ifreq ifr;
    size_t ifname_len = strlen(device);

    if (ifname_len < sizeof(ifr.ifr_name)) {
	    memcpy(ifr.ifr_name, device, ifname_len);
	    ifr.ifr_name[ifname_len] = 0;
    } else {
        fmt_error(WAVE_WARN, "unable to get MAC address of src device due to !(ifname_len < sizeof(ifr.ifr_name))");
	    return NULL;
    }

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        fmt_error(WAVE_WARN, "unable to get MAC address of src device since failed to open a socket");
        return NULL;
    }
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        close(fd);
        fmt_error(WAVE_WARN, "unable to get MAC address of src device since failed perform specified I/O operation on given FD");
        return NULL;
    }
    uint8_t *src_mac = calloc(6,sizeof(uint8_t));
    if (src_mac == NULL){
        fmt_error(WAVE_ERROR, "unable to get MAC address of src device since failed to allocate memory");
        return NULL;
    }
    memcpy(src_mac, (uint8_t *)ifr.ifr_hwaddr.sa_data, 6);
    close(fd);
    return src_mac;
}

char *get_network_device() {
    char error_buffer[PCAP_ERRBUF_SIZE];
    return pcap_lookupdev(error_buffer);
}
