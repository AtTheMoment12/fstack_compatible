TOPDIR=/home/lz/f-stack

ifeq ($(FF_PATH),)
	FF_PATH=${TOPDIR}
endif

ifeq ($(FF_DPDK),)
	FF_DPDK=${TOPDIR}/dpdk/x86_64-native-linuxapp-gcc
endif

LIBS+= -L${FF_PATH}/lib -Wl,--whole-archive,-lfstack,--no-whole-archive
LIBS+= -L${FF_DPDK}/lib -Wl,--whole-archive,-ldpdk,--no-whole-archive
#LIBS+= -L../msgQueue -Wl,--whole-archive,-lmsgQueue,--no-whole-archive
LIBS+= -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -pthread -lnuma 



TARGET="monitor"
all:
	#cc -O -gdwarf-2 -g -I../  -o ${TARGET} *.c  ${LIBS}  
	gcc -o monitor main.c -g  ${LIBS}

.PHONY: clean
clean:
	rm -f *.o *.a ${TARGET} 
