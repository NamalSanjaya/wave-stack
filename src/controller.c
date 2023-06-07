#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pcap.h>

#include "../include/controller.h"
#include "../include/1609_3/wme.h"
#include "../include/pdu_buf.h"
#include "../include/fmt_error.h"
#include "../include/1609_3/wsmp.h"
#include "../include/1609_3/wave_llc.h"

int slot = 0;

pthread_mutex_t mutex_slot;
pthread_cond_t chan_timer;

void slot_mutex_init(){
    pthread_mutex_init(&mutex_slot, NULL);
}

void timer_cond_init(){
    pthread_cond_init(&chan_timer, NULL);
}

void slot_mutex_destroy(){
    pthread_mutex_destroy(&mutex_slot);
}

void timer_cond_destroy(){
    pthread_cond_destroy(&chan_timer);
}

void *timer(){
    while (1){
        usleep(CH_INTERVAL);
        pthread_cond_broadcast(&chan_timer);
    }
}

void *scheduler(void *arg){
    pthread_t monitor_th;
    mib_t *mib_db = (mib_t *) arg;

    // Loading default channel configurations
    uint8_t op_class = 10;
    uint8_t chan_no = 172;
    bool is_adaptable = true;
    uint8_t data_rate = 23;
    int8_t tx_power_level = 18;

    uint8_t be_cwmin = 2;
    uint32_t be_cwmax = 2784;
    uint8_t be_aifsn = 202;
    uint32_t be_txop_limit = 10001;
    bool be_mand = true;

    uint8_t bk_cwmin = 1;
    uint32_t bk_cwmax = 1340;
    uint8_t bk_aifsn = 201;
    uint32_t bk_txop_limit = 10002;
    bool bk_mand = false;

    uint8_t vi_cwmin = 3;
    uint32_t vi_cwmax = 1026;
    uint8_t vi_aifsn = 203;
    uint32_t vi_txop_limit = 10003;
    bool vi_mand = false;

    uint8_t vo_cwmin = 4;
    uint32_t vo_cwmax = 1587;
    uint8_t vo_aifsn = 204;
    uint32_t vo_txop_limit = 10004;
    bool vo_mand = true;

    add_wme_prv_chan_tb(op_class, chan_no, is_adaptable, data_rate, tx_power_level, 
        be_cwmin, be_cwmax, be_aifsn, be_txop_limit, be_mand,
        bk_cwmin, bk_cwmax, bk_aifsn, bk_txop_limit, bk_mand,
        vi_cwmin, vi_cwmax, vi_aifsn, vi_txop_limit, vi_mand,
        vo_cwmin, vo_cwmax, vo_aifsn, vo_txop_limit, vo_mand, mib_db->pcitb);

    if(pthread_create(&monitor_th, NULL, &monitor_wsm_wsa, (void *)mib_db) != 0) return NULL;

    pthread_mutex_lock(&mutex_slot);
    while(1) {
        if(slot == 0){
            printf("---------------------------------------------------------------\n");
            printf("time slot 0: TX: BroadcastWSA()  | RX: MonitorWSA()\n");
            // broadcast_wsa(mib_db);
            // Check whether new WSA is available in UserAvailableService table
            if( (mib_db->uastb)->unprocessed_servs > 0 ){
                fmt_info("Working on unprocessed requests in UserAvailableService table.");
                for (size_t i = 0; i < (mib_db->usrtb)->size; i++){
                    UserAvailableServiceTableEntry_t uast_entry = ( (mib_db->uastb)->table)[i];
                    for (size_t j = 0; j < (mib_db->usrtb)->size; j++){
                        UserServiceRequestTableEntry usrtb_entry = ((mib_db->usrtb)->table)[j];
                        // Only rely on PSID.
                        if(uast_entry.UserAvailableProviderServiceIdentifier == usrtb_entry.UserServiceRequestProviderServiceIdentifier){
                            /**
                             * Application interest found in WSA. 
                             * 1. Switch to relavent channel. (issue MLMEX-CHSTART.req)
                             * 2. Change Service Status (issue WME-set.req())
                             * 3. In time slot 1, need to listen to relavent data.
                            */
                            printf("Application interest found in WSA with psid %0x", usrtb_entry.UserServiceRequestProviderServiceIdentifier);
                        }
                    }
                }
            }

            /**
             * --------  Monitoring WSA ----------
             * UserServiceRequest Table has application services that are interested by the device Higher layers.
             * So while monitoring, if we get a match, that need to be informed to respective Higher layer.
             * WSMServiceRequest is issued to inform WME about app-service match.
             * 
            */
        } 
        else if(slot == 1) {
            printf("---------------------------------------------------------------\n");
            // printf("time slot 0: TX: BroadcastWSA()  | RX: MonitorWSA()\n");
            printf("time slot 1: TX: SendActualWSM() | RX: ListenToIncomingWSM()\n");
            // send_wsm(mib_db->wrtb);
            /**
             * -------- Send Actual WSM --------
             * WSA has required information about WSM.
             * So, during time slot 1, switch to some channel and can send the actual WSM data.
             * In the actual sending data from MIB or internal data strcture, we will send the oldest request and discard 
             * its content and all other its data. 
             * 
            */
        }
        slot = slot == 0 ? 1 : 0;
        pthread_cond_wait(&chan_timer, &mutex_slot);
        // usleep(4000);
    }
    pthread_mutex_unlock(&mutex_slot);
    return NULL;
}

