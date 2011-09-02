#//MARK: -
#//MARK: Default action
default: show_usage

#//MARK: -
#//MARK: Declarations of object for each target

#//MARK: ----- Data structures
includes = \
	bin/includes.o

data_structures_test = \
	bin/list.o \
	bin/graph.o \
	bin/stack.o \
	bin/tree.o \
	bin/map.o \
	bin/heap.o \
	bin/queue.o \
	bin/includes.o \
	bin/data_structures_test.o

data_structures = \
	bin/graph.o \
	bin/list.o \
	bin/stack.o \
	bin/tree.o \
	bin/map.o \
	bin/heap.o \
	bin/queue.o \
	bin/includes.o

#//MARK: ----- TP1
tp1 = \
	bin/sim_airline.o \
	bin/sim_extra.o \
	bin/sim_level.o \
	bin/sim_plane.o \
	bin/sim_client.o \
	bin/sim_message.o \
	bin/sim_server.o \
	bin/sim_transporter.o \
	bin/sim_msg_q_transporter.o \
	bin/sim_pipe_transporter.o \
	bin/sim_smem_transporter.o \
	bin/sim_socket_transporter.o \
	bin/sim_frontend.o \
	bin/sim_validator.o


#//MARK: ----- TP1 Test
tp1_test_files = \
	bin/sim_airline.o \
	bin/sim_extra.o \
	bin/sim_level.o \
	bin/sim_plane.o \
	bin/sim_client.o \
	bin/sim_message.o \
	bin/sim_server.o \
	bin/sim_transporter.o \
	bin/sim_msg_q_transporter.o \
	bin/sim_pipe_transporter.o \
	bin/sim_smem_transporter.o \
	bin/sim_socket_transporter.o \
	bin/tp1_test.o 

#//MARK: ----- TP1 Test	Child Process
tp1_test_child_files = \
	bin/sim_airline.o \
	bin/sim_extra.o \
	bin/sim_level.o \
	bin/sim_plane.o \
	bin/sim_client.o \
	bin/sim_message.o \
	bin/sim_server.o \
	bin/sim_transporter.o \
	bin/sim_msg_q_transporter.o \
	bin/sim_pipe_transporter.o \
	bin/sim_smem_transporter.o \
	bin/sim_socket_transporter.o \
	bin/tp1_test_child.o 

#//MARK: ----- TP1 Test	Server Process	
tp1_test_server_files = \
	bin/sim_airline.o \
	bin/sim_extra.o \
	bin/sim_level.o \
	bin/sim_plane.o \
	bin/sim_client.o \
	bin/sim_message.o \
	bin/sim_server.o \
	bin/sim_transporter.o \
	bin/sim_msg_q_transporter.o \
	bin/sim_pipe_transporter.o \
	bin/sim_smem_transporter.o \
	bin/sim_socket_transporter.o \
	bin/tp1_test_server.o 

#//MARK: ----- Examples declarations
example_1 = \
	bin/ejemplo_1.o
	
	
#//MARK: ----- Utils declarations
utils_test_files = \
	bin/cstring.o \
	bin/sem.o \
	bin/shm.o \
	bin/unixColors.o \
	bin/utils_test.o
	
utils = \
	bin/unixColors.o \
	bin/sem.o \
	bin/shm.o \
	bin/cstring.o 



#//MARK: ----- GCC Declarations
### Flags and declarations	

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
	cc = gcc -m32 -g -lpthread
else
	cc = gcc -m32 -g 
endif


#//MARK: -
#//MARK: Object file compilations

#//MARK: ----- Data structures
#### Graph
data_structures_graph:		
		$(cc) -o bin/graph.o -c src/data_structures/graph.c
#### List
data_structures_list: 
		$(cc) -o bin/list.o -c src/data_structures/list.c
#### Stack
data_structures_stack: 
		$(cc) -o bin/stack.o -c src/data_structures/stack.c		
#### Tree
data_structures_tree: 
		$(cc) -o bin/tree.o -c src/data_structures/tree.c		
