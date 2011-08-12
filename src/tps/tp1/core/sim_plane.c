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

sim_plane sim_plane_deserialize(cstring s, int plane_id) {
	// Todo: Deserialize this
	
}

cstring sim_plane_serialize(sim_plane p) {
	cstring s = cstring_copy(p->start_city);
	s = cstring_write(s,"\n");
	foreach(n, p->medicines_keys) {
		cstring key = (cstring) list_node_value(n);
		int * value = map_get(p->medicines, key);
		sim_keypair kp = (sim_keypair) sim_keypair_init(key, *value);
		s = sim_keypair_serialize(kp);
		s = cstring_write(s,"\n");
		sim_keypair_free(kp);
	}
	return s;
}

void sim_plane_main(sim_plane air) {

}