int server_init(const char *sckfile){
    int server_fd;
    struct sockaddr_un server_addr;

    // Create a socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) return -1;

    // Bind the socket to a name
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, sckfile, sizeof(server_addr.sun_path) - 1);
    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) return -1;

    // Listen for connections. can queue upto 32 requests
    if (listen(server_fd, 32) == -1) return -1;
    printf("listen....\n");
    return server_fd;
}

void server_listen(int server_fd, mib_t *mib_db){
    int client_fd, len;
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;
    local_req_t req;

    while(1){
        // Accept a connection
        client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len);
        if (client_fd == -1) {
            printf("connection failed...\n");
            continue;
        }

        len = recv(client_fd, &req, sizeof(req), 0);
        if (len == -1) {
            printf("receiving error...\n");
            continue;
        }
        hand_over_stack(&req, mib_db);
        close(client_fd);
    }
}

void hand_over_stack(local_req_t *req, mib_t *mib_db){

    uint16_t local_service_index = 1;     // TODO: Parameters which are not finialized
    uint8_t dest_mac_addr[6] = {255, 255, 255, 255, 255, 255}; // Brodcast address
    uint8_t sch_id = 0;   // Let the stack to select a channel
    uint8_t repeat_rate = 10;  // TODO: Need to be calculated
    uint8_t provider_mac_addr[6] = {11,12,13,14,15,16};   // TODO: Put right device MAC address
    uint8_t info_elements_indicator = 0;
    uint16_t sign_lifetime = 0;

    if(req->id == 1){
        // Store the WSM related data in WSM_Req_t
        app_WSM_Req_t wsmr = req->wsmr;
        add_wsm_req_tb(wsmr.chan_id, wsmr.timeslot, wsmr.data_rate, wsmr.tx_power, wsmr.channel_load, wsmr.info_elem_indicator, wsmr.prority, wsmr.wsm_expire_time,
            wsmr.len, wsmr.data, wsmr.peer_mac_addr, wsmr.psid, mib_db->wrtb);
        return;
    }

    if(req->id == 11){
        app_ProviderServiceReqEntry psre = req->psre;
        printf("Start working on provider service request with psid = %0x.\n", psre.psid);
        wme_provider_service_req(local_service_index, psre.act, dest_mac_addr, psre.wsatype, psre.psid, psre.psc, sch_id,
            DEFAULT_CCH, psre.chan_access, repeat_rate, psre.ip_service, psre.ipv6_addr, psre.service_port, provider_mac_addr,
            psre.rcpi_threshold, psre.wsa_count_threshold, psre.wsa_count_thd_interval, info_elements_indicator, sign_lifetime, mib_db);
        // TODO: WME-Notification.indication()
        return;
    }

    if (req->id == 13){
        // Upper layer is interested on some application service. This info will store in UserServiceReq table.
        app_UserServiceReqEntry_t usre = req->usre;
        wme_user_service_req(local_service_index, usre.act, usre.user_req_type, usre.psid, usre.psc, usre.wsatype, usre.channel_id, usre.src_mac_addr, usre.advertiser_id, usre.link_quality, usre.immediate_access, mib_db->usrtb);
        return;
    }
}

