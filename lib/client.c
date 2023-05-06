#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "libwave_sock.h"

void print_app_ProviderServiceReqEntry(app_ProviderServiceReqEntry *psre){
    printf("\n-----BEGIN APP ProviderServiceReqEntry-------\n");
    printf("Action: %d\n", psre->act);
    printf("Dest MAC Addr:      %02x:%02x:%02x:%02x:%02x:%02x\n",
        psre->dest_mac_addr[0], psre->dest_mac_addr[1], psre->dest_mac_addr[2],
        psre->dest_mac_addr[3], psre->dest_mac_addr[4], psre->dest_mac_addr[5]);
    printf("WSA Type:  %d\n", psre->wsatype);
    printf("PSID:  %d\n", psre->psid);

    printf("psc_len:          %d\n", psre->psc_len);
    printf("psc: |");

    for(int i = 0; i < psre->psc_len; i++)
        printf("%02x", psre->psc[i]);
    printf("|");
    printf("\nChannel Access:  %d\n", psre->chan_access);

    if(psre->ip_service){
        printf("Ipv6 Addr: \n");
        for(int i = 0; i < 16; i++)
            printf("%02x", psre->ipv6_addr[i]);
        
    } else printf("Ipv6 not present..\n");
    
    printf("port:  %d\n", psre->service_port);
    printf("rcpi_threshold:  %d\n", psre->rcpi_threshold);
    printf("wsa_count_threshold:  %d\n", psre->wsa_count_threshold);
    printf("wsa_count_thd_interval:  %d\n", psre->wsa_count_thd_interval);

}

int8_t _put_uint8(uint8_t *buf, size_t *i, const uint8_t v) {
    if (*i >= DATA_MAXSIZE) return -1;
    
    buf[*i] = v;
    (*i)++;
    return 0;
}

int8_t _put_uint16(uint8_t *buf, size_t *i, const uint16_t v) { 
    buf[*i] = v >> 8;
    (*i)++;   
 
    buf[*i] = v;
    (*i)++;

    if (*i >= DATA_MAXSIZE) return -1;
    return 0;
}

int8_t _put_uint32(uint8_t *buf, size_t *i, const uint32_t v) {    

    buf[*i] = v >> 24;
    (*i)++;   
 
    buf[*i] = v >> 16;
    (*i)++;

    buf[*i] = v >> 8;
    (*i)++;   
 
    buf[*i] = v;
    (*i)++;

    if (*i >= DATA_MAXSIZE) return -1;
    return 0;
}

int8_t _put_uint8_n(uint8_t *buf, size_t *i, size_t n, const uint8_t *v) {
    for (int j = 0; j < n; j++)
	    _put_uint8(buf, i, v[j]);
    
    if (*i >= DATA_MAXSIZE) return -1;
    return 0;
}

int send_data(uint8_t *src, size_t size, const char *sckfile){
    const char* socket_name = sckfile;
    int socket_fd;
    struct  sockaddr_un name;

    socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socket_name);
    connect(socket_fd, &name, SUN_LEN(&name));

    int written = write(socket_fd, src, size);
    
    close(socket_fd);
    return written;
}

int init_socket(const char *sckfile){
    int socket_fd;
    struct sockaddr_un name;

    // Create a socket
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) return -1;

    // Connect to the server socket
    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, sckfile, sizeof(name.sun_path) - 1);
    if (connect(socket_fd, (struct sockaddr*) &name, sizeof(name)) == -1) return -1;

    return socket_fd;
}

int8_t app_provider_service_req(enum action act, uint8_t *dest_mac_addr, enum wsa_type wsatype, uint32_t psid, 
    uint8_t *psc, uint8_t psc_len, enum channel_access chan_access, bool ip_service, uint8_t *ipv6_addr, uint16_t service_port, int8_t rcpi_threshold, 
    uint8_t wsa_count_threshold, uint8_t wsa_count_thd_interval, const char *sckfile){
    /**
     * 1. do the encoding here.
     * 2. send_data()
     */
    app_ProviderServiceReqEntry *psre = calloc(1, sizeof(app_ProviderServiceReqEntry));
    if(psre == NULL){
        return -1;
    }
    psre->id = WME_ProviderService_request;
    psre->act = act;
    memcpy(psre->dest_mac_addr, dest_mac_addr, 6);
    psre->wsatype = wsatype;
    psre->psid = psid;
    memcpy(psre->psc, psc, 32);
    psre->psc_len = psc_len;
    psre->chan_access = chan_access;
    psre->ip_service = ip_service;
    memcpy(psre->ipv6_addr, ipv6_addr, 16);
    psre->service_port = service_port;
    psre->rcpi_threshold = rcpi_threshold;
    psre->wsa_count_threshold = wsa_count_threshold;
    psre->wsa_count_thd_interval = wsa_count_thd_interval;

    int socket_fd = init_socket(sckfile);

    if (send(socket_fd, psre, sizeof(*psre), 0) == -1) {
        printf("went wrong..\n");
        return -1;
    }
    // Close the socket
    close(socket_fd);
    free(psre);
    return 0;
}
