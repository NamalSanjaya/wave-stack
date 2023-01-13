#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/1609_3/llc/llc_sap.h"
#include "../../../include/fmt_error.h"

/* for llc_main function to create LLC SAP instance of 1609.3. inherte from wave_sap. */
wave_llc_sap *init_wave_llc_sap(char *layer_name, char *filename){
    wave_llc_sap *llc_sap = (wave_llc_sap *) malloc(sizeof(wave_llc_sap));
    if (llc_sap == NULL) fmt_panic("unable to allocate memory to create LLC_SAP");
   
    wave_sap *sap = init_wave_sap(filename);
    strcpy(llc_sap->layer, layer_name);
    llc_sap->sap = sap;
    return llc_sap;
}

/**
 * 1. Need to change based on how our incoming data looks like.
 * 2. Do proper shutdown, error handling
 */
void recv_incoming_data(wave_llc_sap *llc_sap){
    struct epoll_event events[MAX_EPOLL_EVENTS];
	char read_buffer[MAX_READ];
	size_t bytes_read;
    int event_count=0;

    while(1){
        printf("epoll waiting...\n");
	    event_count = epoll_wait(llc_sap->sap->epoll_fd, events, MAX_EPOLL_EVENTS, -1);
        for (size_t i = 0; i < event_count; i++){
		    bytes_read = read(events[i].data.fd, read_buffer, MAX_READ);
            printf("read-%ld: %s, size: %ld\n",i, read_buffer, bytes_read);
            if(!strncmp(read_buffer, "stop", 4)){
                goto end;
            }
            if(!strncmp(read_buffer, "quit", 4)){
                // close_all_clients(srv);
                goto end;
            }
        }
    }
end: 
    if(close(llc_sap->sap->epoll_fd)){
        fprintf(stderr, "Failed to close epoll file descriptor\n");
    }
    close(llc_sap->sap->server_socket_fd);
    unlink(llc_sap->sap->socket_file);
    free(llc_sap->sap);
    free(llc_sap);
}

// void wave_dl_unitdatax_request(){

// }
