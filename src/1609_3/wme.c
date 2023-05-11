#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/1609_3/wme.h"
#include "../../include/1609_3/wave_llc.h"
#include "../../include/1609_3/wsmp.h"
#include "../../include/pdu_buf.h"
#include "../../include/1609_3/wsmp_encode.h"

struct wsmp_iex *create_wsa_iex(uint8_t repeat_rate, bool use_loc2d,  uint32_t loc2d_latitude, uint32_t loc2d_longitude, bool use_loc3d,
    uint32_t loc3d_latitude, uint32_t loc3d_longitude, uint32_t loc3d_elevation, uint16_t advert_len, uint8_t *advert_id){
    struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
    iex->count = 0;
   
    iex->repeat_rate = repeat_rate;
    iex->count++;
    iex->use[WSMP_EID_REPEAT_RATE] = 1;

    if(use_loc2d) {
        memcpy(iex->loc_2d.latitude, &loc2d_latitude, 4);
        memcpy(iex->loc_2d.longitude, &loc2d_longitude, 4);
        iex->count++;
        iex->use[WSMP_EID_2DLOCATION] = 1;
    }

    if(use_loc3d) {
        memcpy(iex->loc_3d.latitude, &loc3d_latitude, 4);
        memcpy(iex->loc_3d.longitude, &loc3d_longitude, 4);
        memcpy(iex->loc_3d.elevation, &loc3d_elevation, 2);
        iex->count++;
        iex->use[WSMP_EID_3DLOCATION] = 1;
    }

    if(advert_len != 0) {
        if(advert_len > 32){
            // TODO: log an warning and avoid encoding
            printf("incorrect Advertiser Identifier");
        } else {
            iex->advert_id.len = advert_len;
            iex->count++;
            iex->use[WSMP_EID_ADVERTISER_ID] = 1;

            for (int i = 0; i < advert_len; i++)
                iex->advert_id.id[i] = advert_id[i];
        }
    }
    if(iex->count == 0) {
        return NULL;
    }
    return iex;
}

struct wsmp_iex *create_sii_iex(uint16_t psc_len, uint8_t *psc, uint8_t *ipaddr, uint16_t port, uint8_t *provider_mac, bool use_rcpi_thres,
    int8_t rcpi_thres, bool use_count_thres, uint8_t count_thres, bool use_count_thres_interv, uint8_t count_thres_interv){
    struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
    iex->count = 0;
    if(psc_len != 0){
        if (psc_len > 31){
            // TODO: log warning msg fmt_error()
            printf("warning..too long provider service context\n");
        } else {
            iex->psc.len = psc_len;
            memcpy(iex->psc.psc, psc, psc_len);
            iex->count++;
            iex->use[WSMP_EID_PROVIDER_SERV_CONTEXT] = 1;
        }
    }

    if(strlen(ipaddr) != 0){
        memcpy(iex->ip, ipaddr, 16);
        iex->count++;
        iex->use[WSMP_EID_IPV6_ADDRESS] = 1;

        memcpy(iex->port, &port, 2);
        iex->count++;
        iex->use[WSMP_EID_SERVICE_PORT] = 1;
    }

    if(strlen(provider_mac) != 0) {
        memcpy(iex->mac, provider_mac, 6);
        iex->count++;
        iex->use[WSMP_EID_PROVIDER_MAC_ADDRESS] = 1;
    }

    if(use_rcpi_thres) {
        if(rcpi_thres < -110 || rcpi_thres > 0){
            // TODO: log warning msg
            printf("warning..rcpi_thres exceeded.\n");
        } else {
            iex->rcpi_thres = rcpi_thres;
            iex->count++;
            iex->use[WSMP_EID_RCPI_THRESHOLD] = 1;
        }
    }

    if(use_count_thres) {
        iex->count_thres = count_thres;
        iex->count++;
        iex->use[WSMP_EID_WSA_COUNT_THRESHOLD] = 1;
    }

    if(use_count_thres_interv){
        if(count_thres_interv == 0){
            // TODO: log warning msg
            printf("warning..count_thres_interv begin zero.\n");
        } else {
            iex->count_thres_int = count_thres_interv;
            iex->count++;
            iex->use[WSMP_EID_WSA_COUNT_THRES_INT] = 1;
        }
    }
    if(iex->count == 0){
        return NULL;
    }
    return iex;
}

