/* WAVE Short Message Protocol - IEEE 1609
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

#include "../../include/1609_3/wsmp.h"
#include "../../include/1609_3/wave_llc.h"
#include "../../include/1609_3/wsmp_encode.h"
#include "../../include/1609_3/wave_llc.h"
#include "../../include/fmt_error.h"
#include "../../include/network.h"

void free_iex(struct wsmp_iex *curs) {
     if (curs == NULL)
	  return;

     if (curs->raw) {
	  int i = 0;

	  for (i = 0; i < curs->raw_count; i++) {
	       if (curs->raw[i].data)
		    free(curs->raw[i].data);
	  }

	  free(curs->raw);
     }

     free(curs);
}

void free_sii(struct wsmp_sii *curs) {
     if (curs == NULL)
	  return;

     free_iex(curs->iex);
     free(curs);
}

void free_cii(struct wsmp_cii *curs) {
     if (curs == NULL)
	  return;

     free_iex(curs->iex);
     free(curs);
}

void free_wra(struct wsmp_wra *curs) {
     if (curs == NULL)
	  return;

     free_iex(curs->iex);
     free(curs);
}

void free_wsa(struct wsmp_wsa *curs) {
     if (curs == NULL)
	  return;

     free_iex(curs->iex);

     if (curs->sis) {
	  int i = 0;
	  for (i = 0; i < curs->sii_count; i++)
	       free_sii(curs->sis[i]);

	  free(curs->sis);
     }

     if (curs->cis) {
	  int i = 0;
	  for (i = 0; i < curs->cii_count; i++)
	       free_cii(curs->cis[i]);

	  free(curs->cis);
     }

     free_wra(curs->wra);
     free(curs);
}

void free_wsm(struct wsmp_wsm *curs) {
     if (curs == NULL)
	  return;

     free_iex(curs->n_iex);
     free_iex(curs->t_iex);

     /* It is important to note that this data field is a serialized byte array, not a struct. */
     /* That is, one should not attempt to put a struct wsmp_wsa into this field. */
     /* Doing so introduces a safety critical error whereby an attacker can craft messages that leave orphaned blocks of memory, denying servce. */
     if (curs->data)
	  free(curs->data);

     free(curs);
}

uint32_t p_to_hex(const uint32_t psid, uint8_t *len) {
     uint32_t conv = 0;

     if (psid >= WSMP_P1_PMIN && psid <= WSMP_P1_PMAX) {
	  conv = psid;
	  *len = 1;
     } else if (psid >= WSMP_P2_PMIN && psid <= WSMP_P2_PMAX) {
	  conv = (psid ^ WSMP_P2_PXOR) + WSMP_P1_MAX+1;
	  *len = 2;
     } else if (psid >= WSMP_P3_PMIN && psid <= WSMP_P3_PMAX) {
	  conv = (psid ^ WSMP_P3_PXOR) + WSMP_P2_MAX+1;
	  *len = 3;
     } else if (psid >= WSMP_P4_PMIN && psid <= WSMP_P4_PMAX) {
	  conv = (psid ^ WSMP_P4_PXOR) + WSMP_P3_MAX+1;
	  *len = 4;
     } else {
	  conv = 0;
	  *len = 0;
     }

     return conv;
}

uint32_t hex_to_p(const uint32_t psid, uint8_t *len) {
     uint32_t conv;

     if (psid <= WSMP_P1_MAX) {
	  conv = psid;
	  *len = 1;
     } else if (psid <= WSMP_P2_MAX) {
	  conv = psid + WSMP_P2_OFF;
	  *len = 2;
     } else if (psid <= WSMP_P3_MAX) {
	  conv = psid + WSMP_P3_OFF;
	  *len = 3;
     } else if (psid <= WSMP_P4_MAX) {
	  conv = psid + WSMP_P4_OFF;
	  *len = 4;
     } else {
	  conv = 0;
	  *len = 0;
     }

     return conv;
}


uint8_t p_count(const uint8_t v) {
     if (v >> 7 == 0b0)
	  return 1;
     else if (v >> 6 == 0b10)
	  return 2;
     else if (v >> 5 == 0b110)
	  return 3;
     else if (v >> 4 == 0b1110)
	  return 4;

     return 0;
}

