#include <pcap.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/if_tun.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#include "../include/network.h"
#include "../include/fmt_error.h"
#include "../include/pdu_buf.h"


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

int alloc_tun(char *dev) {
    struct ifreq ifr;
    int fd, err;

    // open the /dev/net/tun device
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open");
        return -1;
    }

    // configure the device 
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    // create the device 
    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        perror("ioctl");
        close(fd);
        return err;
    }
    return fd;
}

void write_tun(wave_pdu *pdu){
    int sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
    int ifindex = 4;  // tun interface index

    struct sockaddr_ll SendSockAddr;
    SendSockAddr.sll_family   = AF_PACKET;
    SendSockAddr.sll_halen    = ETH_ALEN;
    SendSockAddr.sll_ifindex  = ifindex;
    SendSockAddr.sll_protocol = htons(ETH_P_ALL);
    SendSockAddr.sll_hatype   = 0;
    SendSockAddr.sll_pkttype  = 0;

    ssize_t total =  sendto(sockfd, pdu->current, pdu->offset, 0, (struct sockaddr *) &SendSockAddr, sizeof(struct sockaddr_ll));
    if(total < 0) {
        fmt_error(WAVE_WARN, "Failed to write data to tap interface.");
    }
    printf("WSM data Send to GNU Radio\n");
    
    close(sockfd);
    free_pbuf(pdu);
}
