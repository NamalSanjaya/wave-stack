#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <stddef.h>
#include <stdint.h>
#include <pcap.h>

#include "./1609_3/wme.h"
#include "pdu_buf.h"

#define CH_INTERVAL 100e3  // 50ms
#define SCKFILE "/home/suhail/workspace2/fyp/wave-stack/test/bin/sckfile"
#define WAVE_SCKFILE "/home/suhail/workspace2/fyp/wave-stack/test/bin/wave_sckfile"

#define PCAPFILE "/home/suhail/workspace2/fyp/demo/June6/6_wsmp.pcap"   // For wireshark packet capturing

// This struct use for final demo only. Not a part of stack
struct PacketData {
    unsigned char* payload;
    int payload_length;
};

typedef struct local_resp{
    uint8_t buf[WSMDATAMAXSIZE];
    size_t data_size;
} local_resp_t;

// This methods use for final demo only. Not a part of stack
void packet_callback(unsigned char* user_data, const struct pcap_pkthdr* pkthdr, const unsigned char* packet);
size_t capture_incoming_data(wave_pdu *pdu, int *err);

void slot_mutex_init();
void timer_cond_init();
void slot_mutex_destroy();
void timer_cond_destroy();

void *timer();
void *scheduler(void *arg);

int server_init(const char *sckfile);
void server_listen(int server_fd, mib_t *mib_db);

int wave_sock_init(const char *sckfile);

void hand_over_stack(local_req_t *req, mib_t *mib_db);
void broadcast_wsa(mib_t *mib_db);
void send_wsm(WSM_ReqTable_t *wsm_tb);
void *monitor_wsm_wsa(void *arg);

bool is_hundred_times(); // for demo only

#endif /* _CONTROLLER_H */
