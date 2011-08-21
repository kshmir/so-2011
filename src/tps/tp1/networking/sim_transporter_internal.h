#include "sim_transporter.h"

#ifndef _SIM_TRANSPORTER_INTERNAL_H_
#define _SIM_TRANSPORTER_INTERNAL_H_


void sim_transporter_write(sim_transporter sim, cstring data);

cstring sim_transporter_listen(sim_transporter sim);

#endif