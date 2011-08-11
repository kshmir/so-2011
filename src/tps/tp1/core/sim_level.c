#include "../sim_common_reqs.h"

struct sim_level {
	// level_distances
	//	stores cstrings  as: keys	# Name of the city
	//  stores map		 as: values # Hash of medicines.
	////// This map stores
	////// cstrings as: keys		# Name of medicine
	////// int *    as: values		# Amount of medicine.
	// stores distances	 as: arcs   # Distance between cities
	graph level;
		
	sim_client frontend_client;
	sim_server airlines_server;
}

cstring sim_level_serialize(sim_level level) {		
	// TODO: Serializes the map of the level.
}

sim_level sim_level_deserialize(cstring s) {
	// TODO: Deserializes the map of the level.
}

void sim_level_main(sim_level air) {
	// Inicializa el server, bindea sus funciones al server.
	// Las mismas se vinculan con el cliente del frond a forma de log.
}
