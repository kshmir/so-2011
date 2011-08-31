#include "sim_level.h"

#include <time.h>

static sim_level current_level = NULL;
static list		 airlines = NULL;

struct sim_level {
	// level_distances
	//	stores cstrings  as: keys	# Name of the city
	//  stores map		 as: values # Hash of medicines.
	////// This map stores
	////// cstrings as: keys		# Name of medicine
	////// int *    as: values		# Amount of medicine.
	// stores distances	 as: arcs   # Distance between cities
	graph level;
	
	map	  plane_location;
	map	  plane_distance;
	
		
	sim_client frontend_client;
	sim_server airlines_server;
	
	int	  level_id;
	
	int	  turn;
	
	
	int	  frontend_sem;
	int	  airline_sem;
	int	  level_sem;
	
	
};

graph sim_level_graph(sim_level lev) {
	return lev->level;
}

int sim_level_has_city(sim_level lev, cstring city) {
	return graph_get_node(lev->level, city) != NULL;
}

cstring sim_level_serialize(sim_level level) {
	int n = list_size(graph_keys(level->level));
	cstring s = cstring_fromInt(n);
    s =  cstring_write(s, "\n\n");
	int i = 0, j = 0;
	for (i=0; i<n; i++) {
		cstring city = list_get(graph_keys(level->level),i);
		s = cstring_write(s, city);
		s = cstring_write(s, "\n");
		map meds = (map)(graph_node_value(graph_get_node(level->level, city)));
		list medNames = map_keys(meds);
		for (j=0; j<list_size(medNames); j++) {
			cstring key = list_get(medNames, j);
			int *value = map_get(meds, key);
			sim_keypair kp = (sim_keypair)sim_keypair_init(key, *value);
			s = cstring_write(s, sim_keypair_serialize(kp));
			s = cstring_write(s, "\n");
			sim_keypair_free(kp);
		}
		s = cstring_write(s, "\n");
	}
	for (i=0; i<n; i++) {
		cstring from = list_get(graph_keys(level->level), i);
		list tos = graph_node_arcs(graph_get_node(level->level, from));
		for (j=0; j<list_size(tos); j++){
			cstring to = graph_node_key(graph_arc_to(list_get(tos, j)));
			int weight = graph_arc_weight(list_get(tos, j));
			s = cstring_write(s, from);
			s = cstring_write(s, " ");
			s = cstring_write(s, to);
			s = cstring_write(s, " ");
			s = cstring_write(s, cstring_fromInt(weight));
			s = cstring_write(s, "\n");
		}
		s = cstring_write(s, "\n");
	}
	return s;
}

sim_level sim_level_deserialize(cstring s) {
	sim_level l = calloc(sizeof(struct sim_level), 1);
	l->level = graph_init((comparer)cstring_comparer, (cloner)cstring_copy);
	int i = 0;
	int amount = 0;
	int error = 0;
	list lines = cstring_split_list(s, "\n");
	
	foreach(cstring, line, lines) {
		if (strlen(line) > 0) {
			if (i == 0) {
				int _err = 1;
				amount = cstring_parseInt(line, &_err);
				if (!_err) {
					error = 1;
					break;
				}
			} else if (amount > 0) {
				cstring city_name = (cstring) malloc(sizeof(cstring));
				if (cstring_matches(line, " ")) {
					error = 1;
					break;
				}
				city_name = cstring_copy(line);
				map medicines = map_init(cstring_comparer, NULL);
				graph_add_node(l->level, city_name, medicines);
				

				foreach_next(line);
				while(line != NULL && strlen(line) > 0 && !error) {
					sim_keypair kp = sim_keypair_deserialize(line);
					if (kp != NULL) {
						int * value = calloc(sizeof(int), 1);
						*value = kp->amount;
						map_set(medicines, cstring_copy(kp->name), value);
					}
					else {
						error = 1;
					}
					free(kp);
					foreach_next(line);
				}
				if (error) {
					break;
				}
				amount--;
			} else {
				while(line != NULL && strlen(line) > 0 && !error) {
					sim_map_relation r = sim_map_relation_deserialize(line);
					if (r != NULL) {
						graph_add_arc(l->level, r->from, r->to, r->weight);
					}
					else {
						error = 1;
					}
					foreach_next(line);
				}
			}

		}
		if (error) {
			break;
		}
		i++;
	}
	list_free(lines);	
	if (error) {	
		graph_free(l->level);
		free(l);
		return NULL;
	}
	
	return l;
}

void sim_level_free(sim_level lev) {
	if (lev->frontend_client != NULL) {
		sim_client_free(lev->frontend_client);
	}
	if (lev->airlines_server != NULL) {
		sim_server_free(lev->airlines_server);
	}
	
	list keys = graph_keys(lev->level);
	
	foreach(cstring, key, keys) {
		graph_node n = graph_get_node(lev->level,key);
		if (graph_node_value(n) != NULL) {
			map_free(graph_node_value(n));
		}
	}
	list_free(keys);
	
	free(lev->level);
}

void sim_level_query_receiver(sim_message s) {
	cstring message = sim_message_read(s);
	int noerror = 0;
	if (cstring_compare(message, "INIT_STAT") == 0) {
		if (current_level == NULL) {
			printf("Too soon :(");
		} else {
			cprintf("Got spawn authorization\n", ROSA);
			sem_up(current_level->frontend_sem,1);
		}
	}
}

