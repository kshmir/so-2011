#include "sim_plane.h"

#include <limits.h>

#define MOV_ERROR			-2
#define DATA_ERROR			-1
#define NOTHING				0
#define FILLED				1
#define MOVED				2


struct sim_plane {
	int		id;
	cstring start_city;
	list	medicines_keys;
	map		medicines;
	
	list    visited_places;
	
	int		turn_counter;
	int		must_think;
	int		set_dead;
};

int sim_plane_set_id(sim_plane p, int i) { 
	p->id = i;
}

int sim_plane_id(sim_plane p) { 
	return p->id;
}

void sim_plane_set_dead(sim_plane p,int num) {
	p->set_dead = num;
}

void sim_plane_set_to_think(sim_plane p,int num) {
	p->must_think = num;
}

int sim_plane_comparer(void_p a1, void_p a2) {
	sim_plane _a1 = (sim_plane) a1;
	sim_plane _a2 = (sim_plane) a2;
	return _a1->id - _a2->id;
}

sim_plane sim_plane_init(cstring s) {
	sim_plane p = calloc(sizeof(struct sim_plane),1);
	p->start_city = s;
	p->medicines_keys = list_init();
	p->medicines = map_init(cstring_comparer, NULL);
	return p;
}

cstring sim_plane_start_city(sim_plane p) {
	return p->start_city;
}

void sim_plane_set_medicines(sim_plane p, cstring key, int value) {
	int * v = calloc(sizeof(int),1);
	* v = value;
	
	list_add(p->medicines_keys, key);
	map_set(p->medicines, key, v);
}

cstring sim_plane_serialize(sim_plane p) {
	cstring s = cstring_copy(p->start_city);
	s = cstring_write(s,"\n");
	foreach(cstring, key, p->medicines_keys) {
		int * value = map_get(p->medicines, key);
		sim_keypair kp = (sim_keypair) sim_keypair_init(key, *value);
		s = cstring_write(s,sim_keypair_serialize(kp));
		s = cstring_write(s,"\n");
		sim_keypair_free(kp);
	}
	s[cstring_len(s)-1] = '\n';
	return s;
}



int	sim_plane_make_move(sim_airline airline, sim_plane plane, sim_level level, int policy) {
	graph_node n = graph_get_node(sim_level_graph(level), plane->start_city);
	list n_arcs = graph_node_arcs(n);
	cstring selected = NULL;
	 
	 foreach(graph_arc, arc, n_arcs) {
	 	graph_node to =		graph_arc_to(arc);
	 	cstring city =		graph_node_key(to);
	 	int		distance =	graph_arc_weight(arc);
	 
	 	if (list_indexOf(plane->visited_places, city, cstring_comparer) == -1) {
	 		selected = city;
	 		break;
	 	}
	 }
	 
	 
	 if (selected == NULL) {
	 
	 	if (list_size(n_arcs) == 0) {
	 
	 		return DATA_ERROR;
	 	} else {
			int r = rand() % list_size(n_arcs);
			graph_arc a =list_get(n_arcs, r);
			graph_node _n = graph_arc_to(a);
			selected = graph_node_key(_n);
	 	}
	 }
	list_add(plane->visited_places, selected);
	int mov = sim_client_post_plane_move(sim_airline_client(airline), sim_airline_id(airline), plane->id, plane->start_city, selected);

	if (mov != -1) {
		plane->start_city = selected;
		return MOVED;
	} else {
		return MOV_ERROR;
	}
		
		
	

}

int sim_plane_make_fill(sim_airline airline, sim_plane plane, sim_level level, int policy) {
	graph_node n = graph_get_node(sim_level_graph(level), plane->start_city);
	map m = graph_node_value(n);
	if (m != NULL) {
		int fill_done = 0;
		list city_m_keys = map_keys(m);
		foreach(cstring, med, city_m_keys) {
			int i = 0;
			for(; i < list_size(plane->medicines_keys) && !fill_done; i++) {
				cstring plane_med = list_get(plane->medicines_keys, i);
				if (plane_med != NULL && cstring_compare(med, plane_med) == 0) {
					int * val = map_get(plane->medicines, list_get(plane->medicines_keys, i));
					if (val != NULL) {
						int fill_value = sim_client_post_medicine_fill((sim_client) sim_airline_client(airline), 
																	   sim_airline_id(airline), 
																	   plane->start_city, 
																	   med, plane->id, 
																	   * val);
						
//						cprintf("fill_value: %d vs %d\n", ROJO, fill_value, *val);
						if (fill_value != -1 && fill_value != *val) {
							* val = fill_value;
							fill_done = 1;
						}
					}
				}
			}
			
			if (fill_done) {
//				cprintf("RETURN FILLED\n", ROJO);
				return FILLED;
			}
		}
		list_free(city_m_keys);
	}
	else {
		// For debugging
		list g_keys = graph_keys(sim_level_graph(level));
		foreach(cstring, key, g_keys) {
			cprintf("AIRLINE MAP KEY: %s\n", ROSA, key);
		}
		cprintf("PLANE %d has bad data %s %d\n", ROJO, plane->id, plane->start_city, graph_size(sim_level_graph(level)));
		return DATA_ERROR;
	}	
	return NOTHING;
}

void sim_plane_main(struct sim_plane_data * d) {
	pthread_mutex_lock(sim_airline_mutex(d->airline));
	sim_airline airline = (sim_airline) d->airline;
	sim_plane plane = (sim_plane) d->plane;
	sim_level level = (sim_level) d->level;
	
	plane->visited_places = list_init();

	plane->set_dead = 0;
	plane->turn_counter = 0;
//	cprintf("PLANE: I'm ready with id %d\n", VERDE_CLARO,plane->id);

	sim_airline_set_planes_waiting(airline, sim_airline_planes_waiting(airline) + 1);
	pthread_cond_broadcast(sim_airline_waiting_cond(airline));
//	cprintf("I WAIT %d %d %s\n", ROJO, plane->id, plane->must_think, (plane->id >= 300) ? "!!!!!" : "");
	pthread_cond_wait(sim_airline_planes_cond(airline), sim_airline_mutex(airline));

	
	while (!plane->set_dead) {

//		cprintf("I THINK %d %d %s\n", ROJO, plane->id, plane->must_think, (plane->id >= 300) ? "!!!!!" : "");
		if (plane->must_think) {
			int action_taken = sim_plane_make_fill(airline, plane, level, 0);
			
			if (action_taken == NOTHING) {
				action_taken = sim_plane_make_move(airline, plane, level, 0);
			} else {

			}

		}

		sim_airline_set_planes_running(airline, sim_airline_planes_running(airline) - 1);
		sim_airline_set_planes_waiting(airline, sim_airline_planes_waiting(airline) + 1);
		plane->turn_counter++;
//		cprintf(">>> I WAIT %d %d %s\n", ROJO, plane->id, plane->must_think, (plane->id >= 300) ? "!!!!!" : "");
		pthread_cond_broadcast(sim_airline_waiting_cond(airline));
		pthread_cond_wait(sim_airline_planes_cond(airline), sim_airline_mutex(airline));

	}		
	pthread_mutex_unlock(sim_airline_mutex(airline));

}

void sim_plane_print(sim_plane p) {

}
