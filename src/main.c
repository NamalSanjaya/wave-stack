#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> 

#include "../include/controller.h"
#include "../include/1609_3/wme.h"

int main(){
    pthread_t timer_th, scheduler_th;
    slot_mutex_init();
    timer_cond_init();

    mib_t *mib_db = create_mib();

    if(pthread_create(&timer_th, NULL, &timer, NULL) != 0) return 1;

    if(pthread_create(&scheduler_th, NULL, &scheduler, (void *)mib_db) != 0) return 1;

    int server_fd = server_init(SCKFILE);
    if(server_fd == -1) return 1;
    server_listen(server_fd, mib_db);

    slot_mutex_destroy();
    timer_cond_destroy();
    return 0;
}
