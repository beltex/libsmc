CC     = cc
CFLAGS = -O2 -Wall
INC    = -framework IOKit
PREFIX = /usr/local
EXEC   = c-smc

build : $(EXEC)

clean : 
	rm $(EXEC)

$(EXEC) : src/smc.c
	$(CC) $(CFLAGS) $(INC) -o $@ $?

install : $(EXEC)
	install $(EXEC) $(PREFIX)/bin
