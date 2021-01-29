

LIBS+= -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -pthread -lnuma -shared -fPIC

SOURCE := $(wildcard *.c)
OBJS   := $(patsubst %.c,%.o,$(SOURCE))


TARGET="libmsgQueue.a"
all:$(OBJS)
	cc -O -gdwarf-2 -g  -o ${TARGET} *.c   ${LIBS}  

.PHONY: clean
clean:
	rm -f *.o *.a ${TARGET} 
