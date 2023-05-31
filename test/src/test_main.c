// ----------- test LLC encode & decode ------------------

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include "../../include/1609_3/wave_llc.h"
// #include "../../include/wave_encode.h"
// #include "../../include/pdu_buf.h"
// #include "../include/test_wsmp.h"

// int main(){
//     wave_pdu *pdu = create_pdu_buf();
//     uint8_t *src_macaddr = "88:66:aa:bb:dd:cc";
//     uint8_t *des_macaddr = "11:66:aa:bb:dd:22";
//     uint8_t prority = 2;
//     uint8_t chan_id = 172;
//     enum time_slot tmslot = time_slot0;
//     uint8_t data_rate = 0x0C;
//     uint8_t txpwr_level = 5;
//     uint8_t chan_load = 1;
//     uint64_t wsm_exptime = 1000;
//     int err_code = 0;
//     int *err = &err_code;
//     dl_unitdatax_req(pdu, src_macaddr, des_macaddr, prority, chan_id, tmslot, data_rate, txpwr_level, chan_load, wsm_exptime, err);
//     size_t llc_size = 8;
//     // print_binx(pdu->current, llc_size);
//     // show_pdu(pdu);

//     // test LLC decoding logic
//     uint16_t ethertype = 0x88DC; /* default to WSMP */
//     llc_pdu_metadata *llc_metadata = init_llc_pdu_metadata(ethertype);
//     llc_decode(llc_metadata, pdu, err);
//     print_llc_pdu_metadata(llc_metadata);
//     free_pbuf(pdu);
//     free_llc_pdu_metadata(llc_metadata);
//     printf("\n-- end --\n");
//     return 0;
// }

// -------------------------- end -----------------------

// #include <stdio.h>
// #include "../include/1609_3/llc/llc_sap.h"

// // ------ test for wave_sap, llc_sap -------
// int main(){
//     wave_llc_sap *sap = init_wave_llc_sap("test-llc", "sckfile");
//     recv_incoming_data(sap);
// }
// ------------------------- end ------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../include/1609_3/wsmp.h"
#include "../include/test_wsmp.h"
#include "../../include/1609_3/wsmp_encode.h"
#include "../../include/pdu_buf.h"
#include "../../include/1609_3/wsmp_decode.h"

int main(){
    uint8_t subtype = 0, opt_indicator = 1, tpid=0, chan_id=178, data_rate=0x0C;
    int8_t tx_power=0x9E;
    uint32_t psid = 0xC00305;
    uint16_t len = 11;
    uint8_t *data = calloc(1, sizeof(char));
    uint8_t msg[] = "hello-world";
    if (data==NULL){
        fprintf(stderr, "could not allocate memory");
        exit(1);
    }
    data = msg;
    struct wsmp_wsm *wsm_metadata = create_wsmp_metadata(subtype, tpid, opt_indicator, chan_id, data_rate, tx_power, psid, len, data);
    wave_pdu *pdu = create_pdu_buf();
    int err[1] ;
    wsmp_wsm_encode(wsm_metadata, pdu, err, WSMP_STRICT);
    if(*err) {
        fprintf(stderr, "went wrong in encoding %d", *err);
        exit(1);
    }
    struct wsmp_wsm *decoded_wsm = wsmp_wsm_decode(pdu, err, WSMP_STRICT);
    print_wsm(decoded_wsm);
    free_pbuf(pdu);
    free(decoded_wsm);
}

// --------------- writing wsmp+llc data  to tun interface ---------- 
// test for wsm_waveshortmsg request

// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <net/ethernet.h>
// #include <string.h>
// #include <linux/if_packet.h>
// #include <stdio.h>
// #include <unistd.h>

// #include "../../include/1609_3/wsmp.h"
// #include "../../include/pdu_buf.h"
// #include "../../include/1609_3/wave_llc.h"
// #include "../include/test_wsmp.h"

// int main(){
//     uint8_t info_element_indicator=1, chan_id=172, data_rate=0x0C, chan_load=1, prority = 2;
//     int8_t tx_power=0x9E;
//     uint64_t wsm_exptime = 1000;
//     enum time_slot tmslot = time_slot0;
//     uint8_t *peer_macaddr = 0x1166aabbdd22;
//     uint32_t psid = 0xC00305;
//     uint16_t len = 11;
//     uint8_t *data = calloc(len, sizeof(char));
//     char *msg = "hello-world";
//     if (data==NULL){
//         fprintf(stderr, "could not allocate memory\n");
//         exit(1);
//     }
//     memcpy(data, msg, len);
//     wave_pdu *rpdu = wsm_waveshortmsg_req(chan_id, tmslot, data_rate, tx_power, chan_load, info_element_indicator, prority, wsm_exptime, len, data, peer_macaddr, psid);
//     if (rpdu==NULL){
//         fprintf(stderr, "failed to generate WSM data\n");
//         exit(1);
//     }

//     // writing to tap interface
//     int sockfd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
//     int ifindex = 5;  // tun interface index

//     struct sockaddr_ll SendSockAddr;
//     SendSockAddr.sll_family   = AF_PACKET;
//     SendSockAddr.sll_halen    = ETH_ALEN;
//     SendSockAddr.sll_ifindex  = ifindex;
//     SendSockAddr.sll_protocol = htons(ETH_P_ALL);
//     SendSockAddr.sll_hatype   = 0;
//     SendSockAddr.sll_pkttype  = 0;

//     for (size_t i = 0; i < 1; i++){
//         ssize_t total =  sendto(sockfd, rpdu->current, rpdu->offset, 0, (struct sockaddr *) &SendSockAddr, sizeof(struct sockaddr_ll));
//         printf("sent : %ld\n", total);
//     }
//     close(sockfd);
//     free_pbuf(rpdu);
// }

// ------------------------ end --------------------------

// --------------- creating tun interface and write data to it -------------
// #include <stdio.h>
// #include <fcntl.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <linux/if_tun.h>

// int alloc_tun(char *dev);

// int main() {
//     int tun_fd;
//     char *tun_name = "tun0";
//     char buf[1500];

//     /* create the tun device */
//     if ((tun_fd = alloc_tun(tun_name)) < 0) {
//         fprintf(stderr, "Error creating TUN device\n");
//         return 1;
//     }
//     /* write to the tun device */
//     strcpy(buf, "Hello, world!");
//     if (write(tun_fd, buf, strlen(buf)) < 0) {
//         perror("write");
//         close(tun_fd);
//         return 1;
//     }
//     /* close the tun device */
//     close(tun_fd);
//     return 0;
// }

// int alloc_tun(char *dev) {
//     struct ifreq ifr;
//     int fd, err;

//     /* open the /dev/net/tun device */
//     if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
//         perror("open");
//         return -1;
//     }

//     /* configure the device */
//     memset(&ifr, 0, sizeof(ifr));
//     ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
//     strncpy(ifr.ifr_name, dev, IFNAMSIZ);

//     /* create the device */
//     if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
//         perror("ioctl");
//         close(fd);
//         return err;
//     }

//     /* return the file descriptor */
//     return fd;
// }

// -------------- end -------------------------