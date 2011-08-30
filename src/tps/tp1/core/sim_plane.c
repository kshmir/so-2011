#include "sim_plane.h"

struct sim_plane {
	int		id;
	cstring start_city;
	list	medicines_keys;
	map		medicines;
};

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

void sim_plane_main(sim_plane plane) {
	
}

void sim_plane_print(sim_plane p) {

}
