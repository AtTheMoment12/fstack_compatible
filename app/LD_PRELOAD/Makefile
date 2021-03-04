
FT_BINARY = libft.so
FF_LIBS+= -L${FF_PATH}/lib -Wl,--whole-archive,-lfstack,--no-whole-archive
FF_LIBS+= -L${FF_DPDK}/lib -Wl,--whole-archive,-ldpdk,--no-whole-archive
#FF_LIBS+= -L${FF_PATH}/app/LD_PRELOAD/msgQueue -Wl,--whole-archive,-lmsgQueue,--no-whole-archive
FF_LIBS+= -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -lpthread -lnuma

C_ARGS = -Wall -g -O -fPIC  -O0 -DMT_REAL_TIME 

CC := gcc
LD := ld
LDFLAGS := -shared -fPIC -ldl
SOURCE := $(wildcard *.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))
TARGET_LIB := libft.so

all:$(OBJS) 
	#$(LD) $(LDFLAGS) -o $(TARGET_LIB) $(OBJS) 
	$(CC)  -o $(TARGET_LIB) ft_hook.o ft_sys_hook.o $(FF_LIBS) $(LDFLAGS)

%.o:%.c
	$(CC) -D_GNU_SOURCE -c $(LDFLAGS) $< -o $@ $(FF_LIBS)

.PHONY:clean
#ft_sys_hook.o: ft_sys_hook.cpp
#	@echo -e Compiling $(GREEN)$<$(RESET) ...$(RED)
#	@$(CC) $(C_ARGS) -c -o $@ $<  $(CRESET)

#ft_hook.o: ft_hook.c
#	@echo -e Compiling $(GREEN)$<$(RESET) ...$(RED)
#	@$(CC) $(C_ARGS) -c -o $@ $<  $(CRESET)  

clean:
	@rm -f $(FT_BINARY) *.so *.a *.o echo  


#LIB_OBJ = ft_sys_hook.o ft_hook.o 
#LIB_OBJ = ft_sys_hook.cpp ft_hook.c

#libft.so:$(LIB_OBJ)
#	@echo -e  Linking $(CYAN)$@$(RESET) ...$(RED)
#	@-rm -f $@ 
#	@ar crs  $@  $^ $(CRESET)
#	@chmod +x $@ 

#libft.so:	$(LIB_OBJ)
#	@echo -e Compile $(CYAN)$@$(RESET) ...$(RED)
#	@$(CC) -lstdc++ -ldl -lm -O -gdwarf-2 -o libft.so   $(C_ARGS) --shared $(LIB_OBJ)   $(FF_LIBS) $(CRESET)
