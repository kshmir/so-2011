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
	sim_plane p = malloc(sizeof(struct sim_plane));
	p->id = plane_id;
	p->start_city = cstring_copy_line(s);
	cstring_remove(p->start_city, '\n');
	while (*(s++) != '\n');
	s = cstring_copy(s);
	cstring_remove(s, '\n');
	s = cstring_write_c(s, ' ');
	p->medicines_keys = list_init();
	p->medicines = map_init((comparer)cstring_compare, (cloner)cstring_copy);
	int flag = TRUE;
	while(flag){
		sim_keypair kp = sim_keypair_deserialize(cstring_copy_till_char(s, ' ', 2));
		cstring key = cstring_copy(kp->name);
		int *value = malloc(sizeof(int));
		*value = kp->amount;
		list_add(p->medicines_keys, key);
		map_set(p->medicines, key, value);
		while (*(s++) != ' ');
		while (*(s++) != ' ');
		flag = *s != '\0';
		free(kp);
	}
	return p;
}

cstring sim_plane_serialize(sim_plane p) {
	cstring s = cstring_copy(p->start_city);
	s = cstring_write(s,"\n");
	foreach(n, p->medicines_keys) {
		cstring key = (cstring) list_node_value(n);
		int * value = map_get(p->medicines, key);
		sim_keypair kp = (sim_keypair) sim_keypair_init(key, *value);
		s = cstring_write(s,sim_keypair_serialize(kp));
		s = cstring_write(s," ");
		sim_keypair_free(kp);
	}
	s[cstring_len(s)-1] = '\n';
	return s;
}

void sim_plane_main(sim_plane air) {

}

void sim_plane_print(sim_plane p){
	printf("%s\n",p->start_city);
	int i = 0;
	for(i = 0; i<list_size(p->medicines_keys); i++){
		cstring key = list_get(p->medicines_keys, i);
		int *value = (int*)(map_get(p->medicines, key));
		printf("%s ", key);
		printf("%d\n",*value);
	}
}
