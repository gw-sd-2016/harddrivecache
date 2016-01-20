OBJS=heap_test.o list.o heap.o 
CFLAGS=-O3 -I. -w
LIBS=-pthread
CC=gcc
BIN=heap_test
DEPS=heap.h list.h
	
all:
	make $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	rm $(OBJS)
	rm $(BIN)
