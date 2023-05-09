#ifndef _WAVE_NETWORK_H
#define _WAVE_NETWORK_H

#include "pdu_buf.h"

char *get_network_device();           // what if multiple network devices are availble.
uint8_t *get_mac_addr(char *device);
void write_tun(wave_pdu *pdu);

#endif /* _WAVE_NETWORK_H */
