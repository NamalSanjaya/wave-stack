/** WAVE Short Message Protocol - IEEE 1609
 * Copyright (C) 2017 Nicholas Haltmeyer <ginsback@protonmail.com>,
 *                    Duncan Woodbury    <p3n3troot0r@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
*/

#include <pcap.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include "../../include/pdu_buf.h"
#include "../../include/1609_3/wsmp.h"
#include "../../include/1609_3/wsmp_encode.h"
#include "../include/test_wsmp.h"

#define WSMP_MODE WSMP_STRICT

pcap_t *wsmp_if_init(const char *ifname) {
     char pcap_errbuf[PCAP_ERRBUF_SIZE];
     pcap_errbuf[0] = '\0';

     pcap_t *pcap = pcap_open_live(ifname, 96, 0, 0, pcap_errbuf);

     if (pcap_errbuf[0] != '\0') {
	  fprintf(stderr, "ERROR:\twsmp_if_init | pcap_errbuf[0] == %s\n", pcap_errbuf);
     }

     if (!pcap) {
	  return NULL;
     }

     return pcap;
}

int wsmp_if_send(wave_pdu *pdu, const char *ifname, pcap_t *pcap) {
     struct ether_header hdr;
     hdr.ether_type = htons(0x88DC);
     memset(hdr.ether_dhost, 0xFF, sizeof(hdr.ether_dhost));

     struct ifreq ifr;
     size_t ifname_len = strlen(ifname);

     if (ifname_len < sizeof(ifr.ifr_name)) {
	  memcpy(ifr.ifr_name, ifname, ifname_len);
	  ifr.ifr_name[ifname_len] = 0;
     } else {
	  fprintf(stderr, "ERROR:\twsmp_if_send | !(ifname_len < sizeof(ifr.ifr_name))\n");
	  return EXIT_FAILURE;
     }

     int fd = socket(AF_INET, SOCK_DGRAM, 0);

     if (fd == -1) {
	  perror(0);
	  return EXIT_FAILURE;
     }

     if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
	  perror(0);
	  close(fd);
	  return EXIT_FAILURE;
     }

     // ARPHRD_ETHER := 1
     if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
	  fprintf(stderr, "ERROR:\twsmp_if_send | ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER\n");
	  close(fd);
	  return EXIT_FAILURE;
     }

     const unsigned char *src_mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;
     memcpy(hdr.ether_shost, src_mac, sizeof(hdr.ether_shost));
     close(fd);

     unsigned char frame[sizeof(struct ether_header) + pdu->offset];
     memcpy(frame, &hdr, sizeof(struct ether_header));
     memcpy(frame + sizeof(struct ether_header), pdu->current, pdu->offset);

     uint8_t frame2[pdu->offset];
     memcpy(frame2, pdu->current, pdu->offset);

     if (pcap_inject(pcap, frame2, sizeof(frame2)) == -1) { // BREAK HERE
	  pcap_perror(pcap, 0);
	  return EXIT_FAILURE;
     }
     return EXIT_SUCCESS;
}

int main() {
     char *device = "wlp0s20f3";
     pcap_t *pcap = wsmp_if_init(device);

     if (pcap == NULL) {
	     fprintf(stderr, "Failed to open pcap interface.\n");
	     return EXIT_FAILURE;
     }

     wave_pdu *pdu = gen_wsm_waveshortmsg_req();
     if (pdu==NULL){
          fprintf(stderr, "Failed to serve the wsm creation request.\n");
	     return EXIT_FAILURE;
     }

     int i;
     for (i = 0; i < 100; i++) {
	     printf("%d\n", wsmp_if_send(pdu, device, pcap));
     }

     free_pbuf(pdu);
     pcap_close(pcap);

     return EXIT_SUCCESS;
}
