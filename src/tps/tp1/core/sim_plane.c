#include "sim_plane.h"


struct sim_plane {
	int		id;
	cstring start_city;
	list	medicines_keys;
	map		medicines;
	
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

void sim_plane_main(struct sim_plane_data * d) {
	sim_airline airline = (sim_airline) d->airline;
	sim_plane plane = (sim_plane) d->plane;
	sim_level level = (sim_level) d->level;

	sem_up(sim_airline_internal_sem(airline), 1);	
	plane->set_dead = 0;
	plane->turn_counter = 0;
	cprintf("PLANE: I'm ready with id %d\n", VERDE_CLARO,plane->id);
	while (!plane->set_dead) {
		sim_airline_set_planes_waiting(airline, sim_airline_planes_waiting(airline) + 1);
		pthread_cond_wait(sim_airline_planes_cond(airline), sim_airline_mutex(airline));
		
		if (plane->set_dead) {
			return;
		}
//		sem_down(sim_airline_planes_end_sem(airline),1);
		if (plane->must_think) {
		//	graph_node n = graph_get_node(sim_level_graph(level), plane->start_city);
//			map m = graph_node_value(n);
//			if (m != NULL) {
//				int fill_done = 0;
//				int filld_count = 0;
//				list city_m_keys = map_keys(m);
////				foreach(cstring, med, city_m_keys) {
////					int i = 0;
////					for(; i < list_size(plane->medicines_keys) && !fill_done; i++) {
////						cstring plane_med = list_get(plane->medicines_keys, i);
////						if (plane_med != NULL && cstring_compare(med, plane_med) == 0) {
////							int * val = map_get(plane->medicines, list_get(plane->medicines_keys, i));
////							if (val != NULL) {
////								int fill_value = sim_client_post_medicine_fill((sim_client) sim_airline_client(airline), 
////																			   sim_airline_id(airline), 
////																			   plane->start_city, 
////																			   med, 
////																			   * val);
////								if (fill_value != -1) {
////									* val = fill_value;
////									fill_done = 1;
////								}
////							}
////						}
////
////
////					}
//
//					if (fill_done) {
//						cprintf("PLANE: FILL", VERDE);
//						break;
//					}
//
//				}
//			}
//			else {
//				
//				// For debugging
//				list g_keys = graph_keys(sim_level_graph(level));
//				foreach(cstring, key, g_keys) {
//					cprintf("AIRLINE MAP KEY: %s\n", ROSA, key);
//				}
//				cprintf("PLANE %d has bad data %s %d\n", ROJO, plane->id, plane->start_city, graph_size(sim_level_graph(level)));
//				getc(stdin);
//			}

			cprintf("PLANE: I think :D %d\t %d\t %d\n", VERDE, plane->turn_counter, sim_airline_id(airline), plane->id);
		}
		sim_airline_set_planes_running(airline, sim_airline_planes_running(airline) - 1);
		pthread_cond_broadcast(sim_airline_waiting_cond(airline));
		plane->turn_counter++;
	}

}

void sim_plane_print(sim_plane p) {

}
