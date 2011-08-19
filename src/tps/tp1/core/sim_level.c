#include "sim_level.h"

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
};

cstring sim_level_serialize(sim_level level) {
	int n = list_size(graph_keys(level->level));
	cstring s = cstring_fromInt(n);
	cstring_write(s, "\n\n");
	int i = 0, j = 0;
	for (i=0; i<n; i++) {
		cstring city = list_get(graph_keys(level->level),i);
		cstring_write(s, city);
		cstring_write(s, "\n");
		map meds = (map)(graph_node_value(graph_get_node(level->level, city)));
		list medNames = map_keys(meds);
		for (j=0; j<list_size(medNames); j++) {
			cstring key = list_get(medNames, j);
			int *value = map_get(meds, key);
			sim_keypair kp = (sim_keypair)sim_keypair_init(key, *value);
			s = cstring_write(s, sim_keypair_serialize(kp));
			s = cstring_write(s, "\n");
			sim_keypair_free(kp);
		}
		s = cstring_write(s, "\n");
	}
	for (i=0; i<n; i++) {
		cstring from = list_get(graph_keys(level->level), i);
		list tos = graph_node_arcs(graph_get_node(level->level, from));
		for (j=0; j<list_size(tos); j++){
			cstring to = graph_node_key(graph_arc_to(list_get(tos, j)));
			int weight = graph_arc_weight(list_get(tos, j));
			s = cstring_write(s, from);
			s = cstring_write(s, " ");
			s = cstring_write(s, to);
			s = cstring_write(s, " ");
			s = cstring_write(s, cstring_fromInt(weight));
			s = cstring_write(s, "\n");
		}
		s = cstring_write(s, "\n");
	}
	return s;
}

sim_level sim_level_deserialize(cstring s) {
	sim_level l = malloc(sizeof(struct sim_level));
	l->level = graph_init((comparer)cstring_compare, (cloner)cstring_copy);
	int i = 0;
	int nc = cstring_parseInt(s, &i);
	while (s[i++]!='\n');
	while (s[i++]!='\n');
	int j = 0;
	int flag = TRUE;
	for (j = 0; j<nc; j++) {
		cstring key = cstring_copy_line(s+i);
		cstring_remove(key, '\n');
		while (s[i++]!='\n');
		
		map meds = map_init((comparer)cstring_compare, (cloner)cstring_copy);
		flag = TRUE;
		while (flag) {
			sim_keypair kp = sim_keypair_deserialize(cstring_copy_line(s+i));
			cstring medName = cstring_copy(kp->name);
			int *amount = malloc(sizeof(int));
			*amount = kp->amount;
			free(kp);
			map_set(meds, medName, amount);
			while (s[i++]!='\n');
			if (s[i] == '\n') {
				flag = FALSE;
				i++;
			}
		}
		graph_add_node(l->level, key, meds);
	}
	flag = TRUE;
	while (flag){
		cstring from = cstring_copy_till_char(s+i, ' ', 1);
		while (s[i++] != ' ');
		cstring to = cstring_copy_till_char(s+i, ' ', 1);
		while (s[i++] != ' ');
		int rc = 0;
		int weight = cstring_parseInt(s+i, &rc);
		while (s[i++] != '\n');
		if(s[i]=='\n')
			i++;
		if(s[i]==0)
			flag = FALSE;
		graph_add_arc(l->level, from, to, weight);
		cstring_free(from);
		cstring_free(to);
	}
	return l;
}

void sim_level_main(sim_level air) {
	// Inicializa el server, bindea sus funciones al server.
	// Las mismas se vinculan con el cliente del frond a forma de log.
}
