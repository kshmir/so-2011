#include "../sim_common_reqs.h"

#ifndef _SIM_MESSAGE_H_
#define _SIM_MESSAGE_H_

typedef struct sim_message * sim_message;

typedef void (*sim_receiver)(sim_message data);

#ifndef _SIM_TRANSPORTER_H_
#include "sim_server.h"
#include "sim_client.h"
#endif

#include "sim_transporter_internal.h"

sim_message sim_message_init(sim_transporter t, cstring header, cstring request);

sim_message sim_message_request_and_response(sim_transporter t, cstring header, cstring request);

cstring sim_message_read(sim_message r);

sim_message sim_message_send(sim_message r);

void sim_message_respond(sim_message r);

int sim_message_write(sim_message r, cstring data);

int sim_message_free(sim_message r);

#endif