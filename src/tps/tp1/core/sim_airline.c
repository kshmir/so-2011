#include "../sim_common_reqs.h"

struct sim_airline {
	int id;
	list planes;
	sim_client c;
}

int sim_airline_comparer(void_p a1, void_p a2) {		
	sim_airline _a1 = (sim_airline) a1;
	sim_airline _a2 = (sim_airline) a2;
	return _a1->id - _a2->id;
}

list sim_airline_planes(sim_airline air) {
	return air->planes;
}

sim_airline sim_airline_deserialize(cstring s) {		
	// TODO: Deserialize this
}

cstring sim_airline_serialize(sim_airline s) {
	cstring s = cstring_fromInt(s->id);
	s = cstring_write(s,"\n\n");
	s = cstring_write(s,cstring_fromInt(list_size(s->planes)));
	s = cstring_write(s,"\n\n");
	foreach(n, s->planes) { 
		sim_plane p = (sim_plane) list_node_value(n);
		s = cstring_write(s, sim_plane_serialize(p));
		s = cstring_write(s,"\n");
	}
	return s;
}

void sim_airline_main(sim_client c, int airline_id) {
	// Pide la información del airline y se deserializa
	// Airline inicializa sus threads para cada plane y los pone en accion.
}