CC     = cc
CFLAGS = -mmacosx-version-min=10.6 -std=c99 -arch x86_64 -O2 -Wall
INC    = -framework IOKit
EXEC   = c-smc

$(EXEC): src/smc.c examples/tmp.c
	$(CC) $(CFLAGS) $(INC) -o $@ $?

clean: 
	rm $(EXEC)
