#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "../lib/libwave_sock.h"
#include "../include/controller.h"

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

void *scheduler(){
    pthread_mutex_lock(&mutex_slot);
    while(1) {
        if(slot == 0){
            printf("time slot 0: TX: BroadcastWSA()  | RX: MonitorWSA()\n");
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

void server_listen(int server_fd){
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
        
        printf("Id: %d\n", req.id);
        print_app_ProviderServiceReqEntry(&req.psre);
        close(client_fd);
    }
}
