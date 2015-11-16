OBJS=cache_fs.o 
CFLAGS=-O3 -I. -w
LIBS=-pthread
CC=gcc
BIN=cache_fs
FUSE=`pkg-config fuse --cflags --libs`
DEPS=log.h
	
all:
	make $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LIBS) $(FUSE)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS) $(FUSE)

clean:
	rm $(OBJS)
	rm $(BIN)
	fusermount -u ../fs
