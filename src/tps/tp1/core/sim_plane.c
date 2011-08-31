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
	cprintf("PLANE: I start and wait\n", VERDE);
	plane->set_dead = 0;
	plane->turn_counter = 0;

	while (!plane->set_dead) {
		pthread_cond_wait((void_p)sim_airline_cond(airline), (void_p)sim_airline_mutex(airline));
		if (plane->must_think) {
			cprintf("PLANE: I think :D %d\t %d\t %d\n", VERDE, plane->turn_counter, sim_airline_id(airline), plane->id);
		}
		sem_up(sim_airline_planes_sem(airline), 1);
		plane->turn_counter++;
	}

}

void sim_plane_print(sim_plane p) {

}
