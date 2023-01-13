#ifndef _WAVE_SAP_H
#define _WAVE_SAP_H

#include <sys/epoll.h>
#include "ds/hashtb.h"

#define MAX_EPOLL_EVENTS 5
#define MAX_CONN 5
#define MAX_READ 10
#define MAX_SOCKET_FILE_NAME_LEN 20

typedef struct wave_sap {
    int server_socket_fd;
    int epoll_fd;
    int reg_socket_count;
    char socket_file[MAX_SOCKET_FILE_NAME_LEN];
    ht *reg_sockets;
    ht *as_clients; // SAP interface as a client info in a hash table.
} wave_sap;

wave_sap *init_wave_sap(const char *filename);

#endif  /* _WAVE_SAP_H */
