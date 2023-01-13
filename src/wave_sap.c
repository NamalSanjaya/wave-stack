#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "../include/wave_sap.h"
#include "../include/ds/hashtb.h"
#include "../include/fmt_error.h"

int init_server_socket(const char *filename){
    struct sockaddr_un name;
    int socket_fd;
    size_t size;

    /* Create the socket. */
    socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (socket_fd < 0) fmt_panic("failed to initiate server socket");

    /* Bind a name to the socket. */
    name.sun_family = AF_LOCAL;
    strncpy(name.sun_path, filename, sizeof (name.sun_path));
    name.sun_path[sizeof (name.sun_path) - 1] = '\0';
    size = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path));

    if (bind (socket_fd, (struct sockaddr *) &name, size) < 0) fmt_panic("falied to bind server socket");
    printf("listen...\n");
    if(listen(socket_fd, 5) < 0) fmt_panic("failed to listen to server socket");
    return socket_fd;
}

void watch_socket(wave_sap* self, int watch_fd){
    struct epoll_event *event = (struct epoll_event *) malloc(sizeof(struct epoll_event));
    if(event == NULL) {
        fmt_error(WAVE_ERROR, "falied to allocated memory via malloc");
        return;
    }
	event->events = EPOLLIN;
	event->data.fd = watch_fd;

	if(epoll_ctl(self->epoll_fd, EPOLL_CTL_ADD, watch_fd, event)){
		fmt_error(WAVE_ERROR, "failed to add file descriptor to server_epoll");
		return;
	}
    if(ht_set(self->reg_sockets, "key1", event) == NULL) { 
        fmt_error(WAVE_ERROR, "falied to insert client socket");
        return;
    }
    self->reg_socket_count++;
}

/*
    1. while loop is not good. find good althernative
    2. need a way to shutdown the thread. free resources.
*/
void *accept_conn(void *arg){
    while(1){
        wave_sap* sap = (wave_sap *)arg;
        struct sockaddr_un client_sockaddr;
        socklen_t client_sockaddr_len;
        int client_socket_fd;

        client_socket_fd = accept(sap->server_socket_fd, &client_sockaddr, &client_sockaddr_len);
        if(client_socket_fd < 0) {
            fmt_error(WAVE_WARN, "unable to accept client request");
            continue;
        }
        printf("accepted socket: %d\n", client_socket_fd);
        watch_socket(sap, client_socket_fd);
    }
}

/*
    1. as a client
    2. save to a buffer (need to think about)
    3. how to shutdown and remove resources in `conn_accept_thread`
*/
wave_sap *init_wave_sap(const char *filename){
    wave_sap *sap;
    pthread_t conn_accept_thread;
	int sap_epoll_fd = epoll_create1(0);
    if (sap_epoll_fd == -1) fmt_panic("failed to create epoll file descriptor");

    int server_socket_fd = init_server_socket(filename);  /* start socket to listen on incoming request/data */
    sap = (wave_sap *) malloc(sizeof(wave_sap));
    if(sap == NULL) fmt_panic("could not allocate memory");
     
    sap->server_socket_fd = server_socket_fd;
    sap->epoll_fd = sap_epoll_fd;
    sap->reg_socket_count = 0;
    sap->reg_sockets = ht_create(5);
    sap->as_clients = ht_create(5);
    if (sap->reg_sockets == NULL || sap->as_clients == NULL) fmt_panic("unable to create hash tables needed for SAP interface");
    strcpy(sap->socket_file, filename);

    if(pthread_create(&conn_accept_thread, NULL, &accept_conn, sap) != 0) fmt_panic("could not create thread for accepting socket connections");
    return sap;
}
