/**
 *  SISTEMAS OPERATIVOS - ITBA - 2011  
 *	ALUMNOS:                         
 *		MARSEILLAN 
 *		PEREYRA
 *		VIDELA
 * -----------------------------------
 *
 * @file sim_airline.c
 *
 * @brief Handles all the logic of all the airlines.
 *
 * @author Agustin Marseillan
 * @author Maximo Videla
 * @author Cristian Pereyra
 */

#include "sim_airline.h"


/**
	Airline used by the process.
 */
static sim_airline airline;


/**
	Airline structure
 */
struct sim_airline {
	int					id;								// Id of the airline
	list				planes;							// Planes of the airline
	
	sim_client			c;								// Client used by the airline to talk to the level.
	graph				level_graph;					// Level graph used as a read only graph by the airline.
	sim_level			level;							// Level which contains the graph.

	int					airline_sem;					// Semaphore used for waiting the level
	int					level_sem;						// Semaphore used to make the level sleep.

	int					end_order;						// Flag for ending the airline.
	int					current_turn;					// Current turn we're on.
	
	int					waiting_for_level;				// Flag used when waiting for the level.
	
	int					planes_waiting;					// Amount of planes waiting.
	int					planes_running;					// Amount of planes running.

	pthread_t *			p_threads;						// Thread for each plane.
	pthread_mutex_t	*	unlock_mutex;					// Lock mutex.
	pthread_cond_t	*	unlock_planes_moving;			// Planes cond.
	pthread_cond_t	*	unlock_airline_waiting;			// Airline waiting cond.
	
	pthread_mutex_t	*	level_mutex;					// Level mutex.
	pthread_cond_t	*	level_wait;						// Cond used to wait for the level.
};

/**
 * Getter for planes_waiting
 */
int	sim_airline_planes_waiting(sim_airline air) {
	return air->planes_waiting;
}

/**
 * Setter for planes_waiting
 */
void sim_airline_set_planes_waiting(sim_airline air, int value) {
	air->planes_waiting = value;
}

/**
 * Getter for planes_running
 */
int	sim_airline_planes_running(sim_airline air) {
	return air->planes_running;
}

/**
 * Setter for planes_running
 */
void sim_airline_set_planes_running(sim_airline air, int value) {
	air->planes_running = value;
}

/**
 * Getter for the airline client.
 */
sim_client sim_airline_client(sim_airline air) {		
	return air->c;
}

/**
 * Getter for the airline mutex.
 */
pthread_mutex_t * sim_airline_mutex(sim_airline air) {		
	return air->unlock_mutex;
}

/**
 * Getter for the airline waiting cond.
 */
pthread_cond_t * sim_airline_waiting_cond(sim_airline air) {		
	return air->unlock_airline_waiting;
}

/**
 * Getter for the planes moving.
 */
pthread_cond_t * sim_airline_planes_cond(sim_airline air) {		
	return air->unlock_planes_moving;
}

/**
 * Getter for the airline id.
 */
int sim_airline_id(sim_airline air) {
	return air->id;
}

/**
 * Setter for the airline id.
 */
void sim_airline_set_id(sim_airline air, int id) {
	air->id = id;
}

/**
 * Airline comparer.
 */
int sim_airline_comparer(void_p a1, void_p a2) {		
	sim_airline _a1 = (sim_airline) a1;
	sim_airline _a2 = (sim_airline) a2;
	return _a1->id - _a2->id;
}

/**
 * Airline planes.
 */
list sim_airline_planes(sim_airline air) {
	return air->planes;
}

/**
 * Deserializes the airline.
 *
 * @param s Serialized string
 * @param airline_id Airline to serialize
 *
 * @return Fully built airline.
 */
sim_airline sim_airline_deserialize(cstring s, int airline_id) {
	int aux=0;
	return sim_airline_deserialize_line_error(s, airline_id, &aux);
}

/**
 * Deserializes the airline and tells the line it fails on.
 *
 * @param s Serialized string
 * @param airline_id Airline to serialize
 * @param err_line Line of the error.
 *
 * @return Fully built airline.
 */
