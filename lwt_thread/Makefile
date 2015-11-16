OBJS=example.o lwt.o
CFLAGS=-O3 -I. -w -m32
LIBS=-pthread
DEPS=lwt.h
CC=gcc
BIN=lwt_test
	
all:
	make $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

clean:
	rm $(OBJS)
	rm $(BIN)
	
