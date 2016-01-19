OBJS=hdc_fs.o cache.o hash.o list.o heap.o 
CFLAGS=-O3 -I. -w
LIBS=-pthread
CC=gcc
BIN=hdc_fs
FUSE=`pkg-config fuse --cflags --libs`
DEPS=log.h disk_info.h cache.h hash.h list.h
	
all:
	make $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LIBS) $(FUSE)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS) $(FUSE)

clean:
	rm $(OBJS)
	rm $(BIN)

umount:	
	fusermount -u ../fs