void sim_level_print_receiver(sim_message s) {
	cstring data = sim_message_read(s);
	list header = cstring_split_list(sim_message_header(s), " ");
	
	
	int noerr = 0;
	int sender_id = cstring_parseInt(list_get(header,1), &noerr);
	if (noerr == 0) {
		sender_id = current_level->level_id;
	}
	
	sim_client_print(current_level->frontend_client, data, sender_id);

	list_free_with_data(header);
}

void sim_level_copy_level(sim_message mes) {
	sim_message_write(mes, sim_level_serialize(current_level));
	sim_message_respond(mes);
}

void sim_level_copy_single_airline(sim_message s){
	cstring data = cstring_copy(sim_message_read(s));
	int noerror = 0;
	int airline_id = cstring_parseInt(data, &noerror);

	sim_airline air = NULL;
	foreach(sim_airline, _air, airlines) {

		if (sim_airline_id(_air) == -1) {
			sim_airline_set_id(_air, airline_id);
			air = _air;
			break;
		}
	}

	if (noerror) {
		cstring resp = cstring_init(0);
		
		if (air != NULL) {
			resp = cstring_write(resp, sim_airline_serialize(air, FALSE));
		}
		else {
			resp = cstring_write(resp, "null");
		}
		sim_message_write(s, resp);
		sim_message_respond(s);
	}

	free(data);	
}

void sim_level_start_server(int connection_t, int to_id) {
	current_level->airlines_server = sim_server_init(connection_t, P_AIRLINE, to_id);
	
	char * seq = "PRINT";
	sim_server_add_receiver(current_level->airlines_server, seq, sim_level_print_receiver);
	char * seq2 = "COPY_LEVEL";
	sim_server_add_receiver(current_level->airlines_server, seq2, sim_level_copy_level);
	char * seq3 = "COPY_SAIR";
	sim_server_add_receiver(current_level->airlines_server, seq3, sim_level_copy_single_airline);
	
}

void sim_level_spawn_airlines() {
	foreach(sim_airline, airline, airlines) {
		sim_server_spawn_child(current_level->airlines_server);
		cprintf("LEVEL: Going down for spawning\n", ROSA);
		sem_down(current_level->level_sem, 1);						// Lock #4
	}
}

// Updates all the data of distances of each plane and sends all the ones which must update in numbers
// Separated by a comma.
cstring planes_update_moves() {
	cstring data = cstring_init(0);
	list keys = map_keys(current_level->plane_distance);
	list valids = list_init();
	foreach(int *, key, keys) {
		int * val = map_get(current_level->plane_distance, key);
		if (* val == -1) {
			*val = 0;
		} else if (*val > 0){
			*val -= 1;
		} else {
			list_add(valids, cstring_fromInt(*key));
		}
	}	
	data = cstring_join_list(valids, ",");
	list_free_with_data(valids);
	return data;
}


void send_turn_tick() {
	cstring msg = cstring_copy("TURN ");
	cstring valid_planes = planes_update_moves();
	msg = cstring_write(msg, cstring_fromInt(current_level->turn));


	
	msg = cstring_write(msg, " ");
	msg = cstring_write(msg, valid_planes);

	sem_set_value(current_level->airline_sem, 0);
	
	cprintf("LEVEL: SENDING %s\n", ROSA, msg);
	sim_server_broadcast_query(current_level->airlines_server, msg);

	sem_up(current_level->airline_sem, list_size(airlines));
	
	free(valid_planes);
	free(msg);
}

int sim_level_alive() {
	return current_level->turn <= 100;
}

void start_planes_map() {
	current_level->plane_location = map_init(int_comparer, int_cloner);
	current_level->plane_distance = map_init(int_comparer, int_cloner);
	int j = 1;
	foreach(sim_airline, airline, airlines) {
		list planes = sim_airline_planes(airline);
		int pid = 100 * j;
		int i = 1;
		for(; i < list_size(planes) + 1; i++) {
			int id = pid + i;
			int * value = malloc(sizeof(int));
			*value = 0;
			map_set(current_level->plane_distance, &id, value);
			map_set(current_level->plane_location, &id, sim_plane_start_city((sim_plane)list_get(planes, i - 1)));
		}
		j++;
	}
}

void sim_level_game() {
	cprintf("LEVEL: STARTING GAME\n", ROJO);
	start_planes_map();
	sem_up(current_level->airline_sem,list_size(airlines));
	while (sim_level_alive()) {		
		cprintf("LEVEL: Going down for TURN %d\n", ROJO, current_level->turn);
		send_turn_tick();
		sem_down(current_level->level_sem, list_size(airlines));		
		usleep(200 * 1000);
		current_level->turn++;
	}
}


void sim_level_main(int connection_t, int from_id, int to_id) {
	sim_client c = sim_client_init(connection_t, 0, from_id, to_id, sim_level_query_receiver);
	sim_level l = sim_client_copy_level(c, to_id);	

	current_level = l;
	
	l->frontend_client = c;

	l->airline_sem = sem_create_typed(0, "airline");
	l->frontend_sem = sem_create_typed(0, "frontend");
	l->level_sem = sem_create_typed(0, "level");
	
	l->level_id = to_id;
	l->turn = 0;

	airlines = sim_client_copy_airline(c, to_id);
	
	sem_up(l->frontend_sem, 1);
	cprintf("LEVEL: Going down for startup\n", ROSA);
	sem_down(l->level_sem, 1);				// Lock #3
	
	sim_level_start_server(connection_t, to_id + 1);
	
	sim_level_spawn_airlines();
	
	

	
	sim_level_game();
	
	sim_server_broadcast_query(current_level->airlines_server, "END");
	printf("Game end\n");
	sem_up(l->frontend_sem, 1);

}
