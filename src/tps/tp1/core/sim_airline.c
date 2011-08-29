#include "sim_airline.h"

struct sim_airline {
	int id ;
	list planes ;		pthread_cond_wait()
	sim_client c;
	graph level_graph;
};

extern	int spawned_airplanes=0;
extern	pthread_cond_t all_planes_asleep;
extern pthread_mutex_t waiting_planes;
extern int max_airplanes=0;
extern sim_level air_level=NULL;
extern sim_airline airline=NULL;

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
	if(hasId){
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

void sim_airline_main(sim_client c, int airline_id) {
	pthread_cond_init(&all_planes_asleep,NULL);
	pthread_mutex_init(&waiting_planes,NULL);
	pthread_mutex_lock(&waiting_planes);
	cstring data=cstring_write("REQUEST AIRLINE ",cstring_parseInt(airline_id));
	sim_transporter_write(c->t, data);
	cstring res=sim_transporter_listen(c->t);
	airline = sim_airline_deserialize(res,airline_id);
	sim_transporter_write(c->t,"REQUEST LEVELGRAPH");
	res=sim_transporter_listen(c->t);
	air_level = sim_level_deserialize(res,airline_id);
	max_airplanes=list_size(airline->planes);
	int i = 0;
	for(i=0; i<list_size(airline->planes); i++){
		pthread_t tid;
		pthread_create(&tid,NULL,sim_plane_main,list_get(airline->planes,i));

	}
		pthread_cond_wait(&all_planes_asleep,&waiting_planes);
	// Pide la información del airline y se deserializa checked!
	// Airline inicializa sus threads para cada plane y los pone en accion.
}
