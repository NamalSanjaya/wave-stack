// ----------- test for 1st commit  ------------------

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include "../include/1609_3/wave_llc.h"
// #include "../include/wave_encode.h"
// #include "../include/pdu_buf.h"

// int main(){

//     wave_pdu *pdu = create_pdu_buf();
//     char *src_macaddr = "88:66:aa:bb:dd:cc";
//     char *des_macaddr = "11:66:aa:bb:dd:22";
//     uint8_t prority = 2;
//     uint8_t chan_id = 172;
//     enum time_slot tmslot = time_slot0;
//     uint8_t data_rate = 0x0C;
//     uint8_t txpwr_level = 5;
//     uint8_t chan_load = 1;
//     uint64_t wsm_exptime = 1000;

//     uint16_t count = 1;
//     uint8_t data = 122;

//     dl_unitdatax_req(pdu, src_macaddr, des_macaddr, prority, chan_id, tmslot, data_rate,
//     txpwr_level, chan_load, wsm_exptime, &data, count);

//     for (size_t i = 0; i < pdu->offset; i++){
//         printf("%x, ", *(pdu->current + i) );
//     }
//     show_pdu(pdu);
//     free_pbuf(pdu);
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

int main(){
    uint8_t subtype = 0, opt_indicator = 1, tpid=0, chat_id=172, data_rate=0x0C, channel_load=22;
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
    struct wsmp_wsm *wsm_metadata = create_wsmp_metadata(subtype, tpid, opt_indicator, chat_id, data_rate, tx_power, channel_load, psid, len, data);
    print_wsm(wsm_metadata);
}