uint8_t c_count(uint8_t c) {
     if (c >> 7 == 0b0)
	  return 1;
     else if (c >> 6 == 0b10)
	  return 2;

     return 0;
}

void _g(const uint8_t *buf, size_t *i, uint8_t *v, size_t len, int *err) {
     if (*err) {
	  return;
     } else if (*i >= len) {
	  *err = -WSMP_EFAULT;
	  return;
     }

     *v = buf[*i];
     (*i)++;
}

void _gs(const uint8_t *buf, size_t *i, int8_t *v, size_t len, int *err) {
     if (*err) {
	  return;
     } else if (*i >= len) {
	  *err = -WSMP_EFAULT;
	  return;
     }

     *v = (int8_t) buf[*i];
     (*i)++;
}

void _g_n(const uint8_t *buf, size_t *i, size_t n, uint8_t *v, size_t len, int *err) {
     int j = 0;

     for (j = 0; j < n; j++)
	  _g(buf, i, &v[j], len, err);
}

void _g_p(const uint8_t *buf, size_t *i, uint32_t *v, size_t len, int *err) {
     uint8_t tmp = 0;
     uint8_t p_c = 0;
     uint32_t conv = 0;

     _g(buf, i, &tmp, len, err);
     p_c = p_count(tmp);

     if (p_c == 0) {
	  *err = -WSMP_EBADMSG;
	  return;
     }

     if (p_c == 1)
	  conv = tmp;
     else
	  conv |= tmp << ((p_c-1) * 8);

     int j = 0;

     for (j = p_c-1; j > 0; j--) {
	  _g(buf, i, &tmp, len, err);

	  if (j-1 == 0)
	       conv |= tmp;
	  else
	       conv |= tmp << ((j-1) * 8);
     }

     conv = p_to_hex(conv, &p_c);

     if (p_c == 0) {
	  *err = -WSMP_EBADMSG;
	  return;
     }

     *v = conv;
}

void _g_c(const uint8_t *buf, size_t *i, uint16_t *v, size_t len, int *err) {
     uint8_t c = 0;
     uint8_t tmp = 0;

     _g(buf, i, &c, len, err);

     tmp = c_count(c);

     if (tmp == 0) {
	  *err = -WSMP_EBADMSG;
	  return;
     }

     if (tmp == 1) {
	  *v = c;
     } else {
	  uint16_t conv = c & 0x3F;

	  _g(buf, i, &tmp, len, err);

	  conv <<= 8;
	  conv |= tmp;

	  *v = conv;
     }
}

void _s(uint8_t *buf, size_t *i, const uint8_t v, size_t len, int *err) {
     if (*err) {
	  return;
     } if (*i >= len) {
	  *err = WSMP_EPBUF;
	  return;
     }

     buf[*i] = v;
     (*i)++;
}

void _s_n(uint8_t *buf, size_t *i, size_t n, const uint8_t *v, size_t len, int *err) {
     for (int j = 0; j < n; j++)
	  _s(buf, i, v[j], len, err);
}

void _s_p(uint8_t *buf, size_t *i, const uint32_t v, size_t len, int *err) {
     uint8_t p_c = 0;
     uint32_t conv = hex_to_p(v, &p_c);

     if (p_c == 0) {
	  *err = -WSMP_EBADMSG;
	  return;
     }

     int j = 0;

     for (j = p_c; j > 0; j--) {
	  if (j-1 == 0)
	       _s(buf, i, conv & 0xFF, len, err);
	  else
	       _s(buf, i, (conv >> ((j-1) * 8)) & 0xFF, len, err);
     }
}

void _s_c(uint8_t *buf, size_t *i, const uint16_t v, size_t len, int *err) {
     if (v < 128) {
	  _s(buf, i, v, len, err);
     } else if (v <= WSMP_MAXCOUNT) {
	  uint16_t conv = v | 0x8000;
	  uint8_t cu = conv >> 8;
	  uint8_t cl = conv & 0xFF;

	  _s(buf, i, cu, len, err);
	  _s(buf, i, cl, len, err);
     } else {
	  *err = -WSMP_EBADMSG;
     }
}

