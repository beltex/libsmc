CC     = cc
CFLAGS = -O2 -Wall
INC    = -framework IOKit
EXEC   = c-smc

$(EXEC) : src/smc.c examples/tmp.c
	$(CC) $(CFLAGS) $(INC) -o $@ $?

clean: 
	rm $(EXEC)
