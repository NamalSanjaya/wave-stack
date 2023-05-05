#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../include/1609_3/wsmp.h"
#include "../test/include/test_wsmp.h"
#include "wave_sock.h"

int main() {
    int server_fd, client_fd, len;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len;
    app_ProviderServiceReqEntry psre;

    // Create a socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a name
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SCKFILE, sizeof(server_addr.sun_path) - 1);
    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("listen....\n");
    // Accept a connection
    client_addr_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Receive the employee struct from the client
    len = recv(client_fd, &psre, sizeof(psre), 0);
    if (len == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    // Print the received employee struct
    print_app_ProviderServiceReqEntry(&psre);

    // Close the client and server sockets
    close(client_fd);
    close(server_fd);

    return 0;
}
