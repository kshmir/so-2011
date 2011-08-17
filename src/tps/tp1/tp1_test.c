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
	printf("%s/n", cstring_from_file("testFile"));
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

int main(int argc, char ** params) {
	
	//file_test();
	networking_test();
	//sleep(1);
	return 0;
}
