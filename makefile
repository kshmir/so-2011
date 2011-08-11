# Actions of make makefile
default : show_usage

# START Declaration of requirements for each target

### Data Structures
includes = \
	bin/includes.o

data_structures_test = \
	bin/graph.o \
	bin/list.o \
	bin/stack.o \
	bin/tree.o \
	bin/map.o \
	bin/heap.o \
	bin/includes.o \
	bin/data_structures_test.o

	
### Examples
example_1 = \
	bin/ejemplo_1.o
	
### Utils	
utils = \
	bin/cstring.o \
	bin/utils_test.o

### TP1
tp1 = \
	bin/tp1.o
	
	
### Flags and declarations	
cc = gcc -m32 -g 


# END of Declaration of requirements for each target


# START Declaration of object files creation


### Data structures tests

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
		
		
#### Global Test File for Data Structures
data_structures_tests_data_structures_test: 
		$(cc) -o bin/data_structures_test.o \
			  -c src/data_structures/tests/data_structures_test.c
			  
utils_cstring: 
		$(cc) -o bin/cstring.o -c src/utils/cstring.c		                    
		
utils_test_build: 
	$(cc) -o bin/utils_test.o -c src/utils/utils_test.c		                    
		
### Includes
includes: 
		$(cc) -o bin/includes.o -c src/includes.c		
		
# END Declaration of object files creation
		


	
# START Declaration of targets


### All targets must move to bin
clear_data_structures_test :
	rm bin/*.o;
	rm data_structures_test;
	
clear_utils_test:
	rm bin/*.o;
	rm utils_test;	

### Data structures tests
data_structures_test : data_structures_graph data_structures_list data_structures_heap \
	data_structures_stack data_structures_tree data_structures_map includes \
	data_structures_tests_data_structures_test 
	$(cc) -o data_structures_test $(data_structures_test)
	
utils_test :  utils_cstring utils_test_build
	$(cc) -o utils_test $(utils)

### Generates tp1
tp1: 
	gcc -o tp1.o $(tp1)

# END Declaration of targets



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

