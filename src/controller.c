#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "../include/controller.h"
#include "../include/1609_3/wme.h"
#include "../include/pdu_buf.h"

int slot = 0;

pthread_mutex_t mutex_slot;
pthread_cond_t chan_timer;

void slot_mutex_init(){
    pthread_mutex_init(&mutex_slot, NULL);
}

void timer_cond_init(){
    pthread_cond_init(&chan_timer, NULL);
}

void slot_mutex_destroy(){
    pthread_mutex_destroy(&mutex_slot);
}

void timer_cond_destroy(){
    pthread_cond_destroy(&chan_timer);
}

void *timer(){
    while (1){
        usleep(CH_INTERVAL);
        pthread_cond_broadcast(&chan_timer);
    }
}

void *scheduler(void *arg){
    mib_t *mib_db = (mib_t *) arg;
    pthread_mutex_lock(&mutex_slot);
    while(1) {
        if(slot == 0){
            printf("time slot 0: TX: BroadcastWSA()  | RX: MonitorWSA()\n");
            /** Need a function to perform following tasks.(Tx part)
             * 1. loop over and check whether 
             *  a) Encoded/stored WSA is there?
             *  b) is that one changed(due to update in ProviderServiceRequestTable)
             *  c) Do we need to create new WSA and store in PDU Table
             */
            broadcast_wsa(mib_db);

        } 
        else if(slot == 1) {
            printf("time slot 1: TX: SendActualWSM() | RX: ListenToIncomingWSM()\n");
        }
        slot = slot == 0 ? 1 : 0;
        pthread_cond_wait(&chan_timer, &mutex_slot);
    }
    pthread_mutex_unlock(&mutex_slot);
}

int server_init(const char *sckfile){
    int server_fd;
    struct sockaddr_un server_addr;

    // Create a socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) return -1;

    // Bind the socket to a name
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, sckfile, sizeof(server_addr.sun_path) - 1);
    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) return -1;

    // Listen for connections. can queue upto 32 requests
    if (listen(server_fd, 32) == -1) return -1;
    printf("listen....\n");
    return server_fd;
}

void server_listen(int server_fd, mib_t *mib_db){
    int client_fd, len;
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;
    local_req_t req;

    while(1){
        // Accept a connection
        client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len);
        if (client_fd == -1) {
            printf("connection failed...\n");
            continue;
        }

        len = recv(client_fd, &req, sizeof(req), 0);
        if (len == -1) {
            printf("receiving error...\n");
            continue;
        }
        hand_over_stack(&req, mib_db);
        close(client_fd);
    }
}

void hand_over_stack(local_req_t *req, mib_t *mib_db){

    uint16_t local_service_index = 1;     // TODO: Parameters which are not finialized
    uint8_t dest_mac_addr[6] = {255, 255, 255, 255, 255, 255}; // Brodcast address
    uint8_t sch_id = 0;   // Let the stack to select a channel
    uint8_t repeat_rate = 10;  // TODO: Need to be calculated
    uint8_t provider_mac_addr[6] = {11,12,13,14,15,16};   // TODO: put right device MAC address
    uint8_t info_elements_indicator = 0;
    uint16_t sign_lifetime = 0;

    if(req->id == 11){
        app_ProviderServiceReqEntry psre = req->psre;
        wme_provider_service_req(local_service_index, psre.act, dest_mac_addr, psre.wsatype, psre.psid, psre.psc, sch_id,
            DEFAULT_CCH, psre.chan_access, repeat_rate, psre.ip_service, psre.ipv6_addr, psre.service_port, provider_mac_addr,
            psre.rcpi_threshold, psre.wsa_count_threshold, psre.wsa_count_thd_interval, info_elements_indicator, sign_lifetime, mib_db);
    }
}

void broadcast_wsa(mib_t *mib_db){
    uint8_t wsa_id = 4;
    int err[1];
    wave_pdu *wsa = get_pdu(wsa_id, mib_db->pdutb);
    if(wsa == NULL){
        return;
    }
    uint32_t psid = 1;     // TODO: Need to correct
    uint8_t peer_mac_addr[6] = {255, 255, 255, 255, 255, 255};
    wave_pdu *pdu = create_pdu_buf();   // TODO: check right place to free the pdu memory
    wsm_waveshortmsg_req(0, time_slot0, 0, 0, 0, 0, 0, 0, wsa->offset, wsa->current, peer_mac_addr, psid, pdu);
}
