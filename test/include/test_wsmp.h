#ifndef _TEST_WSMP_H
#define _TEST_WSMP_H

#include "../../include/1609_3/wsmp.h"

void print_bin8(uint8_t value);
void print_binx(uint8_t *buf, size_t size);

void print_wsm(struct wsmp_wsm *wsm);
void print_sii(struct wsmp_sii *sii);
void print_cii(struct wsmp_cii *cii);
void print_wra(struct wsmp_wra *wra);
void print_wsa(struct wsmp_wsa *wsa);

struct wsmp_wsm *gen_wsm_metadata();
enum confirm_result_code gen_wsm_waveshortmsg_req();

#endif /* _TEST_WSMP_H */
