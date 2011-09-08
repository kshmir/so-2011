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

	int	  tick_sem;

};

graph sim_level_graph(sim_level lev) {
	return lev->level;
}

int sim_level_has_city(sim_level lev, cstring city) {
	return graph_get_node(lev->level, city) != NULL;
}

cstring sim_level_serialize(sim_level level) {
	list g_k = graph_keys(level->level);
	int n = list_size(g_k);
	cstring s = cstring_fromInt(n);
	s =  cstring_write(s, "\n\n");
	int i = 0, j = 0;
	for (i=0; i<n; i++) {
		cstring city = list_get(g_k,i);
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
		cstring from = list_get(g_k, i);
		list tos = graph_node_arcs(graph_get_node(level->level, from));
		for (j=0; j<list_size(tos); j++){
			cstring to = graph_node_key(graph_arc_to(list_get(tos, j)));
			int weight = graph_arc_weight(list_get(tos, j));
			cstring aux = cstring_fromInt(weight);
			s = cstring_write(s, from);
			s = cstring_write(s, " ");
			s = cstring_write(s, to);
			s = cstring_write(s, " ");
			s = cstring_write(s, aux);
			s = cstring_write(s, "\n");
			free(aux);
		}
		s = cstring_write(s, "\n");
	}
	list_free(g_k);
	return s;
}
sim_level sim_level_deserialize(cstring s) {
	int aux=0;
	return sim_level_deserialize_line_error(s,&aux);
}
sim_level sim_level_deserialize_line_error(cstring s,int* err_line) {
	sim_level l = calloc(sizeof(struct sim_level), 1);
	l->level = graph_init((comparer)cstring_comparer, (cloner)cstring_copy);
	int i = 0;
	int amount = 0;
	int error = 0;
	int line_number=0;
	list lines = cstring_split_list(s, "\n");

	foreach(cstring, line, lines) {
		line_number++;
		if (strlen(line) > 0) {
			if (i == 0) {
				int _err = 1;
				amount = cstring_parseInt(line, &_err);
				if (!_err) {
					error = 1;
					break;
				}
			} else if (amount > 0) {
				cstring city_name;
				if (cstring_matches(line, " ")) {
					error = 1;
					break;
				}
				city_name = cstring_copy(line);
				map medicines = map_init(cstring_comparer, NULL);
				graph_add_node(l->level, city_name, medicines);
				foreach_next(line);
				line_number++;
				while(line != NULL && strlen(line) > 0 && !error) {
					sim_keypair kp = sim_keypair_deserialize(line);

					if (kp != NULL) {
						line_number++;
						int * value = calloc(sizeof(int), 1);
						*value = kp->amount;
						cstring n = cstring_copy(kp->name);
						map_set(medicines, n, value);
					}
					else {
						error = 1;
					}
					free(kp);
					foreach_next(line);
				}
				amount--;
			} else {
				while(line != NULL && strlen(line) > 0 && !error) {
					sim_map_relation r = sim_map_relation_deserialize(line);
					if (r != NULL) {
					line_number++;
						graph_add_arc(l->level, r->from, r->to, r->weight);
					}
					else {
						error = 1;
					}
					free(r);
					foreach_next(line);
				}
			}

		}
		if (error) {
			break;
		}
		i++;
	}
	list_free_with_data(lines);	
	if (error) {	
		*err_line=line_number;
		graph_free(l->level);
		free(l);
		return NULL;
	}
	*err_line=0;
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
	//	sem_free_typed(lev->level_sem, "level");
	//	sem_free_typed(lev->airline_sem, "airline");

	free(lev->level);
}