#### Map
data_structures_map: 
		$(cc) -o bin/map.o -c src/data_structures/map.c		
#### Heap
data_structures_heap: 
		$(cc) -o bin/heap.o -c src/data_structures/heap.c	
#### Heap
data_structures_queue: 
		$(cc) -o bin/queue.o -c src/data_structures/queue.c		                    
		
		
#### Global Test File for Data Structures
data_structures_tests_data_structures_test: 
		$(cc) -o bin/data_structures_test.o \
			  -c src/data_structures/tests/data_structures_test.c

### End of data structures


#//MARK: ----- Utils
			  
utils_cstring: 
	$(cc) -o bin/cstring.o -c src/utils/cstring.c		                    
	
utils_sem: 
	$(cc) -o bin/sem.o -c src/utils/sem.c		                    
utils_shm: 
	$(cc) -o bin/shm.o -c src/utils/shm.c		                    
		
utils_test_build: 
	$(cc) -o bin/utils_test.o -c src/utils/utils_test.c		                    
	
### End of Utils
		
### Includes
includes: 
		$(cc) -o bin/includes.o -c src/includes.c		
### End of Includes

#//MARK: ----- TP1

sim_airline: 
	$(cc) -o bin/sim_airline.o -c src/tps/tp1/core/sim_airline.c

sim_extra: 
	$(cc) -o bin/sim_extra.o -c src/tps/tp1/core/sim_extra.c
  
sim_level: 
	$(cc) -o bin/sim_level.o -c src/tps/tp1/core/sim_level.c

sim_plane: 
	$(cc) -o bin/sim_plane.o -c src/tps/tp1/core/sim_plane.c
  
sim_client: 
	$(cc) -o bin/sim_client.o -c src/tps/tp1/networking/sim_client.c
  
sim_message: 
	$(cc) -o bin/sim_message.o -c src/tps/tp1/networking/sim_message.c
  
sim_server: 
	$(cc) -o bin/sim_server.o -c src/tps/tp1/networking/sim_server.c
  
sim_transporter: 
	$(cc) -o bin/sim_transporter.o -c src/tps/tp1/networking/sim_transporter.c
  
sim_msg_q_transporter: 
	$(cc) -o bin/sim_msg_q_transporter.o -c src/tps/tp1/networking/transporters/sim_msg_q_transporter.c
  
sim_pipe_transporter: 
	$(cc) -o bin/sim_pipe_transporter.o -c src/tps/tp1/networking/transporters/sim_pipe_transporter.c
  
sim_smem_transporter: 
	$(cc) -o bin/sim_smem_transporter.o -c src/tps/tp1/networking/transporters/sim_smem_transporter.c
  
sim_socket_transporter: 
	$(cc) -o bin/sim_socket_transporter.o -c src/tps/tp1/networking/transporters/sim_socket_transporter.c
	
sim_frontend: 
	$(cc) -o bin/sim_frontend.o -c src/tps/tp1/app/sim_frontend.c
	
sim_validator: 
	$(cc) -o bin/sim_validator.o -c src/tps/tp1/app/sim_validator.c
	
sim_tp1: 
	$(cc) -o bin/tp1.o -c src/tps/tp1/tp1.c
	
sim_tp1_airline: 
	$(cc) -o bin/tp1_airline.o -c src/tps/tp1/tp1_airline.c
	
sim_tp1_level: 
	$(cc) -o bin/tp1_level.o -c src/tps/tp1/tp1_level.c

sim_tp1_test: 
	$(cc) -o bin/tp1_test.o -c src/tps/tp1/tests/tp1_test.c
	
sim_tp1_test_child: 
	$(cc) -o bin/tp1_test_child.o -c src/tps/tp1/tests/tp1_test_child.c
	
sim_tp1_test_server: 
	$(cc) -o bin/tp1_test_server.o -c src/tps/tp1/tests/tp1_test_server.c

colors: 
	$(cc) -o bin/colors.o -c src/utils/colors.c

noColors: 
	$(cc) -o bin/noColors.o -c src/utils/noColors.c
	
unixColors: 
	$(cc) -o bin/unixColors.o -c src/utils/unixColors.c
	
