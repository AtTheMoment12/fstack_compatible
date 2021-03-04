#!/bin/sh

while [ 1 ]
do
	ipcs -m | awk '$6==0 {print $2}' |
	while read s
	do
		ipcrm -m $s
		echo "shmid:$s be deleted"
	done
done
