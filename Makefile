# Fill this makefile with the makefile format instructions to 
#   build your program.
CC = gcc
CFLAGS = -g -Wall
LIB = 

SRC1 = driver.c data.c dictionary.c
OBJ1 = $(SRC1:.c=.o)

SRC2 = driver.c data.c dictionary.c sorted_array_dictionary.c
OBJ2 = $(SRC2:.c=.o)

SRC3 = driver.c data.c dictionary.c sorted_array_dictionary.c my_stack.c radix_tree_dictionary.c
OBJ3 = $(SRC3:.c=.o)

all : dict1 dict2 dict3

dict1: $(OBJ1)
	$(CC) $(CFLAGS) -o dict1 $(OBJ1) $(LIB)

dict2: $(OBJ2) 
	$(CC) $(CFLAGS) -o dict2 $(OBJ2) $(LIB)

dict3: $(OBJ3) 
	$(CC) $(CFLAGS) -o dict3 $(OBJ3) $(LIB)


driver.o: driver.c data.h dictionary.h sorted_array_dictionary.h radix_tree_dictionary.h

data.o: data.c data.h

dictionary.o: dictionary.c dictionary.h

sorted_array_dictionary.o: sorted_array_dictionary.c sorted_array_dictionary.h

my_stack.o: my_stack.c my_stack.h

radix_tree_dictionary.o: radix_tree_dictionary.c radix_tree_dictionary.h my_stack.h

clean:
	rm -f *.o dict1 dict2 dict3
