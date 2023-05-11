#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "./1609_3/wme.h"

#define CH_INTERVAL 3e6  // 46ms
#define SCKFILE "<path-to-sckfile>"

void slot_mutex_init();
void timer_cond_init();
void slot_mutex_destroy();
void timer_cond_destroy();

void *timer();
void *scheduler(void *arg);

int server_init(const char *sckfile);
void server_listen(int server_fd, mib_t *mib_db);

void hand_over_stack(local_req_t *req, mib_t *mib_db);
void broadcast_wsa(mib_t *mib_db);
void send_wsm(WSM_ReqTable_t *wsm_tb);

#endif /* _CONTROLLER_H */