struct wsmp_iex *create_cii_iex(bool use_edac, uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, uint8_t chan_access){
    struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
    iex->count = 0;
    if(use_edac){
        memcpy(iex->edca.ac_be, ac_be, 4);
        memcpy(iex->edca.ac_bk, ac_bk, 4);
        memcpy(iex->edca.ac_vi, ac_vi, 4);
        memcpy(iex->edca.ac_vo, ac_vo, 4);
        iex->count++;
        iex->use[WSMP_EID_EDCA_PARAMETER_SET] = 1;
    }

    if(chan_access > 2){
        // log warning msg
        printf("log warining msg\n");
    } else {
        iex->chan_access = chan_access;
        iex->count++;
        iex->use[WSMP_EID_CHANNEL_ACCESS] = 1;
    }
    
    if(iex->count == 0) {
        return NULL;
    }
    return iex;
}

struct wsmp_sii *create_wsa_sii(uint32_t psid, uint8_t chan_index, uint16_t psc_len, uint8_t *psc, uint8_t *ipaddr, uint16_t port, uint8_t *provider_mac, 
    bool use_rcpi_thres, int8_t rcpi_thres, bool use_count_thres, uint8_t count_thres, bool use_count_thres_interv, uint8_t count_thres_interv){
    struct wsmp_sii *sii = calloc(1, sizeof(struct wsmp_sii));
    // TODO: Add value checks
    sii->psid = psid;
    sii->chan_index = chan_index;
    sii->iex = create_sii_iex(psc_len, psc, ipaddr, port, provider_mac, use_rcpi_thres, rcpi_thres, use_count_thres, count_thres, 
                use_count_thres_interv, count_thres_interv);
    sii->use_iex = sii->iex == NULL ? false : true;
    return sii;
}

struct wsmp_cii *create_wsa_cii(uint8_t op_class, uint8_t chan_no, int8_t tx_pow, uint8_t adapt, uint8_t data_rate, bool use_edac,
    uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, uint8_t chan_access){
    struct wsmp_cii *cii = calloc(1, sizeof(struct wsmp_cii));
    // TODO: Add value checks
    cii->op_class = op_class;
    cii->chan = chan_no;
    cii->tx_pow = tx_pow;
    cii->adapt = adapt;
    cii->data_rate = data_rate;

    cii->iex = create_cii_iex(use_edac, ac_be, ac_bk, ac_vi, ac_vo, chan_access);
    cii->use_iex = cii->iex == NULL ? false : true;
    return cii;
}

/** TODO:
 *  1.wra is not supported. This primitive is not completed.
 *  2.WSA is sent periodically. So, do we need to create WSA each time?. can't we do the creation only when WSA related    
 *    data updated on the MIB.
 */
struct wsmp_wsa *create_wsa_metadata(uint8_t wsa_id, ProviderServiceRequestTable *provider_serv_tb, ProviderChannelInfoTable *provider_chan_tb) {
    struct wsmp_wsa *wsa = calloc(1, sizeof(struct wsmp_wsa));
    wsa->proto_version = WSMP_VERSION;
    wsa->wsaType = unsecured;
    wsa->version = WSMP_VERSION;
    if (wsa_id > 15){
        // TODO: log an error
        return NULL;
    }
    wsa->id = wsa_id;
    wsa->content_count = 0;  // TODO: This should be a global parameter. update whenever a change happen to WSA content. check extern in c
    
    // TODO: Advertiser Identifier need to be changed as required
    uint8_t advert_id[32] = "dummy-id";
    uint16_t advert_len = strlen(advert_id);