void sim_level_query_receiver(sim_message s) {
	cstring message = sim_message_read(s);
	int noerror = 0;
	if (cstring_compare(message, "INIT_STAT") == 0) {
		if (current_level == NULL) {
			cprintf("WTF\n", ROJO);
//			sleep(1);
			sem_up(current_level->frontend_sem,1);
		} else {
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
	cstring l = sim_level_serialize(current_level);
	sim_message_write(mes, l);
	sim_message_respond(mes);
	free(l);
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
		free(resp);
	} else {
		cprintf("ERROR WITH DATA %s\n", AMARILLO, data);
	}


	free(data);	
}

void sim_level_med_fill_transaction(sim_message msg) {

	
	cstring data = sim_message_read(msg);

///	cprintf("MEDFILL: %s;%s\n", AMARILLO, sim_message_header(msg), data);
	list splitted = cstring_split_list(data, " ");
	cstring city = list_get(splitted, 0);
	cstring medicine = list_get(splitted, 1);
	int error = 0;
	int ret_value = -1;
	int value =  cstring_parseInt(list_get(splitted, 2), &error);
	int plane_id =  cstring_parseInt(list_get(splitted, 3), &error);
	graph_node city_data_node = graph_get_node(current_level->level, city);
	if (city_data_node != NULL) {
		map city_data = graph_node_value(city_data_node);
		if (map_get(city_data, medicine) != NULL) {
			int * med = (int *) map_get(city_data, medicine);

			if (* med != 0) {
				int old_val = * med ;
				if (value > * med) {
					ret_value = value - * med;
					* med = 0;
				} else {
					ret_value = 0;
					* med = * med - value;
				}			
				//cprintf("LEVEL: Plane %d filling medicine %s in %s \n\tBefore:%d After:%d Diff: %d, Stays with: %d\n", ROJO, plane_id, medicine, city, old_val, *med, old_val - *med, ret_value);
			}
			else {
				ret_value = value;
			}

		}
	}

	cstring response = cstring_fromInt(ret_value);
	sim_message_write(msg, response);
	sim_message_respond(msg);
	free(response);

	list_free_with_data(splitted);
}

void sim_level_set_path_transaction(sim_message msg) {
	cstring data = sim_message_read(msg);	
//	cprintf("PATHT: %s;%s\n", ROSA, sim_message_header(msg), data);
	list splitted = cstring_split_list(data, " ");
	cstring id_string = list_get(splitted, 0);
	cstring city_from = list_get(splitted, 1);
	cstring city_to = list_get(splitted, 2);


	int error = 0;
	int id = cstring_parseInt(id_string, &error);

	int found = 0;
	list ids = map_keys(current_level->plane_distance);
	foreach(int *, key, ids) {
		if (*key == id) {

			found = 1;
			break;
		}
	}

	if (found) {

		cstring plane_city_from = map_get(current_level->plane_location, &id);
		int * distance = map_get(current_level->plane_distance, &id);
		if (plane_city_from != NULL && cstring_compare(plane_city_from, city_from) == 0
				&& distance != NULL && * distance == 0) {

			free(map_remove(current_level->plane_location, &id));
			plane_city_from = NULL;

			cstring _city_to = cstring_copy(city_to);
			map_set(current_level->plane_location, &id, _city_to);

			graph_node city_node = graph_get_node(current_level->level, city_from);

			int error = 1;
			list n_arcs = graph_node_arcs(city_node);
			foreach_(graph_arc, arc, n_arcs) {
				graph_node to_node = graph_arc_to(arc);
				if (cstring_compare(graph_node_key(to_node), city_to) == 0) {
					* distance = graph_arc_weight(arc);
					error = 0;
				}
			}
			//cprintf("LEVEL: Plane %d moving from %s to %s takes %d turns \n", ROJO, id, city_from, city_to, *distance);
			cstring response = cstring_fromInt((error == 0) ? 1 : -1);

			sim_message_write(msg, response);
			sim_message_respond(msg);
			free(response);
		} else {
			cstring response = cstring_fromInt((error == 0) ? 1 : -1);

			sim_message_write(msg, response);
			sim_message_respond(msg);
			free(response);
		}

	} else {
		cprintf("PLANE DOES NOT EXIST %d\n", ROJO, id);
		cstring response = cstring_fromInt(-1);
		sim_message_write(msg, response);
		sim_message_respond(msg);
		free(response);
	}

	list_free(ids);
	list_free_with_data(splitted);

}

void sim_level_med_get_value(sim_message msg) {
	cstring data = sim_message_read(msg);
	list splitted = cstring_split_list(data, " ");
	cstring city = list_get(splitted, 0);
	cstring medicine = list_get(splitted, 1);	

	int ret_value = -1;

	graph_node city_data_node = graph_get_node(current_level->level, city);
	if (city_data_node != NULL) {
		map city_data = graph_node_value(city_data_node);
		if (map_get(city_data, medicine) != NULL) {
			ret_value = * (int *) map_get(city_data, medicine);
		}
	}

	cstring response = cstring_fromInt(ret_value);
	sim_message_write(msg, response);
	sim_message_respond(msg);
	free(response);

	list_free_with_data(splitted);
}

void sim_level_start_server(int connection_t, int to_id) {


	current_level->airlines_server = sim_server_init(connection_t, P_AIRLINE, to_id);


	char * seq = "PRINT";
	sim_server_add_receiver(current_level->airlines_server, seq, sim_level_print_receiver);
	char * seq2 = "COPY_LEVEL";
	sim_server_add_receiver(current_level->airlines_server, seq2, sim_level_copy_level);
	char * seq3 = "COPY_SAIR";
	sim_server_add_receiver(current_level->airlines_server, seq3, sim_level_copy_single_airline);
	char * seq4 = "MEDF";
	sim_server_add_receiver(current_level->airlines_server, seq4, sim_level_med_fill_transaction);	
	char * seq5 = "PMOV";
	sim_server_add_receiver(current_level->airlines_server, seq5, sim_level_set_path_transaction);	
	char * seq6 = "MEDG";
	sim_server_add_receiver(current_level->airlines_server, seq6, sim_level_med_get_value);	
}

void sim_level_spawn_airlines() {
	foreach(sim_airline, airline, airlines) {
		sim_server_spawn_child(current_level->airlines_server);
//		cprintf("LEVEL: Going down for spawning\n", ROSA);
		sem_down(current_level->level_sem, 1);						// Lock #4
	}
}

// Updates all the data of distances of each plane and sends all the ones which must update in numbers
// Separated by a comma.
cstring planes_update_moves() {
	cstring data = NULL;
	list keys = map_keys(current_level->plane_distance);
	list valids = list_init();
	foreach(int *, key, keys) {
		int * val = map_get(current_level->plane_distance, key);
		if (* val < 0) {
			*val = 0;
		} else if (*val > 0){
			*val -= 1;
//			cprintf("LEVEL: plane %d moving to %s, %d turns ahead.\n", VERDE, *key, map_get(current_level->plane_location, key), *val);

		} else {
			list_add(valids, cstring_fromInt(*key));
		}
	}	
	data = cstring_join_list(valids, ",");
	list_free_with_data(valids);
	list_free(keys);
	return data;
}


void send_turn_tick() {
	cstring aux = cstring_fromInt(current_level->turn);
	cstring msg = cstring_copy("TURN ");
	cstring valid_planes = planes_update_moves();
	msg = cstring_write(msg, aux);



	msg = cstring_write(msg, " ");
	msg = cstring_write(msg, valid_planes);





	sim_server_broadcast_query(current_level->airlines_server, msg);


	free(aux);
	free(valid_planes);
	free(msg);
}

int sim_level_alive() {
	int alive = 0;

	list cities = graph_keys(current_level->level);
	foreach(cstring, city, cities) {
		graph_node n = graph_get_node(current_level->level, city);
		map meds_map = graph_node_value(n);
		list medicines = map_keys(meds_map);
		int i = 0;
		for(; i < list_size(medicines); i++) {
			cstring medicine = list_get(medicines, i);
			int amount = * (int*) map_get(meds_map, medicine);
			if (amount > 0) {
				alive = 1;
			}
		}
		list_free(medicines);
	}
	list_free(cities);
	return alive;
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
	cprintf("LEVEL: Starting game\n", CELESTE);
	start_planes_map();
	sem_set_value(current_level->level_sem, 0);		
	sem_up(current_level->airline_sem,list_size(airlines));
	while (sim_level_alive()) {		
		cprintf("-----------------------------------------------------\nLEVEL: Going down for TURN %d\n", CELESTE, current_level->turn);
		send_turn_tick();
		sem_down(current_level->level_sem, list_size(airlines));		
		current_level->turn++;
	}
}


void sim_level_main(int connection_t, int from_id, int to_id) {

	sim_client c = sim_client_init(connection_t, 0, from_id, to_id, sim_level_query_receiver);

//	cprintf("LEVEL CONNECTED ...\n", ROJO);
	
	sim_level l = sim_client_copy_level(c, to_id);
	if (l == NULL) {
		cprintf("OMFG %d", ROJO, (int) l);
	}


	current_level = l;

	l->frontend_client = c;
	
	l->airline_sem = sem_create_typed("airline");
	l->frontend_sem = sem_create_typed("frontend");
	l->level_sem = sem_create_typed("level");

	l->tick_sem = sem_create_typed("tick");
	sem_set_value(l->tick_sem, 1);
	sem_set_value(l->level_sem, 0);

	l->level_id = to_id;
	l->turn = 0;

	
	
	airlines = sim_client_copy_airline(c, to_id);
	


	sim_level_start_server(connection_t, to_id + 1);

	sim_level_spawn_airlines();





	sim_level_game();



	sim_server_broadcast_query(current_level->airlines_server, "END");



	cprintf("LEVEL: Game end\n", CELESTE);

	sem_up(l->level_sem, list_size(airlines));
	sem_down(l->airline_sem, list_size(airlines));
	sem_up(l->frontend_sem, 1);	

	sim_level_free(l);
}
