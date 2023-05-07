#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> 

#include "../include/controller.h"

int main(){
    pthread_t timer_th, scheduler_th;
    slot_mutex_init();
    timer_cond_init();

    if(pthread_create(&timer_th, NULL, &timer, NULL) != 0) return 1;

    if(pthread_create(&scheduler_th, NULL, &scheduler, NULL) != 0) return 1;

    int server_fd = server_init(SCKFILE);
    if(server_fd == -1) return 1;
    server_listen(server_fd);

    slot_mutex_destroy();
    timer_cond_destroy();
    return 0;
}