    uint32_t loc2d_latitude, loc2d_longitude, loc3d_latitude, loc3d_longitude, loc3d_elevation;
    bool use_loc2d = false, use_loc3d = false;

    ProviderServiceRequestTableEntry *entry = get_wme_prvtb(0, provider_serv_tb);
    if (entry == NULL) return NULL;

    // TODO: Need to figure out how to retrive, from where these 7 parameters can get.
    use_loc2d = true;
    loc2d_latitude = 1;
    loc2d_longitude = 1;

    use_loc3d = true;
    loc3d_latitude = 1;
    loc3d_longitude = 1;
    loc3d_elevation = 1;

    wsa->iex = create_wsa_iex(entry->ProviderRepeatRate, use_loc2d, loc2d_latitude, loc2d_longitude, use_loc3d, loc3d_latitude, 
                loc3d_longitude, loc3d_elevation, advert_len, advert_id);
    wsa->use_iex = wsa->iex == NULL ? false : true;

    wsa->sii_count = provider_serv_tb->size;
    if (wsa->sii_count > 0) {
        wsa->sis = calloc(wsa->sii_count, sizeof(struct wsmp_sii));
        int err_instances = 0;

        for (size_t i = 0; i < wsa->sii_count; i++) {
            entry = get_wme_prvtb(i, provider_serv_tb);

            // TODO: need to provide `ProviderIpService` parameter so that, we can say whether an IP or WSMP.
            // TODO: Need to figure out how to change these boolean values
            bool use_rcpi_thres = true;
            bool use_count_thres = true;
            bool use_count_thres_interv = true;

            struct wsmp_sii *sis = create_wsa_sii(
                            entry->ProviderServiceIdentifier, 
                            get_chan_index(entry->ProviderServiceChannelNumber, provider_chan_tb), 
                            wme_prvtb_psc_len(entry->ProviderServiceContext), entry->ProviderServiceContext, 
                            entry->ProviderIpv6Address, 
                            entry->ProviderServicePort, 
                            entry->ProviderMacAddress,
                            use_rcpi_thres, entry->ProviderRcpiThreshold, 
                            use_count_thres, entry->ProviderWsaCountThreshold, 
                            use_count_thres_interv, entry->ProviderWsaCountThresholdInterval);
            if(sis == NULL){
                err_instances++;
                continue;
            }
            wsa->sis[i] = sis;
        }
        wsa->sii_count = wsa->sii_count - err_instances;
    }

    wsa->cii_count = provider_chan_tb->size;
    if (wsa->cii_count > 0) {
        wsa->cis = calloc(wsa->cii_count, sizeof(struct wsmp_cii));
        int err_instances = 0;
        ProviderChannelInfoTableEntry *chan_entry;

        for (int j = 0; j < wsa->cii_count; j++) {
            chan_entry = get_wme_prv_chan_entry(j, provider_chan_tb);
            uint8_t adapt = chan_entry->ProviderChannelInfoAdaptable ? 1 : 0;
            
            // TODO: These are just test values. Need to change accordingly.
            uint8_t ca_be[4] = "be";
            uint8_t ca_bk[4] = "bk";
            uint8_t ca_vi[4] = "vi";
            uint8_t ca_vo[4] = "vo";

            bool use_edca = true;  // TODO: Need to change.
        
            uint8_t chan_access = alternatingTimeslot1Only;   // we only support Provider to associate other devices during time slot 1.

            struct wsmp_cii *cii = create_wsa_cii(
                chan_entry->ProviderChannelInfoOperatingClass, 
                chan_entry->ProviderChannelInfoChannelNumber, 
                chan_entry->ProviderChannelInfoTransmitPowerLevel, adapt,
                chan_entry->ProviderChannelInfoDataRate, 
                use_edca, ca_be, ca_bk, ca_vi, ca_vo ,chan_access);
            if(cii == NULL){
                err_instances++;
                continue;
            }
            wsa->cis[j] = cii;
        }
        wsa->cii_count = wsa->cii_count - err_instances;
    }

