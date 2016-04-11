#!/bin/bash
gcc weighted.c -o weighted
rm /home/timstamler/harddrivecache/results/large*.txt
cd /home/timstamler/harddrivecache/dev
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=33554432
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/large1.txt
make clean
make SIZE=67108864
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/large2.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=134217728
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/large3.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=268435456
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/large4.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=536870912
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/large5.txt
make clean
