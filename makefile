# Actions of make makefile
default : show_usage

# START Declaration of requirements for each target

### Data Structures
data_structures_test = \
	bin/graph.o \
	bin/list.o \
	bin/data_structures_test.o
	
### Examples	
example_1 = \
	bin/ejemplo_1.o

### TP1
tp1 = \
	bin/tp1.o
	
	
### Flags and declarations	
cc = gcc -g


# END of Declaration of requirements for each target


# START Declaration of object files creation


### Data structures tests

#### Graph
data_structures_graph:		
		$(cc) -o bin/graph.o -c src/data_structures/graph.c
#### List
data_structures_list: 
		$(cc) -o bin/list.o -c src/data_structures/list.c
		
#### Global Test File for Data Structures
data_structures_tests_data_structures_test: 
		$(cc) -o bin/data_structures_test.o \
			  -c src/data_structures/tests/data_structures_test.c
		
		
		
# END Declaration of object files creation
		


	
# START Declaration of targets


### All targets must move to bin
clear_data_structures_test :
	rm bin/*.o;
	rm data_structures_test;

### Data structures tests
data_structures_test : data_structures_graph data_structures_list \
	data_structures_tests_data_structures_test 
	$(cc) -o data_structures_test $(data_structures_test)
	

### Generates tp1
tp1: 
	gcc -o tp1 $(tp1) $(Front)  $(Flags)

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
	@echo "\t\ttp{number}(_test)"
	@echo "\t\tdata_structures_test"
	@echo "\t\texample{number}"
	@echo "\t\tclean_{target}"
	
	
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