struct wsmp_iex *create_n_iex(uint8_t chan_id, uint8_t data_rate, int8_t tx_pow){
     struct wsmp_iex *iex = calloc(1, sizeof(struct wsmp_iex));
     if(chan_id >= 0){
          iex->chan = chan_id;
          iex->count++;
          iex->use[WSMP_EID_CHANNEL_NUMBER_80211] = 1;
     };
     if(data_rate > 1){
          iex->data_rate = data_rate;
          iex->count++;
          iex->use[WSMP_EID_DATA_RATE_80211] = 1;
     };
     if(tx_pow != 0){
          iex->tx_pow = tx_pow;
          iex->count++;
          iex->use[WSMP_EID_TX_POWER_USED_80211] = 1;
     };
     return iex;
};

// TODO: log errors, warns etc, how to return error.
struct wsmp_wsm *create_wsmp_metadata(uint8_t subtype, uint8_t tpid, uint8_t info_elem_indicator, uint8_t chan_id, uint8_t data_rate, 
     int8_t tx_pow, uint32_t psid, uint16_t len, uint8_t *data){
     struct wsmp_wsm *wsm = calloc(1, sizeof(struct wsmp_wsm));

     wsm->subtype = subtype;
     wsm->version = WSMP_VERSION;
     wsm->tpid = tpid;  // T-Header Extension field(tpid), only support for tpid=0.

     // N-header option indicator can be 0,1. We always set to one.
     if (info_elem_indicator == 1){
          wsm->use_n_iex = 1;
          wsm->n_iex = create_n_iex(chan_id, data_rate, tx_pow);
     } else{
          wsm->use_n_iex = 0;
     }
     /** 
      * wsm->tpid = 2,3,4,5 are not needed support. No WAVE Information Elements are specified for tpid=1.
      * Therefore tpid=1 case will be neglected.
     */
     if (wsm->tpid == 0){
          wsm->use_t_iex = 0;
          wsm->psid = psid;
     }
     wsm->len = len;
     wsm->data = data;
     return wsm;
}

enum confirm_result_code wsm_waveshortmsg_req(uint8_t chan_id, enum time_slot timeslot, uint8_t data_rate, int8_t tx_power, uint8_t channel_load, 
     uint8_t info_elem_indicator, uint8_t prority, uint64_t wsm_expire_time, uint16_t len, uint8_t *data, uint8_t *peer_mac_addr, uint32_t psid){
     uint8_t subtype = 0, tpid=0; // currently only need to support for TPID=0
     struct wsmp_wsm *wsm_metadata = create_wsmp_metadata(subtype, tpid, info_elem_indicator, chan_id, data_rate, tx_power, psid, len, data);
     int err[1];
     wave_pdu *pdu = create_pdu_buf();
     // WSMP Header + WSM data <= WSM Max Length : this check is done within the wsm_encoding
     wsmp_wsm_encode(wsm_metadata, pdu, err, WSMP_STRICT);
     if(*err) {
          fmt_error(WAVE_WARN, "failed to encode wsm with err code");
          return rejected_unspecified;
     }
     // TODO: Resolve this logic
     // WSM data to the LLC sublayer
     char *device = get_network_device();
     uint8_t *src_macaddr = get_mac_addr(device);

     // uint8_t *src_macaddr = "1a:ff:ff:89:21:ca";
     uint8_t tx_pow_level = 14; // TODO: need to map to correct value (tx_power --> tx_pow_level)
     dl_unitdatax_req(pdu, src_macaddr, peer_mac_addr, prority, chan_id, timeslot, data_rate, tx_pow_level, channel_load, wsm_expire_time, err);
     // free_wsm(wsm_metadata);
     return accepted;
}

/**
 * When to use
 * 1. In WSA monitoring, once WSMP layer recieved a WSA it will indicate that to WME via this primitive.
 * 2. 
*/
void wsm_waveshortmsg_ind(uint8_t version, uint8_t channel_number, uint8_t data_rate, int8_t tx_power,
     uint8_t channel_load, uint8_t prority, uint16_t len, uint8_t *peer_mac_addr, uint32_t psid, wave_pdu *pdu){
     /**
      * For WSA - store data in MIB avaible table
      * For WSM - send the data to upper layer
     */
    
    

}