winColors: 
	$(cc) -o bin/winColors.o -c src/utils/winColors.c

#//MARK: ----- Targets


### All targets must move to bin
clear_data_structures_test:
	rm bin/*.o;
	rm data_structures_test;
	
clear_utils_test:
	rm bin/*.o;
	rm utils_test;	

### Data structures tests
data_structures_test: data_structures_graph data_structures_list data_structures_heap \
	data_structures_stack data_structures_tree data_structures_map includes \
	data_structures_queue \
	data_structures_tests_data_structures_test 
	$(cc) -o data_structures_test $(data_structures_test)
	
utils_test:  utils_cstring utils_test_build utils_sem utils_shm data_structures_list unixColors
	$(cc) -o utils_test $(utils_test_files) $(data_structures)

build_tp1: data_structures_graph \
	data_structures_list \
	data_structures_heap \
	data_structures_stack \
	data_structures_tree \
	data_structures_queue \
	data_structures_map includes \
	sim_airline \
	sim_extra \
	sim_level \
	sim_plane \
	sim_client \
	sim_message \
	sim_server \
	sim_transporter \
	sim_msg_q_transporter \
	sim_pipe_transporter \
	sim_smem_transporter \
	sim_socket_transporter \
	sim_frontend \
	sim_validator

### Generates tp1
tp1: build_tp1 \
	sim_tp1 \
	sim_tp1_level \
	sim_tp1_airline \
	utils_cstring utils_sem utils_shm unixColors
	$(cc) -o tp1 $(tp1) $(data_structures) $(utils) bin/tp1.o
	$(cc) -o tp1_level $(tp1) $(data_structures) $(utils) bin/tp1_level.o
	$(cc) -o tp1_airline $(tp1) $(data_structures) $(utils) bin/tp1_airline.o

tp1_test: build_tp1 \
	sim_tp1_test \
	sim_tp1_test_child \
	sim_tp1_test_server \
	utils_cstring utils_sem utils_shm unixColors
	$(cc) -o tp1_test_child $(tp1_test_child_files) $(data_structures) $(utils)
	$(cc) -o tp1_test_server $(tp1_test_server_files) $(data_structures) $(utils)
	$(cc) -o tp1_test $(tp1_test_files) $(data_structures) $(utils)



#//MARK: ----- Usage

# Usage print script
show_usage:

	@echo "--------------------------------------------"
	@echo "MAKEFILE USAGE DOCUMENTATION"
	@echo "\nSISTEMAS OPERATIVOS - ITBA - 2011"
	@echo "\nAlumnos:"
	@echo "\tMARSEILLAN, AGUSTIN"
	@echo "\tPEREYRA, CRISTIAN"
	@echo "\tVIDELA, MAXIMO"


	@echo "--------------------------------------------"

	@echo "Usage: \"make {action}\"\n"
	
	@echo "\taction:"
	@echo "\t\tfinal binaries:"
	@echo "\t\t\ttp{number}(_test)"
	@echo "\t\t\texample{number}"
	@echo "\t\ttests:"
	@echo "\t\t\tdata_structures_test"
	@echo "\t\t\tutils_test"
	@echo "\t\tcleaning:"
	@echo "\t\t\tclean_{target}"
	
	
	@echo "--------------------------------------------"
	@echo "Examples:"
	
	@echo "\t\nCompiling a TP:\n"
	@echo "\t\"make tp1\""
	@echo "\t\tIt compiles the tp number 1"
	@echo "\t\"make tp1_test\""
	@echo "\t\tRuns the tests for the tp number 1"


	@echo "\t\nCompiling an example:\n"
	@echo "\t\"make example1\""
	@echo "\t\tIt compiles the example number 1"
	
	
	@echo "\t\"make clean_tp1_test\""
	@echo "\t\tCleans the tests for the tp1"	
	
	@echo "\nNotes:"
	@echo "Only TP1 is available for now"
	@echo "Example 1: Threads"
	@echo "Example 2: Fork/exec/wait"
	@echo "--------------------------------------------"	
 