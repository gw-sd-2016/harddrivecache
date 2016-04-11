#!/bin/bash
gcc weighted.c -o weighted
rm /home/timstamler/harddrivecache/results/weighted*.txt
cd /home/timstamler/harddrivecache/dev
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=33554432
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/weighted1.txt
make clean
make SIZE=67108864
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/weighted2.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=134217728
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/weighted3.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=268435456
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/weighted4.txt
rm -rf /home/timstamler/harddrivecache/mnt/ssd/*
make clean
make SIZE=536870912
make umount
./hdc_fs
/home/timstamler/harddrivecache/bench/weighted >> /home/timstamler/harddrivecache/results/weighted5.txt
make clean
