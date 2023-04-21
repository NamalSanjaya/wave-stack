#ifndef _WME_H
#define _WME_H

#include <stdint.h>
#include <stdbool.h>

enum action {
    add, delete, change
};

enum wsa_type {
    unsecured, secured
};

struct wsmp_iex *create_wsa_iex(uint8_t repeat_rate, bool use_loc2d,  uint32_t loc2d_latitude, uint32_t loc2d_longitude, bool use_loc3d,
    uint32_t loc3d_latitude, uint32_t loc3d_longitude, uint32_t loc3d_elevation, uint16_t advert_len, uint8_t *advert_id);

struct wsmp_iex *create_sii_iex(uint16_t psc_len, uint8_t *psc, uint8_t *ipaddr, uint16_t port, uint8_t *provider_mac, bool use_rcpi_thres,
    int8_t rcpi_thres, bool use_count_thres, uint8_t count_thres, bool use_count_thres_interv, uint8_t count_thres_interv);

struct wsmp_iex *create_cii_iex(bool use_edca, uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, bool use_chan_access, uint8_t chan_access);
struct wsmp_sii *create_wsa_sii(uint32_t psid, uint8_t chan_index, uint16_t psc_len, uint8_t *psc, uint8_t *ipaddr, uint16_t port, uint8_t *provider_mac, 
    bool use_rcpi_thres, int8_t rcpi_thres, bool use_count_thres, uint8_t count_thres, bool use_count_thres_interv, uint8_t count_thres_interv);

struct wsmp_cii *create_wsa_cii(uint8_t op_class, uint8_t chan_no, int8_t tx_pow, uint8_t adapt, uint8_t data_rate, bool use_edac,
    uint8_t *ac_be, uint8_t *ac_bk, uint8_t *ac_vi, uint8_t *ac_vo, bool use_chan_access, uint8_t chan_access);

struct wsmp_wsa *create_wsa_metadata(uint8_t wsa_id);

#endif /* _WME_H */