    // TODO: Since we are not supporting WRA, it is dropped from implementation. WRA part need to be supported in the future.
    wsa->use_wra = false;

    return wsa;
}

enum wme_service_confirm wme_provider_service_req(uint16_t local_service_index, enum action act, uint8_t *dest_mac_addr, enum wsa_type wsatype,
    uint32_t psid, uint8_t *psc, uint8_t sch_id, uint8_t wsa_chan_id, enum time_slot chan_access, uint8_t repeat_rate, 
    bool ip_service, uint8_t *ipv6_addr, uint16_t service_port, uint8_t *provider_mac_addr, int8_t rcpi_threshold, 
    uint8_t wsa_count_threshold, uint8_t wsa_count_thd_interval, uint8_t info_elements_indicator, uint16_t sign_lifetime, mib_t *mib_db){
    /**
     * ----- ACTION == Add -------
     * 1. wme-set.req()  (providerServiceInfo) - done
     * 2. wme-set.req()  (serviceStatus) - done
     *  -- check Annex B. providerServiceInfo table entry. `serviceStatus` is just a one parameter there.
     * 3. MLMEX-CHSTART.req() (channel access assignment. This primitive in IEEE 1609.4)
     * 4. Create and store WSA under WSA Identifier. (when time slot 0 comes it will get from DB and sent over the air) - done
     */ 

    if(act == add){
        bool best_available = false;                   // Best Available option is set to `false` in our implementation.
        enum service_status serv_status = pending;
        if(IS_NOT_VALID_CHANNEL(sch_id)){
            sch_id = find_suitable_channel();                   // Assign a suitable channel
        }
        if(IS_NOT_VALID_CHANNEL(wsa_chan_id)){
            wsa_chan_id = DEFAULT_CCH;
        }
        wme_prvtb_add(wsatype, psid, psc, chan_access, best_available, OPERATING_CLASS, sch_id, wsa_chan_id, repeat_rate, ip_service, ipv6_addr,
            provider_mac_addr, service_port, rcpi_threshold, wsa_count_threshold, wsa_count_thd_interval, serv_status, mib_db->psrtb);
        
        uint8_t wsa_id = 4;
        int err[1];

        wave_pdu *pdu = create_pdu_buf();
        struct wsmp_wsa *wsa_metadata = create_wsa_metadata(wsa_id, mib_db->psrtb, mib_db->pcitb);
        wsmp_wsa_encode(wsa_metadata, pdu, err, WSMP_STRICT);
        if(*err) {
            printf("unable to encode WSA. error: %d", *err);
            return rejected_unspecified;
        }
        (mib_db->pdutb)->wsa_store[wsa_id] = pdu;
    }

    // else if(act == change){}
    // else if(act == delete){}
    return Accepted;
}

// TODO: Need to implement the channel assignment algorithm to pick a good channel for applicatioin service.
// Need to design algorithm, metadata etc
uint8_t find_suitable_channel(){
    return 172;
}

enum wme_service_confirm wme_user_service_req(uint8_t local_service_indx, enum action act, enum user_request_type user_req_type, uint8_t *psid, uint8_t *psc,
    enum wsa_type wsatype, uint8_t channel_id, uint8_t *src_mac_addr, uint8_t *advert_id, uint8_t link_quality, uint8_t immediate_access,
    UserServiceRequestTable *user_serv_tb){
    /**
     * 1. population of a corresponding UserServiceRequestTableEntry in the MIB - done
     * 2. consideration of the service request in determining channel access assignments
     * 3. generates a WME-UserService.confirm indicating whether the request is accepted - return value from the function
     */
    uint8_t priority = 0;
    uint8_t op_class = OPERATING_CLASS;
    enum service_status serv_status = pending;
    if(act == add){
        add_wme_user_serv_req_tb(user_req_type, psid, psc, priority, wsatype, src_mac_addr, advert_id, op_class, channel_id, link_quality, 
        immediate_access, serv_status, user_serv_tb);
    }
    // else if(act == delete){}
    return Accepted;
}
