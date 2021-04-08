#!/bin/bash
export RTE_SDK=/home/lz/f-stack/dpdk
export RTE_TARGET=x86_64-native-linuxapp-gcc
echo 4 > /sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages
echo 4 > /sys/devices/system/node/node1/hugepages/hugepages-1048576kB/nr_hugepages
mount -t hugetlbfs -o pagesize=1048576kB  none  /mnt/huge_1048576kB
modprobe uio
rmmod igb_uio
insmod $RTE_SDK/$RTE_TARGET/kmod/igb_uio.ko
ifconfig p4p2 down
$RTE_SDK/usertools/dpdk-devbind.py --bind=igb_uio 0000:82:00.1

