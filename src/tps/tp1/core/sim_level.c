#include "sim_level.h"

struct sim_level {
	// level_distances
	//	stores cstrings  as: keys	# Name of the city
	//  stores map		 as: values # Hash of medicines.
	////// This map stores
	////// cstrings as: keys		# Name of medicine
	////// int *    as: values		# Amount of medicine.
	// stores distances	 as: arcs   # Distance between cities
	graph level;
		
	sim_client frontend_client;
	sim_server airlines_server;
	
	int	  control_sem;
};

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
	
}

void sim_level_main(int connection_t, int from_id, int to_id) {
	sim_client c = sim_client_init(connection_t, 0, from_id, to_id, sim_level_query_receiver);
	
	
	sim_level l = sim_client_copy_level(c, to_id);
	int control_sem = sem_create_typed(0, "control");
	

	list airlines = sim_client_copy_airline(c, to_id);
	
	cstring	alert = cstring_copy("PRINT! ");
	
	
	sim_airline first_airline = list_get(airlines,0);
	sim_plane first_plane = list_get(sim_airline_planes(first_airline), 0);
	alert = cstring_write(alert, sim_plane_start_city(first_plane));
	
	sim_client_print(c, alert, to_id);
	
	sem_up(control_sem, 1);
	
	// Inicializa el server, bindea sus funciones al server.
	// Las mismas se vinculan con el cliente del frond a forma de log.
}
