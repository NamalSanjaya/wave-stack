#ifndef _WAVE_NETWORK_H
#define _WAVE_NETWORK_H

// what if multiple network devices are availble.
char *get_network_device();
uint8_t *get_mac_addr(char *device);

#endif /* _WAVE_NETWORK_H */
