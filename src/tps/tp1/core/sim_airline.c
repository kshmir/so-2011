
#include "sim_airline.h"

static sim_airline airline;



struct sim_airline {
	int					id;
	list				planes;
	sim_client			c;
	graph				level_graph;
	sim_level			level;

	int					airline_sem;
	int					level_sem;

	int					planes_sem;
	int					internal_sem;
	
	int					end_order;
	int					current_turn;
	
	
	pthread_t *			p_threads;				// Thread for each plane.
	pthread_mutex_t	*	unlock_mutex;			// Cond used to unlock.
	pthread_cond_t	*	unlock_cond;			// Cond used to unlock.
};


int sim_airline_planes_sem(sim_airline air) {
	return air->planes_sem;
}

int sim_airline_internal_sem(sim_airline air) {
	return air->internal_sem;
}

pthread_mutex_t * sim_airline_mutex(sim_airline air) {		
	return air->unlock_mutex;
}

pthread_cond_t * sim_airline_cond(sim_airline air) {		
	return air->unlock_cond;
}

int sim_airline_id(sim_airline air) {
	return air->id;
}

void sim_airline_set_id(sim_airline air, int id) {
	air->id = id;
}

int sim_airline_comparer(void_p a1, void_p a2) {		
	sim_airline _a1 = (sim_airline) a1;
	sim_airline _a2 = (sim_airline) a2;
	return _a1->id - _a2->id;
}

list sim_airline_planes(sim_airline air) {
	return air->planes;
}

sim_airline sim_airline_deserialize(cstring s, int airline_id) {
	sim_airline airline = calloc(sizeof(struct sim_airline), 1);
	airline->planes = list_init();
	airline->id = airline_id;
	int extra = 1;
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
			} else {
				cstring plane_start_point = cstring_copy(line);
				sim_plane p = (sim_plane) sim_plane_init(plane_start_point);
				if (airline_id != -1) {
					sim_plane_set_id(p, airline_id + extra++);
					sim_plane_set_dead(p, FALSE);
				}
				foreach_next(line);
				while(strlen(line) > 0 && !error) {
					sim_keypair kp = sim_keypair_deserialize(line);
					if (kp != NULL) {
						sim_plane_set_medicines(p, cstring_copy(kp->name), kp->amount);
					} else {
						error = 1;
					}
					sim_keypair_free(kp);
					foreach_next(line);
				}
				list_add(airline->planes, p);
				amount--;
			}
		}
		i++;
	}
	list_free(lines);

	if (error || amount > 0) {
		return NULL;
	}


	return airline;
}

cstring sim_airline_serialize(sim_airline air, int hasId) {
	cstring s = cstring_init(1);
	if (hasId) {
		s = cstring_write(s,cstring_fromInt(air->id));
		s = cstring_write(s,"\n\n");
	}
	s = cstring_write(s,cstring_fromInt(list_size(air->planes)));
	s = cstring_write(s,"\n\n");
	foreach(sim_plane, p, air->planes) { 
		s = cstring_write(s, (cstring) sim_plane_serialize(p));
		
		s = cstring_write(s,"\n");
	}
	return s;
}


void set_planes_to_think(cstring splitted) {
	list splits = cstring_split_list(splitted, ",");
	foreach(cstring, _id, splits) {
		int noerr = 0;
		int id = cstring_parseInt(_id, &noerr);

		list planes = airline->planes;
		int i = 0;
		for (; i < list_size(planes); i++) {
			sim_plane p = (sim_plane)list_get(planes,i);
			if (sim_plane_id(p) == id) {
				sim_plane_set_to_think(p,TRUE);
			}
		}
	}
}

void sim_airline_query_receiver(sim_message resp) {
	cstring data = sim_message_read(resp);
	
	if (cstring_compare(data, "END") == 0) {				// END finishes the airline by unlocking it.
		
		airline->end_order = 1;
		foreach(sim_plane, plane, airline->planes) {
			sim_plane_set_dead(plane, 1);
		}
		sem_up(airline->internal_sem, 1);
	} else if (cstring_matches(data, "TURN")) {				// TURN changes the turn inside the airline.
															// And unlocks it in order to think
		list params = cstring_split_list(data, " ");
		int noerror = 0;
		int turn = cstring_parseInt(list_get(params, 1), &noerror);
		
		if (noerror) {

			airline->current_turn = turn;
			set_planes_to_think(list_get(params,2));
		//	cprintf("LEVEL TO AIRLINE: Setting airline UP %d\n", AZUL_CLARO, airline->id);
			sem_up(airline->internal_sem, 1);
			sem_down(airline->airline_sem, 1);
		//	cprintf("AIRLINE: I UNLOCK\n", ROSA);
			cprintf("GOT MESS\n",AZUL);
		}

		list_free_with_data(params);
	}

}

void sim_airline_game() {
	sleep(1);
	while (!airline->end_order) {
		sem_down(airline->internal_sem, 1);					// Lock #5
		pthread_cond_broadcast(airline->unlock_cond);
			
		sem_down(airline->planes_sem, list_size(airline->planes));

//		cprintf("AIRLINE: Sending level UP %d %d\n", AZUL, airline->id, sem_value(airline->internal_sem));
		sem_up(airline->level_sem, 1);						// Unlock #5
	}
}


void sim_airline_main(int connection_t, int from_id, int to_id) {

	sim_client c = sim_client_init(connection_t, 0, from_id, to_id, sim_airline_query_receiver);
	printf("Client OPEN\n");

	sim_airline air = sim_client_copy_single_airline(c, to_id);
	
	air->end_order = 0;

	cstring _planes_sem = cstring_copy("planes");
	_planes_sem = cstring_write(_planes_sem, cstring_fromInt(to_id));
	air->planes_sem = sem_create_typed(0, _planes_sem);
	air->airline_sem = sem_create_typed(0, "airline");
	air->level_sem = sem_create_typed(0, "level");
	cstring _internal_sem = cstring_copy("internal");
	_internal_sem = cstring_write(_internal_sem, cstring_fromInt(to_id));
	
	air->internal_sem = sem_create_typed(0, _internal_sem);
	air->level = sim_client_copy_level(c, to_id);
	air->current_turn = 0;
	
	sem_set_value(air->internal_sem, 0);
	
	air->unlock_mutex = (pthread_mutex_t *) calloc(sizeof(pthread_mutex_t), 1);
	air->unlock_cond = (pthread_cond_t *) calloc(sizeof(pthread_cond_t), 1);
	air->p_threads = (pthread_t *) calloc(sizeof(pthread_t), list_size(air->planes));
	
	
	pthread_mutex_init(air->unlock_mutex, NULL);
	pthread_cond_init(air->unlock_cond, NULL);
	
	airline = air;
	airline->c = c;
	
	
	cprintf("My level has %d cities\n", ROJO, graph_size(sim_level_graph(air->level)));
	
										 
	int i = list_size(airline->planes);
	foreach(sim_plane, plane, airline->planes) {
		struct sim_plane_data * dat = malloc(sizeof(struct sim_plane_data));		
		dat->plane = plane;
		dat->airline = airline;
		dat->level = airline->level;
		pthread_create(&airline->p_threads[i++], NULL, (void_p) sim_plane_main, (void_p) dat);

		sem_down(air->internal_sem, 1);
	}		

	sem_up(air->level_sem,1);								// Tells the level we're ready to play.
	sem_down(air->airline_sem,1);
	
	sim_airline_game();
	
	
	sim_client_free(c);

	// Pide la información del airline y se deserializa checked!
	// Airline inicializa sus threads para cada plane y los pone en accion.
}
