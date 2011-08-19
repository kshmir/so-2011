#include "sim_airline.h"

struct sim_airline {
	int id ;
	list planes ;
	sim_client c;
};

int sim_airline_comparer(void_p a1, void_p a2) {		
	sim_airline _a1 = (sim_airline) a1;
	sim_airline _a2 = (sim_airline) a2;
	return _a1->id - _a2->id;
}

list sim_airline_planes(sim_airline air) {
	return air->planes;
}

sim_airline sim_airline_deserialize(cstring s, int airline_id) {
	sim_airline airline = malloc(sizeof(struct sim_airline));
	airline->id = airline_id;
	int i = 0;
	int nplanes = cstring_parseInt(cstring_copy_line(s), &i);
	i = 0;
	while (s[i++]!='\n');
	while (s[i++]!='\n');
	int j = 0;
	airline->planes = list_init();
	for(j = 0; j<nplanes; j++){
		cstring splane = cstring_copy_till_char(s+i, '\n', 3);
		sim_plane p = sim_plane_deserialize(splane, airline_id*1000 + j);
		list_add(airline->planes, p);
		free(splane);
		while (s[i++]!='\n');
		while (s[i++]!='\n');
		while (s[i++]!='\n');
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
	foreach(n, air->planes) { 
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