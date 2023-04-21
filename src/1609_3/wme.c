#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/1609_3/wme.h"
#include "../../include/1609_3/wave_llc.h"
#include "../../include/1609_3/wsmp.h"
#include "../../include/1609_3/wme.h"

struct wsmp_iex *create_wsa_iex(uint8_t repeat_rate, bool use_loc2d,  uint32_t loc2d_latitude, uint32_t loc2d_longitude, bool use_loc3d,
    uint32_t loc3d_latitude, uint32_t loc3d_longitude, uint32_t loc3d_elevation, uint16_t advert_len, uint8_t *advert_id){
    struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
    iex->count = 0;
    if(repeat_rate != 0){
        iex->repeat_rate = repeat_rate;
        iex->count++;
        iex->use[WSMP_EID_REPEAT_RATE] = 1;
    }

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
            printf("warning msg");
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
            printf("log warning..\n");
        } else {
            iex->psc.len = psc_len;
            memcpy(iex->psc.psc, psc, psc_len);
            iex->count++;
            iex->use[WSMP_EID_PROVIDER_SERV_CONTEXT] = 1;
        }
    }

    if(ipaddr == NULL){
        memcpy(iex->ip, ipaddr, 16);
        iex->count++;
        iex->use[WSMP_EID_IPV6_ADDRESS] = 1;

        memcpy(iex->port, &port, 2)
        iex->count++;
        iex->use[WSMP_EID_SERVICE_PORT] = 1;
    }

    if(provider_mac == NULL) {
        memcpy(iex->mac, provider_mac, 6);
        iex->count++;
        iex->use[WSMP_EID_PROVIDER_MAC_ADDRESS] = 1;
    }

    if(use_rcpi_thres) {
        if(rcpi_thres < -110 || rcpi_thres > 0){
            // TODO: log warning msg
            printf("log warning..\n");
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
            printf("log warning..\n");
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

struct wsmp_iex *create_cii_iex(bool use_edca, uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo,
    bool use_chan_access, uint8_t chan_access){
    struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
    iex->count = 0;
    if(use_edca){
        memcpy(iex->edca.ac_be, ac_be, 4);
        memcpy(iex->edca.ac_bk, ac_bk, 4);
        memcpy(iex->edca.ac_vi, ac_vi, 4);
        memcpy(iex->edca.ac_vo, ac_vo, 4);
        iex->count++;
        iex->use[WSMP_EID_EDCA_PARAMETER_SET] = 1;
    }

    if(use_chan_access){
        if(chan_access > 2){
            // log warning msg
            printf("log warining msg\n");
        } else {
            iex->chan_access = chan_access;
            iex->count++;
            iex->use[WSMP_EID_CHANNEL_ACCESS] = 1;
        }
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
    uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, bool use_chan_access, uint8_t chan_access){
    struct wsmp_cii *cii = calloc(1, sizeof(struct wsmp_cii));
    // TODO: Add value checks
    cii->op_class = op_class;
    cii->chan = chan_no;
    cii->tx_pow = tx_pow;
    cii->adapt = adapt;
    cii->data_rate = data_rate;

    cii->iex = create_cii_iex(use_edca, ac_be, ac_bk, ac_vi, ac_vo, use_chan_access, chan_access);
    cii->use_iex = cii->iex == NULL ? false : true;
    return cii;
}

/** TODO:
 *  1.wra is not supported. This primitive is not completed.
 *  2.WSA is sent periodically. So, do we need to create WSA each time?. can't we do the creation only when WSA related    
 *    data updated on the MIB.
*/
struct wsmp_wsa *create_wsa_metadata(uint8_t wsa_id) {
    struct wsmp_wsa *wsa = calloc(1, sizeof(struct wsmp_wsa));
    wsa->version = WSMP_VERSION;
    if (wsa_id > 15){
        // TODO: log an error
        return NULL;
    }
    wsa->id = wsa_id;
    wsa->content_count = 0;  // TODO: This should be a global parameter. update whenever a change happen to WSA content. check extern in c
    
    // TODO: Advertiser Identifier need to be changed as required
    uint8_t *advert_id = "dummy-id";
    uint16_t advert_len = strlen(advert_id);

    uint8_t repeat_rate=0;  // repeat_rate=0 means it won't include in the WSA.
    uint32_t loc2d_latitude, loc2d_longitude, loc3d_latitude, loc3d_longitude, loc3d_elevation;
    bool use_loc2d = false, use_loc3d = false;

    // TODO: get_mib() function need to be implemented. different types of functions may need to get different metadata from MIB.
    repeat_rate = get_mib("repeat_rate-oid"); 

    // TODO: Need to figure out how to retrive, from where these 7 parameters can get.
    use_loc2d = true;
    loc2d_latitude = 1;
    loc2d_longitude = 1;

    use_loc3d = true;
    loc3d_latitude = 1;
    loc3d_longitude = 1;
    loc3d_elevation = 1;

    wsa->iex = create_wsa_iex(repeat_rate, use_loc2d, loc2d_latitude, loc2d_longitude, use_loc3d, loc3d_latitude, 
                loc3d_longitude, loc3d_elevation, advert_len, advert_id));
    wsa->use_iex = wsa->iex == NULL ? false : true;

    wsa->sii_count = get_no_of_ProviderServiceRequestTableEntries();  // get no.of entries in ProviderServiceRequestTable
    if (wsa->sii_count > 0) {
        wsa->sis = calloc(wsa->sii_count, sizeof(struct wsmp_sii));
        int err_instances = 0;
        // loop through each entry in ProviderServiceRequestTable
        // TODO: get each value from MIB from a Entry in ProviderServiceRequestTable
        for (int i = 0; i < wsa->sii_count; i++) {
            int err = 0;
            uint32_t psid = get_mib("psid-oid", &err);
            if(err){
                // TODO: log warn msg
                err_instances++;
                continue;
            }
            uint8_t chan_index = get_mib("channel_index-oid", &err);
            if(err){
                // TODO: log warn msg
                err_instances++;
                continue;
            }
            uint8_t *psc = get_mib("provider_service_context-oid", &err);
            uint16_t psc_len = 0;
            if(err){
                psc_len = strlen(psc);
            }
            uint8_t *ipaddr = get_mib("ipaddress-oid", &err);
            uint8_t *provider_mac = get_mib("provider_MAC_address-oid", &err);
            uint16_t port = get_mib("port-oid", &err);

            bool use_rcpi_thres = true;
            int8_t rcpi_thres = get_mib("rcpi_threshold-oid", &err);
            if(err){
                use_rcpi_thres = false;
            }

            bool use_count_thres = true;
            uint8_t count_thres = get_mib("count_threshold-oid", &err);
            if(err){
                use_count_thres = false;
            }

            bool use_count_thres_interv = true;
            uint8_t count_thres_interv = get_mib("count_threshold_intervel-oid", &err);
            if(err){
                use_count_thres_interv = false;
            }
            wsmp_sii *sis = create_wsa_sii(psid, chan_index, psc_len, psc, ipaddr, port, provider_mac, use_rcpi_thres, rcpi_thres, use_count_thres, 
                            count_thres, use_count_thres_interv, count_thres_interv);
            if(sis == NULL){
                err_instances++;
                continue;
            }
            wsa->sis[i] = sis;
        }
        wsa->sii_count = wsa->sii_count - err_instances;
    }

    wsa->cii_count = get_no_of_ProviderChannelInfoTableEntries();  // get no.of entries in ProviderChannelInfoTable
    if (wsa->cii_count > 0) {
        wsa->cis = calloc(wsa->cii_count, sizeof(struct wsmp_cii));
        int err_instances = 0;
        for (int j = 0; j < wsa->cii_count; j++) {
            int err = 0;
            uint8_t op_class = get_mib("operating_class-oid", &err);
            if(err){
                err_instances++;
                continue;
            }
            uint8_t chan_no = get_mib("channel_number-oid", &err);
            if(err){
                err_instances++;
                continue;
            }
            int8_t tx_pow = get_mib("transmit_power-oid", &err);
            if(err){
                err_instances++;
                continue;
            }
            uint8_t adapt = get_mib("adoptable-oid", &err);
            if(err){
                err_instances++;
                continue;
            }
            uint8_t data_rate = get_mib("data_rate-oid", &err);
            if(err){
                err_instances++;
                continue;
            }
            int err1 = 0, err2 = 0, err3 = 0;
            uint8_t *ac_be = get_mib("ac_be-oid", &err);
            uint8_t *ac_bk = get_mib("ac_bk-oid", &err1);
            uint8_t *ac_vi = get_mib("ac_vi-oid", &err2);
            uint8_t *ac_vo = get_mib("ac_vo-oid", &err3);
            bool use_edca = (err || err1 || er2 || err3) ? false : true;
        
            uint8_t chan_access = get_mib("channel_access-oid", &err);
            bool use_chan_access = (err == 0) ? true : false;

            struct wsmp_cii *cii = create_wsa_cii(op_class, chan_no, tx_pow, adapt, data_rate, use_edca, ac_be, ac_bk, ac_vi, ac_vo, use_chan_access ,chan_access);
            if(cii == NULL){
                err_instances++;
                continue;
            }
            wsa->cis[j] = cii;
        }
        wsa->cii_count = wsa->cii_count - err_instances;
    }
    // TODO: WRA part need to be added to WSA.
    return wsa;
}

void wme_provider_service_req(uint16_t local_service_index, enum action act, uint8_t *dest_mac_addr, enum wsa_type wsatype,
    uint32_t psid, uint32_t psctx, uint8_t sch_id, uint8_t wsa_chan_id, enum timeslot chan_access, uint8_t repeat_rate, 
    bool is_ip_service, uint8_t *ipv6_addr, uint16_t service_port, uint8_t *provider_mac_addr, int8_t rcpi_threshold, 
    uint8_t wsa_count_threshold, uint8_t wsa_count_thd_interval, uint8_t info_elements_indicator, uint16_t sign_lifetime){
    /**
     * 1. wme-set.req()  (providerServiceInfo)
     * 2. wme-set.req()  (serviceStatus)
     *  -- check Annex B. providerServiceInfo table entry. `serviceStatus` is just a one parameter there.
     * 3. MLMEX-CHSTART.req() (channel access assignment. This primitive in IEEE 1609.4)
     * 4. Initiate transmit WSA (check whether do we need this. Because send WSA)
    */ 

   /** TODO:
    * 1. see how the MIB storing should works
   */
}
