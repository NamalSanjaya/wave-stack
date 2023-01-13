#ifndef _WAVE_LLC_SAP_H
#define _WAVE_LLC_SAP_H

#include "../../wave_sap.h"

typedef struct wave_llc_sap{
    char layer[10];
    wave_sap *sap;
} wave_llc_sap;

wave_llc_sap *init_wave_llc_sap(char *layer_name, char *filename);
void recv_incoming_data(wave_llc_sap *llc_sap);

#endif  /* _WAVE_LLC_SAP_H */