void broadcast_wsa(mib_t *mib_db){

    /** TODO: 
     * 1. MLMEX-CHSTART.req() (channel access assignment. This primitive in IEEE 1609.4)
     * 2. Issue WSM_WaveShortMessage.Req() - done
     */ 
    uint8_t wsa_id = 4;
    wave_pdu *wsa = get_pdu(wsa_id, mib_db->pdutb);
    if(wsa == NULL){
        return;
    }
    uint32_t psid = WSA_PSID;    
    uint8_t peer_mac_addr[6] = {255, 255, 255, 255, 255, 255};
    wsm_waveshortmsg_req(0, time_slot0, 0, 0, 0, 0, 0, 0, wsa->offset, wsa->current, peer_mac_addr, psid);
}

void send_wsm(WSM_ReqTable_t *wsm_tb){
    int err[1];
    WSM_Req_t wsmr = get_nxt_wsm_req(wsm_tb, err);
    if(*err) return;
    wsm_waveshortmsg_req(wsmr.chan_id, wsmr.timeslot, wsmr.data_rate, wsmr.tx_power, wsmr.channel_load, wsmr.info_elem_indicator, wsmr.prority, wsmr.wsm_expire_time,
        wsmr.len, wsmr.data, wsmr.peer_mac_addr, wsmr.psid);
}

void *monitor_wsm_wsa(void *arg){
    /**
     * Steps (Listening to Air)
     * 1. WSM-WaveShortMessage.ind - when WSMP get a WSA then it indicate it to WME.
     * 2. Set UserAvailableService Info in MIB.
     * Note: 
     * Schduler need to detect a matching in UserServiceRequest table and UserAvailable table.
     * If there is a match, we tune to that SCH channel in time slot 1, and listen to that data.
     * 
     */
    mib_t *mib_db = (mib_t *) arg;

    while(1){
        wave_pdu *pdu = create_pdu_buf();
        int err[1];
        size_t total = capture_incoming_data(pdu, err);  // only for demo
        if(*err){
            printf("unable to capture data.\n");
            continue;
        }
        // printf("Pcap read: %ld\n", total);
        if(total == 0) continue;
        dl_recv(pdu,  mib_db->uastb, err);
        usleep(100000);
    }
    
    return NULL;
}

size_t capture_incoming_data(wave_pdu *pdu, int *err){
    pcap_t* handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct PacketData packet_data;
    *err = 0;
    handle = pcap_open_offline(PCAPFILE, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error : %s\n", errbuf);
        *err = 1;
        return 0;
    }

    packet_data.payload = NULL;
    pcap_dispatch(handle, -1, packet_callback, (unsigned char*)&packet_data);

    // To remove IEEE 802.11p part
    size_t llc_start_indx = 41; 
    size_t sent_upper = 0;
    if (packet_data.payload != NULL) {  
        sent_upper = packet_data.payload_length-llc_start_indx;      
        add_data_to_pbuf(pdu, packet_data.payload+llc_start_indx, sent_upper, err);
        free(packet_data.payload);
        
    } else {
        printf("No packets found.\n");
        return 0;
    }
    pcap_close(handle);
    return sent_upper;
}

void packet_callback(unsigned char* user_data, const struct pcap_pkthdr* pkthdr, const unsigned char* packet) {
    struct PacketData* packet_data = (struct PacketData*)user_data;
    packet_data->payload = (unsigned char*)malloc(pkthdr->caplen);
    memcpy(packet_data->payload, packet, pkthdr->caplen);
    packet_data->payload_length = pkthdr->caplen;
}

int wave_sock_init(const char *sckfile){
    int socket_fd;
    struct sockaddr_un name;

    // Create a socket
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) return -1;

    // Connect to the server socket
    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, sckfile, sizeof(name.sun_path) - 1);
    if (connect(socket_fd, (struct sockaddr*) &name, sizeof(name)) == -1) return -1;
    return socket_fd;
}

// check whether time is in times of 100 milliseconds.
bool is_hundred_times(){
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);

    long milliseconds = currentTime.tv_sec * 1000 + currentTime.tv_nsec / 1000000;
    if (milliseconds % 100 < 15){
        return true;
    }
    return false;
}