sim_airline sim_airline_deserialize_line_error(cstring s, int airline_id, int *err_line) {
	sim_airline airline = calloc(sizeof(struct sim_airline), 1);
	airline->planes = list_init();
	airline->id = airline_id;
	int extra = 1;
	int i = 0;
	int amount = 0;
	int error = 0;
	*err_line=0;
	list lines = cstring_split_list(s, "\n");
	foreach(cstring, line, lines) {
		(*err_line)++;
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
				(*err_line)++;
				while(strlen(line) > 0 && !error) {
					sim_keypair kp = sim_keypair_deserialize(line);
					if (kp != NULL) {
						(*err_line)++;
						sim_plane_set_medicines(p, cstring_copy(kp->name), kp->amount);
					} else {
						error = 1;
						break;
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
	list_free_with_data(lines);
	if (error || amount > 0) {
		if(amount>0)
			*err_line=1;
		return NULL;
	}
	*err_line=0;
	return airline;
}

/**
 * Serializes the airline
 *
 * @param air Airline to serialize
 * @param hasId Serialize with it's id or not.
 *
 * @return Fully built airline.
 */
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


/**
 * Sets the planes whose id is in the string to think
 *
 * @param splitted string.
 */
void set_planes_to_think(cstring splitted) {
	list splits = cstring_split_list(splitted, ",");

	list planes = airline->planes;
	int i = 0;
	for (; i < list_size(planes); i++) {
		sim_plane p = (sim_plane)list_get(planes,i);
		sim_plane_set_to_think(p,FALSE);
	}
	foreach(cstring, _id, splits) {
		int noerr = 0;
		int id = cstring_parseInt(_id, &noerr);

		for (i = 0; i < list_size(planes); i++) {
			sim_plane p = (sim_plane)list_get(planes,i);
			if (sim_plane_id(p) == id) {
				sim_plane_set_to_think(p,TRUE);
			}
		}
	}
	list_free_with_data(splits);
}

/**
 * Receiver of the server query.
 *
 * @param resp petition.
 */
void sim_airline_query_receiver(sim_message resp) {
	cstring data = sim_message_read(resp);

	if (cstring_compare(data, "END") == 0) {				// END finishes the airline by unlocking it.

		airline->end_order = 1;
		foreach(sim_plane, plane, airline->planes) {
			sim_plane_set_dead(plane, 1);

		}
		airline->planes_running = 0;
		airline->planes_waiting = list_size(airline->planes);
		pthread_cond_broadcast(airline->unlock_airline_waiting);

		pthread_cond_broadcast(airline->level_wait);

	} else if (cstring_matches(data, "TURN")) {				// TURN changes the turn inside the airline.
		// And unlocks it in order to think
		list params = cstring_split_list(data, " ");
		int noerror = 0;
		int turn = cstring_parseInt(list_get(params, 1), &noerror);

		if (noerror) {
			pthread_mutex_lock(airline->level_mutex);
			airline->current_turn = turn;
			set_planes_to_think(list_get(params,2));
			if (pthread_cond_broadcast(airline->level_wait) != 0) 
				perror("ERROR!");
			pthread_mutex_unlock(airline->level_mutex);
		}

		list_free_with_data(params);
	}

}

/**
 * Game iteration.
 */
void sim_airline_game() {
	while (!airline->end_order) {
		while (airline->planes_waiting < list_size(airline->planes)) {
			pthread_cond_wait(airline->unlock_airline_waiting, airline->unlock_mutex);
		}

		airline->planes_running = list_size(airline->planes);
		pthread_cond_broadcast(airline->unlock_planes_moving);
		airline->waiting_for_level = 0;
		airline->planes_waiting = 0;

		while(airline->planes_running > 0) { 
			pthread_cond_wait(airline->unlock_airline_waiting, airline->unlock_mutex);
		}		

		cprintf("SENDING LEVEL UP %d AIRLINE: %d\n", CELESTE, sem_value(airline->level_sem), airline->id);

		sem_up(airline->level_sem, 1);					
		pthread_cond_wait(airline->level_wait,airline->level_mutex);
		airline->waiting_for_level = 1;
	}
}

/**
 * Airline main.
 *
 * @param connection_t	Connection type
 * @param from_id		Id of it's owner.
 * @param to_id			Id of the airline owner.
 */
void sim_airline_main(int connection_t, int from_id, int to_id) {

	sim_client c = sim_client_init(connection_t, 0, from_id, to_id, sim_airline_query_receiver);

	sim_airline air = sim_client_copy_single_airline(c, to_id);
	
	airline = air;
	air->end_order = 0;

	air->airline_sem = sem_create_typed("airline");
	air->level_sem = sem_create_typed("level");
	air->level = sim_client_copy_level(c, to_id);
	air->current_turn = 0;
	air->planes_waiting = 0;
	air->planes_running = 0;

	air->waiting_for_level = 0;

	air->level_mutex =						(pthread_mutex_t *) calloc(sizeof(pthread_mutex_t), 1);

	air->unlock_mutex =						(pthread_mutex_t *) calloc(sizeof(pthread_mutex_t), 1);
	air->unlock_planes_moving =				(pthread_cond_t *)	calloc(sizeof(pthread_cond_t), 1);
	air->unlock_airline_waiting =			(pthread_cond_t *)	calloc(sizeof(pthread_cond_t), 1);
	air->level_wait =						(pthread_cond_t *)	calloc(sizeof(pthread_cond_t), 1);
	air->p_threads =						(pthread_t *)		calloc(sizeof(pthread_t), list_size(air->planes) * 16);
	
	
	pthread_mutex_init(air->level_mutex, NULL);
	pthread_mutex_init(air->unlock_mutex, NULL);
	
	pthread_cond_init(air->unlock_planes_moving, NULL);
	pthread_cond_init(air->unlock_airline_waiting, NULL);
	pthread_cond_init(air->level_wait, NULL);	
	
	airline->c = c;
	
	int i = list_size(airline->planes);
	
	foreach(sim_plane, plane, airline->planes) {
		struct sim_plane_data * dat = malloc(sizeof(struct sim_plane_data));		
		dat->plane = plane;
		dat->airline = airline;
		dat->level = airline->level;
		pthread_create(&airline->p_threads[i], NULL, (void_p) sim_plane_main, (void_p) dat);
		declare_thread(&airline->p_threads[i++]);
	}		

	while (airline->planes_waiting < list_size(airline->planes)) {
		pthread_cond_wait(airline->unlock_airline_waiting, airline->unlock_mutex);
	}
	

	
	pthread_mutex_lock(airline->level_mutex);
	cprintf("WAITING FOR LEVEL AIRLINE: %d\n", ROJO, airline->id);
	sem_up(air->level_sem,1);		
	pthread_cond_wait(airline->level_wait,airline->level_mutex);
	airline->waiting_for_level = 1;
	sim_airline_game();
	pthread_mutex_unlock(airline->level_mutex);
	sem_down(air->level_sem, 1);

	sem_down(air->airline_sem, 1);



	sim_client_free(c);
}

/**
 * Game iteration.
 */
void sim_airline_free(sim_airline s) {
	// Talloc frees everything now
}