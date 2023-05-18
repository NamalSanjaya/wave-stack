#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "libwave_sock.h"


int wave_sock_open_temp(const char *sckfile){
    int sock_fd;
    struct sockaddr_un server_addr;

    // Create a socket
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) return -1;

    // Bind the socket to a name
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, sckfile, sizeof(server_addr.sun_path) - 1);
    if (bind(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) return -1;

    // Listen for connections. can queue upto 32 requests
    if (listen(sock_fd, 32) == -1) return -1;
    return sock_fd;
}

void wave_sock_listen_temp(int sock_fd){
    int len;
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;
    local_resp_t resp;

    while(1){
        // Accept a connection
        client_addr_len = sizeof(client_addr);
        client_fd = accept(sock_fd, (struct sockaddr*) &client_addr, &client_addr_len);
        if (client_fd == -1) {
            printf("wave_socket failed...\n");
            continue;
        }

        // Receive the employee struct from the client
        len = recv(client_fd, &resp, sizeof(resp), 0);
        if (len == -1) {
            printf("receiving error in wave socket...\n");
            continue;
        }
        // Print the received employee struct
        printf("data-size: %d...\n", resp->data_size);
        close(client_fd);
    }
}
