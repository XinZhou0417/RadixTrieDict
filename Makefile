IDIR = ./include
ODIR = ./build
LDIR = ./lib
BDIR = ./bin
SDIR = ./src

# Create the directory for target if it doesn't exist
dir_guard=@mkdir -p $(@D)

CC = gcc
CFLAGS = -Wall -g -I$(IDIR)

OBJ = $(ODIR)/my_stack.o \
	$(ODIR)/dictionary.o $(ODIR)/sorted_array_dictionary.o $(ODIR)/radix_tree_dictionary.o \
	$(ODIR)/cafe_data.o $(ODIR)/cafe_driver.o \
	$(ODIR)/server.o

$(ODIR)/%.o : $(SDIR)/%.c
	$(dir_guard)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/driver : $(OBJ)
	$(dir_guard)
	$(CC) -o $@ $^ $(CFLAGS)

all: $(BDIR)/driver

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(BDIR)/driver