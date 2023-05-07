#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "libwave_sock.h"

#define SCKFILE "<path-to-sockfile>"

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
    int len;
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;
    app_ProviderServiceReqEntry psre;

    while(1){
        // Accept a connection
        client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len);
        if (client_fd == -1) {
            printf("connection failed...\n");
            continue;
        }

        // Receive the employee struct from the client
        len = recv(client_fd, &psre, sizeof(psre), 0);
        if (len == -1) {
            printf("receiving error...\n");
            continue;
        }
        // Print the received employee struct
        print_app_ProviderServiceReqEntry(&psre);
        close(client_fd);
    }
}
