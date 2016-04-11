#!/bin/bash
gcc bench.c -o bench
rm /home/timstamler/harddrivecache/results/random*.txt
cd /home/timstamler/harddrivecache/dev
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=33554432
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/bench >> /home/timstamler/harddrivecache/results/random1.txt
make clean
make SIZE=67108864
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/bench >> /home/timstamler/harddrivecache/results/random2.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=134217728
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/bench >> /home/timstamler/harddrivecache/results/random3.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=268435456
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/bench >> /home/timstamler/harddrivecache/results/random4.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=536870912
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/bench >> /home/timstamler/harddrivecache/results/random5.txt
make clean
