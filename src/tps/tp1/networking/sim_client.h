#include "../sim_common_reqs.h"

#ifndef _SIM_CLIENT_H_
#define _SIM_CLIENT_H_

#include "sim_message.h"
#include "sim_transporter.h"

typedef struct sim_client * sim_client;


/**
 * Starts the client from an existing transporter, it allows to make the construction more flexible. for example, for testing.
 */
sim_client sim_client_from_transporter(sim_transporter t, sim_receiver r);

/**
 * Starts the client with the parameters received, usually used after being fork'd/exec'd by a server.
 */
sim_client sim_client_init(connection_type c_type, process_type p_type, int server_id, int client_id, sim_receiver query_receiver);

// GET requests of params.

/**
 * Gets a distance between points, we should check if we must really use this or not.
 */
int sim_client_get_distance(sim_client c, int object_id, cstring from, cstring to);

/**
 * Gets all the medicines from a point.
 */
map sim_client_get_medicines(sim_client c, int object_id, cstring from);


// POST request of params.

/**
 * Fills a point with medicine.
 */
int sim_client_post_medicine_fill(sim_client c, int object_id, cstring city, cstring medicine, int amount);

/**
 * Sends a print message to a server.
 */
int sim_client_print(sim_client c, cstring message, int _id);

/**
 * Cleans the client, it's thread and it's data.
 */
void sim_client_free(sim_client c);


#endif