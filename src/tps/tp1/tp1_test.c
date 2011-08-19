#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "networking/sim_server.h"
#include "networking/sim_transporter.h"
#include "sim_common_reqs.h"

void core_test() {
	
	
	
}



void file_test(){
	printf("%s", cstring_from_file("./testFile"));
}

struct thread_data {
	sim_transporter t;
	int	index;
};

void_p listen(struct thread_data * _data) {
	int i = 0, count = 0;
	for (i = 0; i < 200; i++) {
		cstring data = sim_transporter_listen(_data->t);
		assert(cstring_compare(data, "cliente") == 0);
	}
}

void networking_test() {
	sim_transporter t = sim_transporter_fork(C_PIPE, P_TESTER, 0, 1);
	pthread_t thread[200];
	struct thread_data data[200];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int i = 0;	
	for (; i < 200; i++) {
		data[i].t = t;
		data[i].index = i;
		pthread_create(&thread[i], &attr, (void_p)listen, (void_p)&data[i]);
	}

	i = 0;
	for (; i < 200; i++) {
		int s = 0;
		pthread_join(thread[i], (void_p) &s);
	}
	
	printf("Server got all data via threads\n");
	sim_transporter_write(t, "server");
	
}

void serializing_test(){
	
	//create the strings to be tested
	printf("Serializing & deserializing planes:\n");
	cstring plane = cstring_copy("Andorra\ndroga1 1 droga2 2 droga3 3\n");
	cstring airline = cstring_copy("2\n\nAndorra\ndroga1 1 droga2 2 droga3 3\n\nPeru\nPaco 1 Paco2 2 Pacojerlo 4\n\n");
	cstring level = cstring_copy("2\n\nAndorra\nPenicilina 300\nFaso 20\n\nPeru\nCaramelos 2000\n\nAndorra Peru 30\n\n");
	//deserialize them
	sim_plane p = sim_plane_deserialize(plane, -1);
	sim_airline a = sim_airline_deserialize(airline, -1);
	sim_level l = sim_level_deserialize(level);
	
	//serialize them
	cstring s1 = sim_plane_serialize(p);
	cstring s2 = sim_airline_serialize(a, FALSE);
	cstring s3 = sim_level_serialize(l);
	
	l = sim_level_deserialize(s3);
	cstring s4 = sim_level_serialize(l);
	//print them
	//printf("%s\n%s\n%s\n",s1,s2,s3);
	assert(!cstring_compare(plane, s1));
	assert(!cstring_compare(airline, s2));
	assert(!cstring_compare(s3, s4));
	printf("DONE!\n");
}

int main(int argc, char ** params) {
	
	//file_test();
	serializing_test();
	//networking_test();
	return 0;
}