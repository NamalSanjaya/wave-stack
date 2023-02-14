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

#include "../include/test_wsmp.h"

void print_iex(struct wsmp_iex *iex) {
     printf("\n---- fields in wsmp_iex struct ----\n");
     printf("count:            %d\n", iex->count);
     printf("chan:             %d\n", iex->chan);
     printf("data rate:        %d\n", iex->data_rate);
     printf("tx_pow:           %d\n", iex->tx_pow);
     printf("psc.len:          %d\n", iex->psc.len);
     printf("psc.data:\n");

     int i;
     for (i = 0; i < iex->psc.len; i++)
          printf("%02x", iex->psc.psc[i]);

     printf("\n");
     printf("ip:               ");

     for (i = 0; i < 16; i++)
          printf("%02x", iex->ip[i]);

     printf("\n");
     printf("port:             %02x%02x\n",
            iex->port[0], iex->port[1]);
     printf("mac               %02x%02x%02x%02x%02x%02x\n",
            iex->mac[0], iex->mac[1], iex->mac[2],
            iex->mac[3], iex->mac[4], iex->mac[5]);
     printf("rcpi_thres:       %d\n", iex->rcpi_thres);
     printf("count_thres:      %d\n", iex->count_thres);
     printf("count_thres_int:  %d\n", iex->count_thres_int);
     printf("edca.ac_be:       %02x%02x%02x%02x\n",
            iex->edca.ac_be[0], iex->edca.ac_be[1],
            iex->edca.ac_be[2], iex->edca.ac_be[3]);
     printf("edca.ac_bk:       %02x%02x%02x%02x\n",
            iex->edca.ac_bk[0], iex->edca.ac_bk[1],
            iex->edca.ac_bk[2], iex->edca.ac_bk[3]);
     printf("edca.ac_vi:       %02x%02x%02x%02x\n",
            iex->edca.ac_vi[0], iex->edca.ac_vi[1],
            iex->edca.ac_vi[2], iex->edca.ac_vi[3]);
     printf("edca.ac_vo:       %02x%02x%02x%02x\n",
            iex->edca.ac_vo[0], iex->edca.ac_vo[1],
            iex->edca.ac_vo[2], iex->edca.ac_vo[3]);
     printf("chan_access:      %d\n", iex->chan_access);
     printf("repeat_rate:      %d\n", iex->repeat_rate);
     printf("loc_2d.latitude:  %02x%02x%02x%02x\n",
            iex->loc_2d.latitude[0], iex->loc_2d.latitude[1],
            iex->loc_2d.latitude[2], iex->loc_2d.latitude[3]);
     printf("loc_2d.longitude: %02x%02x%02x%02x\n",
            iex->loc_2d.longitude[0], iex->loc_2d.longitude[1],
            iex->loc_2d.longitude[2], iex->loc_2d.longitude[3]);
     printf("loc_3d.latitude:  %02x%02x%02x%02x\n",
            iex->loc_3d.latitude[0], iex->loc_3d.latitude[1],
            iex->loc_3d.latitude[2], iex->loc_3d.latitude[3]);
     printf("loc_3d.longitude: %02x%02x%02x%02x\n",
            iex->loc_3d.longitude[0], iex->loc_3d.longitude[1],
            iex->loc_3d.longitude[2], iex->loc_3d.longitude[3]);
     printf("loc_3d.elevation: %02x%02x\n", iex->loc_3d.elevation[0],
            iex->loc_3d.elevation[1]);
     printf("advert_id.len:    %d\n", iex->advert_id.len);

     for (i = 0; i < iex->advert_id.len; i++)
          printf("%02x", iex->advert_id.id[i]);

     printf("\n");
     printf("sec_dns:          ");

     for (i = 0; i < 16; i++)
          printf("%02x", iex->sec_dns[i]);

     printf("\n");
     printf("gateway_mac:      %02x%02x%02x%02x%02x%02x\n",
            iex->gateway_mac[0], iex->gateway_mac[1],
            iex->gateway_mac[2], iex->gateway_mac[3],
            iex->gateway_mac[4], iex->gateway_mac[5]);
     printf("\n");
     printf("raw_count:        %d\n", iex->raw_count);
     printf("raw:\n");

     int j;
     for (i = 0; i < iex->raw_count; i++) {
          printf("raw[%d].id:        %d\n", i, iex->raw[i].id);
          printf("raw[%d].len:       %d\n", i, iex->raw[i].len);
          printf("raw[%d].data:\n", i);

          for (j = 0; j < iex->raw[i].len; j++)
               printf("%02x ", iex->raw[i].data[j]);

          printf("\n");
     }

     printf("in_use:\n");
     for (i = 0; i < WSMP_EID_CHANNEL_LOAD+1; i++)
          printf("        (%d, %d)\n", i, iex->use[i]);

     printf("\n-- END IEX--\n");
}

void print_wsm(struct wsmp_wsm *wsm) {
     printf("\n---- fields in wsmp_wsm struct ----\n");
     printf("subtype:   %d\n", wsm->subtype);
     printf("version:   %d\n", wsm->version);
     printf("tpid:      %d\n", wsm->tpid);
     printf("use_n_iex: %d\n", wsm->use_n_iex);

     if (wsm->use_n_iex) {
          printf("n_iex:\n");
          print_iex(wsm->n_iex);
          printf("\n");
     }

     printf("psid:      %08x\n", wsm->psid);
     printf("use_t_iex: %d\n", wsm->use_t_iex);

     if (wsm->use_t_iex) {
          printf("t_iex:\n");
          print_iex(wsm->t_iex);
          printf("\n");
     }

     printf("length:    %d\n", wsm->len);
     printf("data:\n");

     int i;
     for (i = 0; i < wsm->len; i++)
          printf("%02x ", wsm->data[i]);

     printf("\n-- END WSM --\n");
}
