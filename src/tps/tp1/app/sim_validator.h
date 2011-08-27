#include "../sim_common_reqs.h"
#include "../networking/sim_transporter.h"
#ifndef _SIM_FRONTEND_H_
#define _SIM_FRONTEND_H_

int sim_validator_params(list args, cstring * return_level, list return_airline, cstring * error, connection_type * c_type);

#endif