#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#define CH_INTERVAL 46e3  // 46ms
#define SCKFILE "<path-to-sckfile>"

void slot_mutex_init();
void timer_cond_init();
void slot_mutex_destroy();
void timer_cond_destroy();

void *timer();
void *scheduler();

int server_init(const char *sckfile);
void server_listen(int server_fd);

#endif /* _CONTROLLER_H */
